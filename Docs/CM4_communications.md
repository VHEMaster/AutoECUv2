# Dual-core communication ownership

The communication runtime executes on CM4. CM7 remains the owner of the engine,
configuration defaults, persistent configuration, and the global configuration FSM.
The firmware images must be built and flashed together from the same revision.

## Ownership

| Layer / resource | Owner |
| --- | --- |
| Engine control, sensors, actuators, SPI/QSPI, engine parameter storage | CM7 |
| Configuration defaults, flash layout/version conversion, configuration FSM | CM7 |
| CAN driver, FDCAN1 handle, MSP and interrupt handlers | CM4 |
| USART1, USART3 and DMA1 streams 0, 1, 4, 5 | CM4 |
| ISO-TP, OBD2, UDS, KWP, Router and Signals runtime | CM4 |
| Existing USB device files | CM4 |
| System clocks and shared TIM2/TIM5 counters | CM7 |
| IPC transport and parameter proxies | Both |

CM4 mirrors CM7's `ECU/Communication`, `ECU/Config`, `ECU/Drivers`,
`ECU/Common`, and `ECU/MiddleLayer` hierarchy. Shared configuration/type headers
remain authoritative in CM7; thin CM4 include adapters prevent definition drift.
Neither core dereferences the other core's context pointers or calls its callbacks.
Protocol callbacks and their userdata remain entirely within CM4.

KWP and parts of UDS were scaffolding in the starting revision. This change moves
the existing implementations and UART hardware ownership; it does not add missing
K-line framing/initialization or new diagnostic services. USB initialization remains
as in the starting revision.

## D2 SRAM layout

| Address | Size | Use |
| --- | ---: | --- |
| `0x30000000`–`0x3002ffff` | 192 KiB | CM4 data, BSS, heap, stack |
| `0x30030000`–`0x30047fff` | 96 KiB | Shared `.ipc_shared`, `NOLOAD` in both images |

The shared structure occupies 88,224 bytes. Both linker scripts reserve exactly the
same range and assert the section size. CM7 no longer offers D2 private RAM to its
linker. The CM4 stack is at the end of its private range, outside IPC.

RM0399 calls D2 SRAM **AHB SRAM**, on the 32-bit D2 matrix. D1 AXI SRAM is at
`0x24000000`. The former CM4 `0x10000000` mapping is an alias of D2 SRAM, not
an additional bank. The new scripts use the common `0x30000000` address view.

CM7's MPU maps D2 as shareable, noncacheable, execute-never Normal memory. Its
512 KiB region also covers the unused address hole after physical D2 SRAM; no
allocation uses that hole. The mapping applies to CM7 only; CM4's code executes
from its own flash bank. Shared publication uses aligned 32-bit acquire/release
operations (DMB on Cortex-M), rather than volatile alone or cache-line invalidation.

