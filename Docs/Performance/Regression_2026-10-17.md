# Regression review of the last 24 hours

Reviewed snapshot: `f02a2a89c30aaf3a363cf31319c137c1b66821c6`.
Time window: **2026-10-16 01:46:57 through 2026-10-17 01:46:57 Europe/Kyiv**.
The 13 commits are based on `5ea8e8c`; the six newest follow `0ef0826`.
All changed source files were inspected. This branch adds tests and this report;
firmware sources, build configuration and master are unchanged.

## Findings

### P2: NaN guards are optimized away

`8a12b1f` adds `OPTIMIZE_FAST` to `math_interpolate_input`, which previously
honored its `value != value` guard in a global `-Og` build. The macro selects
`Ofast`, including assumptions that floating inputs are finite. The guard is
removed. The new `math_interpolate_1d_direct` also has an ineffective guard.

Locations at the reviewed snapshot:

- `CM7/Math/interpolation.c:12` and `:20`.
- `CM7/Math/interpolation.c:271` and `:279`.

Native reproduction uses a volatile IEEE-754 quiet-NaN pattern `0x7fc00000`,
axis `{0, 1}`, and output `{11, 22}`. With global `-Og`:

- The pre-day `math_interpolate_input` returns its zero-initialized result.
- The reviewed function returns a nonzero result structure.
- The direct function returns **11**, despite its explicit NaN `return 0`.

The numeric result is a native reproduction, not a claim that ARM must return
that same number for an input excluded by fast-math assumptions. The ARM GCC
13.2.1 assembly independently confirms the lost guard: the previous `-Og`
`math_interpolate_input` has `vcmp.f32 s0, s0` and an early return branch; the
reviewed function has no self-comparison and reads the table after its size test.

This is a defensive-input contract regression, not a reproduced failure on
ordinary finite CKP inputs. In a global `-Ofast` pre-day build the old guard was
already ineffective; the regression is that changing only the function attribute
also removes it in global `-Og`/`-Os` configurations.

A local fix can preserve the fast arithmetic, using integer classification of
binary32 NaN before entering the lookup:

```c
uint32_t value_bits;
memcpy(&value_bits, &value, sizeof(value_bits));
if((value_bits & 0x7fffffffu) > 0x7f800000u || size == 0)
  return result; /* return 0 in math_interpolate_1d_direct */
```

The strict `>` preserves the prior distinction between NaN and infinity; this
is not an unsolicited change to the infinity contract. A temporary-source
probe with this guard restores the expected results in both native profiles.
No firmware fix is committed here. Disabling fast-math globally is unnecessary
for this specific correction.

### Observable change: default outgoing CAN signals route disabled

`6101145` also changes
`CM7/ECU/Config/Communication/Src/config_router.c:29` from `true` to `false` for
the downstream `CAN_1 / SIGNALS_1 / 0x100..0x1FF` route. With this default
configuration those signal messages will no longer be routed out. The ISO-TP
OBD2 route (`0x7E0`, `0x7DF`, `0x7E8`) stays enabled.

This may be intentional. It is listed separately because it changes feature
behavior beyond timing optimization. Existing saved configurations are not
retroactively rewritten merely by changing the default initializer.

### Benign numerical change: ignition diagnostic means

`06fdef7` starts publishing the previously unwritten `advance_mean`. Its fused
accumulation also changes native rounding of `advance_requested_mean`.
Across the main scenarios the maximum old/new requested-mean difference was
**0.00000190734863 degrees**. Both means meet the independent double-precision
oracle bound documented in the test README. These fields have no read sites
in the current control code; cylinder advances and pulse events match exactly.
The new diagnostic update is intentional, so declaring full raw-state bitwise
equality without accounting for it would be incorrect.

## Whole-day differential results

Both global native profiles (`-Og`, `-Ofast`) pass against `5ea8e8c`, and also
against `0ef0826` to isolate the newer changes:

| Check, per profile/reference run | Result |
| --- | --- |
| 128,000 finite direct-vs-original lookup cases | Bit-identical |
| 200,000 finite clamp inputs plus boundary neighbors | Bit-identical |
| 20,000 position/history steps and both 8-cylinder batches | Bit-identical; one batch snapshot |
| 112 x 2,400 CKP updates | Runtime state identical except the separately validated mean diagnostics |
| 921,244 peripheral events | Exact order, arguments and pulse timestamps match |

Scenarios include 4/8 cylinders, one/two banks, phase acquisition/loss,
sequential/semi/distributor/mono modes, update triggers, slew, disabled
cylinders/groups, ignition acceptance, cutoff, MAP compensation/sensor failures,
and knock preparation/start/completion/overflow.

Additional focused checks pass in both native profiles:

