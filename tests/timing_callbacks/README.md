# Performance regression audit: f02a2a8

These tests belong to the October 17 review branch, based on unmodified firmware
at `f02a2a8`. Python 3, native GCC and git history back to `5ea8e8c` are required.
No ARM peripheral is accessed. Temporary compilation products are removed.

## Main differential checks

```sh
python3 tests/timing_callbacks/run.py --baseline 5ea8e8c
python3 tests/timing_callbacks/run.py --baseline 5ea8e8c --optimization Ofast
python3 tests/timing_callbacks/run.py --baseline 0ef0826
python3 tests/timing_callbacks/run.py --baseline 0ef0826 --optimization Ofast
python3 tests/timing_callbacks/test_ckp_decoders.py
python3 tests/timing_callbacks/test_time_helpers.py
python3 tests/timing_callbacks/test_critical_sections.py
python3 tests/timing_callbacks/test_queuedpulse_deadlines.py
```

The first pair compares the whole 13-commit range. The second isolates the six
newest commits. Original public headers and a direct-lookup adapter preserve the
pre-snapshot reference ABI. Real project types/default tables are used; ARM
critical/cache operations, time and peripheral calls are mocked.

Each main run checks 128,000 scalar lookups, 200,000 clamp inputs plus boundary
neighbors, 20,000 position/history steps, and 112 x 2,400 CKP updates with 4/8
cylinders, one/two banks, phase transitions, group modes, slew, acceptance,
cutoff, MAP variation/failure and knock-window completion. All 921,244 peripheral
events and original runtime fields except the two ignition mean diagnostics
must match exactly.

`06fdef7` newly publishes `advance_mean` and changes rounding of
`advance_requested_mean`. Both are checked against independent double-precision
means of the strictly compared cylinder arrays, using an explicit bound of
`8 * FLT_EPSILON * max(1, abs(expected))`. These are the only numerical allowances
in the timing runtime comparison. Neither mean is read by the current control
code. Observed maximum old/new requested-mean difference: 0.00000190734863 deg.

The focused checks compare decoder private/public state, the actual configured
26-bit clock wrap and interpolation, nested PRIMASK behavior and queued-pulse
arithmetic. The clock tests do not pretend that the new API still accepts an
arbitrary timer mask. See the report for counts and limitations.

## Known failing NaN contract

```sh
python3 tests/timing_callbacks/test_math_contracts.py
```

On reviewed firmware this deliberately exits **1**: `OPTIMIZE_FAST` eliminates
the NaN checks. It also tests 100,000 air-density and 100,000 thermistor inputs in
both profiles. Their numerical assertions pass; the nonzero exit identifies
the NaN-contract failure rather than a physical-range numeric failure.

To verify the proposed guard in a **temporary source copy only**:

```sh
python3 tests/timing_callbacks/test_math_contracts.py --verify-nan-fix
```

This substitutes a representation-based NaN check in the temporary interpolation
translation unit and must exit 0. It does not edit firmware. It is a focused
fix probe, not full target validation of a merged fix.

## ARM image

```sh
python3 tests/timing_callbacks/check_itcm_elf.py path/to/firmware.elf --readelf arm-none-eabi-readelf
```

Both complete controlled ARM GCC 13.2.1 builds linked within the real 64 KiB ITCM
limit: 59,552 bytes at global `-Og`, 54,544 at global `-Os`. Existing local
per-function optimization attributes remain active. See
`Docs/Performance/Regression_2026-10-17.md` for exact scope and findings.

These are host behavioral tests and compile/link checks. IRQ preemption,
DMA/cache/bus timing, actual pulse hardware and OBD2 traffic are not emulated.
