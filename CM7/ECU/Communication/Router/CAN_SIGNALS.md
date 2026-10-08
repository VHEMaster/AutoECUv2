# Periodic CAN signals

This feature is configured using the static `router_signal_tx_message_config_t` table in
`CM7/ECU/Config/Communication/Src/config_router.c`. The router instance receives the table through
`router_init_ctx_t.signals_tx` and runs it from the existing communication loop.
No additional RTOS task, dynamic allocation, or flash configuration layout modification is needed.

Each message independently specifies its CAN identifier, period (microseconds), enable flag,
and up to four signals. Each signal specifies an existing parameter ID
(`ecu_config_parameter_id_t`), linear scaling (`value * multiplier + offset`),
and the starting payload byte offset for a little-endian unsigned 16-bit integer.
Values are rounded to nearest nonnegative integer and saturated to 0..65535.
Invalid/unavailable/nonfinite parameters become zero and have their validity bit cleared.
Avoid overlapping signal fields or using bytes 6 and 7 for signals: byte 6 is the validity mask.

The router matches outgoing message IDs against
`router_config_t.can.signals.downstream_list` and selects the configured CAN instance.
For the initial configuration the list maps standard CAN ID 0x600 to CAN1.

## Initial message (CAN ID 0x600, 8 bytes, every 100 ms)

| Byte | Signal | Encoding |
|---|---|---|
| 0..1 | CKP1 RPM | uint16 little endian, 1 rpm/bit |
| 2..3 | MAP1 | uint16 little endian, value in bar * 1000 (0.1 kPa/bit) |
| 4..5 | TPS1 | uint16 little endian, percentage * 100 (0.01%/bit) |
| 6 | Validity | bit0 RPM, bit1 MAP, bit2 TPS; 1 = valid |
| 7 | Reserved | zero |

The default IDs reference the configured CKP1, MAP1, TPS1 sensor instances in the global
parameter keeper, not the calculated bank-specific blended source. The current default
sensor-to-bank mapping uses MAP1 and TPS1 globally; for alternative bank mappings, update
the parameter IDs in the table accordingly.

Timing uses the firmware microsecond timebase (`time_now_us` / `time_diff`) with
first transmission after one complete period. If the FDCAN TX FIFO is full
(`E_AGAIN`), a fully encoded message is retained and retried in subsequent
communication-loop iterations. Hardware acceptance and bus delivery are not guaranteed
by successful queueing.

## Limitations

- Static firmware configuration only; the CAN signal definitions are not yet stored in versioned flash.
- Signal mapping uses fixed offsets and unsigned 16-bit little-endian encoding.
- No automatic detection or rejection of overlapping fields; configure them carefully.
- No hardware-in-loop or STM32CubeIDE build has been executed for this change.