| Check, per profile | Result |
| --- | --- |
| Real 60-2/36-2 decoders: 256,000 edges | Public data, diagnostics and private filter/history state match; 242,360 valid edges |
| 250,000 clock-helper cases | Same behavior with the actual configured 26-bit mask |
| 100,000 value/time interpolation pairs around wrap | Bit-identical floats/timestamps |
| 200,000 nested critical-section transitions | PRIMASK restoration preserved |
| 2,400,000 queue deadline cases | Wrap, late threshold and random inputs match |
| 100,000 density + 100,000 NTC cases | Physical-range assertions pass; maximum density difference 0.000000953674316 mg/cc at global Og, zero observed NTC difference |

The math-contract test additionally detects the known NaN failure and exits 1;
it must not be presented as an all-green test run. `--verify-nan-fix` applies
only the suggested guard to a temporary source copy and exits 0.

## ARM compile/link and ITCM

All **222 CM7/project-linked C translation units plus startup** compiled and
linked with ARM GCC 13.2.1, Cortex-M7, FPv5-D16 hard-float, DEBUG/CORE_CM7,
function/data sections, linker garbage collection and the project's actual
64 KiB ITCM linker script. Standard newlib/support libraries were explicitly
linked. Different CubeIDE/compiler/library settings may change exact sizes.

| Global profile | Linked ITCM bytes | Free bytes |
| --- | ---: | ---: |
| `-Og` | 59,552 | 5,984 |
| `-Os` | 54,544 | 10,992 |

`flash_fsm`/QSPI management execute from internal MCU Flash, not the external
QSPI device they control. Their relocation does not introduce execution from
an unavailable external Flash. No internal-Flash programming path was added.

## Commit-by-commit review map

| Commit | Change and review outcome |
| --- | --- |
| `f60d509` | IRQ priorities: CAN/UART 8 can preempt communication SWI 9; fast task 7 and default CKP/CMP 3 retain higher urgency. FIFO/context paths inspected; no confirmed new race found. Actual preemption timing is not emulated. |
| `6101145` | Pass timing data directly to children and retain stable knock callback pointer. Old public ABI is compiled separately in the reference harness. Finite scenarios match; default CAN route change noted above. |
| `ebd2518` | Direct interpolation and fast math helpers. Finite lookups match; the new direct API's NaN guard is ineffective. |
| `4513d2d` | Group slew scale hoisted. Slew/triggers/acceptance and resulting events match. |
| `e527bb2` | Position calculation in destination. Request histories, mode transitions and complete output structs match. |
| `8a12b1f` | Interpolation attributes/location. Finite behavior matches; NaN protection regression identified. |
| `0ef0826` | Single batch snapshot. Both cylinder modes/history match; critical snapshot count checked. |
| `d3a68bd` | Inline clock/critical helpers, attributes and Flash relocation. Helper semantics, ARM build and final placement checked. |
| `7898df5` | Read `now` at the point where current-time interpolation needs it. No uninitialized use remains; false/unused branches do not sample the clock. Hardware time read occurs later than before, which is consistent with a current-position request. |
| `06fdef7` | Header math helpers, full USPD filter constant division and ignition pass fusion. Decoder state matches; new/rounded diagnostic means accounted for independently. |
| `6e5f408` | Fixed mask, new initialization signatures and inline queue deadline. TIM2/TIM5 ARR and all call sites use the same 0x03FFFFFF masks; wrap and deadline checks pass. |
| `c1b0423` | Typed copy. Natural runtime structure alignment retained; complete position/state comparison passes. |
| `f02a2a8` | Injection pass fusion. Pressure, mass/time/phase diagnostics and all output events match, including extended pressure scenarios. |

Each patch was read, and cumulative behavior was tested at the reviewed head
against two immutable reference points. This is not a claim that every historical
intermediate commit was separately built and run through the whole suite.

## Limits and primary documentation

No new on-target performance measurement was performed. The user's measured
25-28% improvement and stable OBD2 are hardware results supplied by the user,
not results of these host tests. Host mocks do not emulate DMA, cache/bus stalls,
IRQ arrival/preemption, real output timing, external-Flash I/O or OBD2 traffic.
Static review and both ARM links complement, but do not replace, those tests.

GCC [optimization documentation](https://gcc.gnu.org/onlinedocs/gcc/Optimize-Options.html),
`-ffinite-math-only`, explicitly permits assuming no NaN/infinity. This explains
why a floating `value != value` guard cannot be relied on under `Ofast`.
Cortex-M7 priority and load/store behavior were checked against ST
[PM0253](https://www.st.com/resource/en/programming_manual/pm0253-stm32f7-series-and-stm32h7-series-cortexm7-processor-programming-manual-stmicroelectronics.pdf)
and the project's HAL/CMSIS sources and priority grouping.