References: [ST RM0399, embedded SRAM and bus matrix](https://www.st.com/resource/en/reference_manual/dm00176879-stm32h745755-and-stm32h747757-advanced-armbased-32bit-mcus-stmicroelectronics.pdf),
[ST AN5557, dual-core architecture](https://www.st.com/resource/en/application_note/an5557-stm32h745755-and-stm32h747757-lines-dualcore-architecture-stmicroelectronics.pdf).

## Boot and execution

1. CM4 initializes its private C runtime and enters the HSEM0 boot wait.
2. CM7 configures clocks and the MPU, enables the D2 SRAM clocks, clears IPC,
   publishes the ABI/readiness header and completes stores with DSB. It releases
   HSEM0 after finishing peripheral and application initialization, so the cores
   do not race clock-enable register updates during board startup.
3. CM4 updates its clock variable and HAL tick, initializes its communication
   peripherals/contexts, then publishes readiness. It never reinitializes CM7's
   engine timers, GPIO configuration or watchdogs.
4. CM7's existing configuration FSM submits reset/configure operations and polls
   their results. Engine parameter lookup starts only after CM7 has initialized it.
5. CM4's foreground loop services RPC, protocols and parameter mailboxes. Slow
   communication callbacks run every 1 ms using TIM5. The former empty 10 kHz
   communication SWI is no longer scheduled on CM7.

CM4 reconfiguration raises BASEPRI to mask the communication IRQs while tables
and contexts change. SysTick runs at priority 0 so HAL timeouts can still advance.
Reset work completes before its RPC reply is published. Signals reset preserves
its initialization and buffer bindings. FDCAN CAL, both FDCAN1 interrupts, UART
interrupts and their DMA interrupts belong only to CM4.

## RPC contract

`ipc_rpc_call(core, key, input, input_size, output, output_size)` is a copied-message
API: core 0 is CM7, core 1 is CM4. There are two bounded slots in each direction,
with a 16 KiB payload per slot. Slot state transitions are
`FREE -> BUILDING -> REQUEST -> REPLY -> FREE`.

- The first accepted call returns `E_AGAIN`. Retry the same key and unchanged
  arguments to collect the terminal result. One logical transaction owns a key
  until it collects that result. Distinct keys may complete in a different order.
- A full queue or an already active local producer returns `E_AGAIN` immediately.
  The local producer guard also permits calls from a preempting interrupt without
  interrupt masking, lock waiting, or slot corruption.
- `E_OK` means the remote handler completed, not just that a request was queued.
  Remote errors propagate unchanged. Response buffer overflow is explicit.
- The server may return `E_AGAIN` to defer completion; it must retain idempotent
  progress. The request payload remains owned by the slot until completion.
- No RPC waits for another core, spins for readiness, allocates memory, sends raw
  context pointers, or invokes a callback on another core.

CM7's existing `ecu_comm_*_get_default_config()` calls remain local. Its
`ecu_comm_*_configure()` and `ecu_comm_*_reset()` calls now use RPC and the
configuration FSM's existing `E_AGAIN` convention. Runtime context/transport APIs
(`ecu_comm_get_*_ctx`, ISO-TP frames, CAN callbacks) are CM4-internal; engine code
continues to use parameter IDs and the configuration interface.

The CM7 RPC server also implements `COMM_IPC_PARAMETER_READ` and
`COMM_IPC_PARAMETER_WRITE` using `comm_ipc_parameter_rpc_t`. CM4 can submit these
through the same transport for individual parameter transactions. Continuous
Signals/OBD2 exchange uses the less expensive mailbox path below.

ABI mismatch or a peer that has not started leaves calls pending with `E_AGAIN`.
There is no automatic retry deadline or independent-core reset recovery. A whole
system reset restarts both endpoints and clears all outstanding transactions.
A caller must continue polling accepted calls; abandoning a reply retains its slot.

## Parameter proxies

CM4 resolves the existing common-parameter API to stable **local** values.
Subscriptions cross the boundary as parameter IDs. CM7 validates each ID against
its authoritative parameter registry and publishes the lookup result. CM4 returns
`E_AGAIN` until resolution and the first copied snapshot are available. OBD2
configuration pre-registers its linked parameters before accepting traffic;
Signals configuration naturally retries while its parameter links resolve.

Each subscription has one SPSC value mailbox in each direction. Value, validity
and TIM5 timestamp are copied before the producer publishes `pending`. The
consumer acknowledges only after copying the entire record, so there are no
partially read snapshots. CM7 briefly masks local interrupts when copying an
engine value. CM4 processes protocol state and its mirrors in one foreground
context.

Signals upstream writes and invalidations mark their local values dirty. A busy
mailbox retains the latest dirty value for a later pass; engine snapshots cannot
roll those values back. Reconfiguring Signals stops its old runtime, waits for
outstanding value publications to be acknowledged, and releases the old local
write ownership before rebuilding parameter links. CM7 applies incoming values by the same configured
parameter ID used by the former single-core implementation. Downstream mirrors
become invalid after 100 ms without a fresh engine snapshot; upstream validity
continues to follow the configured Signals message timeout. OBD2 value reads
return `E_NOTRDY` for invalid mirrors instead of treating them as fresh values.

Each poll services at most 32 subscriptions with a rotating cursor. There are 512
unique IDs per system boot. Identical IDs share a subscription; reconfiguration
reuses existing IDs. Exhausting the registry returns `E_OVERFLOW`. Mailboxes
represent latest parameter state, not a lossless event history. Repeated writes
may coalesce while a peer is busy.

## Building and verification

Import both CubeIDE child projects, or use GNU Arm Embedded GCC and Python 3:

```sh
python3 Tools/build_firmware.py --core CM4 --configuration Debug
python3 Tools/build_firmware.py --core CM7 --configuration Debug
python3 Tools/build_firmware.py --core CM4 --configuration Release
python3 Tools/build_firmware.py --core CM7 --configuration Release
python3 Tests/run_ipc_tests.py
python3 Tools/check_dualcore.py --configuration Debug
python3 Tools/check_dualcore.py --configuration Release
```

The build helper reads source entries, linked resources, include paths, defines
and optimization settings from the checked-in CubeIDE projects. Debug uses `-Og`;
Release uses `-Ofast`. Set `ARM_CC` or pass `--cc` for a custom compiler path.
ELF files and maps go under `build/`. Host tests compile the production transport
and both sides of the production parameter bridge with small platform adapters.

Validated with GNU Arm GCC 13.2.1: both complete images link in Debug and Release.
Host coverage includes both RPC directions, full queues, copied inputs, propagated
errors, maximum payload, response capacity, 10,000 slot reuses, 4,000 concurrent RPC calls, 200,000 concurrent
value transfers, ID resolution, bidirectional value/validity transfer, first
snapshot gating, stale data, TIM5 wrap, invalid IDs, stale echo suppression, and reconfiguration of write ownership.

Hardware validation remains necessary: boot with both images, OBD2/ISO-TP traffic
under engine ISR load, CAN routing in both directions, reconfiguration during
traffic, message timeouts, and the actual CM7-to-CM4 propagation latency. Host
stress tests and successful linking do not measure board timing or peripheral
behavior.
