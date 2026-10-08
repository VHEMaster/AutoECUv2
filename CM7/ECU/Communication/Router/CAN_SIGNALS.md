# Periodic CAN signals

The periodic polling, encoding and retry logic belongs exclusively to `Communication/Signals`.
The Router only accepts an already encoded `can_message_t` and selects the CAN transport using its
configured downstream message-ID routing list. The CAN driver performs the actual transmission.

`config_signals.c` contains the current firmware default configuration and initializes the
Signals component after communication devices in `middlelayer_comm_init()`.
`middlelayer_comm_loop_comm()` calls the Signals poller after `ecu_comm_loop_comm()`.
No additional task or RTOS timer is required.

Messages are configured independently (enabled, CAN ID, period in microseconds, number of
signals and source parameters). A signal specifies a parameter-keeper ID, multiplier, offset and
payload byte offset. Values are encoded as saturated, rounded unsigned 16-bit little-endian
integers. The layout reserves byte 6 for per-signal validity flags and byte 7 as zero.
Invalid source values are encoded as zero with their validity bit clear.

## Default CAN1 message

Standard ID 0x600, period 100 ms, 8 bytes:

| Bytes | Parameter | Encoding |
|---|---|---|
| 0-1 | CKP1 engine RPM | uint16 little-endian, 1 rpm/bit |
| 2-3 | MAP1 manifold pressure | uint16 little-endian, bar x 1000 (0.1 kPa/bit) |
| 4-5 | TPS1 throttle position | uint16 little-endian, percent x 100 (0.01%/bit) |
| 6 | Validity mask | bit 0 RPM, bit 1 MAP, bit 2 TPS |
| 7 | Reserved | 0 |

The default parameter IDs use the global sensor instances CKP1/MAP1/TPS1 rather than
bank-blended calculated inputs. These correspond to the currently configured physical sensors
for bank 1. Remapping requires editing the firmware configuration.

On `E_AGAIN` (CAN TX queue full), Signals retains the encoded frame and retries on later
poll iterations. The 100 ms period is a best-effort software schedule and is not a hard
real-time bus delivery guarantee. No new frames are generated while the previous one is pending.
The table currently supports up to 4 messages, with up to 4 unsigned 16-bit signals each
(the maximum non-overlapping fields in bytes 0..5 is actually 3). A source field cannot
overlap another source field or reserved bytes 6-7.

Signal definitions are static firmware configuration, not versioned flash settings.
Build and hardware integration still require validation in STM32CubeIDE and with a CAN analyzer.
