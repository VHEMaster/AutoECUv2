# AutoECUv2

**Open-source engine control firmware for a custom STM32H745-based ECU.**

AutoECUv2 brings crank-angle scheduling, engine calculations, sensor acquisition, actuator control, and Classic CAN / CAN FD communication into a modular C firmware project. It is an independent automotive engineering project built around dedicated ECU hardware and the STM32H7 platform.

The project emphasizes explicit hardware integration, configurable engine behavior, and predictable execution of time-critical work. Its main application currently runs on the Cortex-M7; the repository also contains a Cortex-M4 project for further development of the dual-core platform.

**Status: active development.** Core engine-control components are implemented, while some communication services and control modules remain partial or experimental. The feature descriptions below distinguish existing functionality from development directions; they do not imply completed vehicle validation or standards certification.

## Highlights

- Crankshaft and camshaft synchronization with angle-based ignition, injection, and ADC acquisition scheduling.
- Layered integration of automotive interface ICs, physical sensors, engine calculations, and control modules.
- Configurable engine calculations with lookup tables, source selection, fallback values, and per-bank/per-cylinder corrections.
- Electronic throttle, boost-control valve, idle-control, fuel-pump, and cooling-fan logic.
- Classic CAN and CAN FD frames up to 64 bytes, configurable signal packing/decoding, and routing in both directions.
- ISO-TP TX/RX over Classic CAN and CAN FD, plus a subset of OBD-II and UDS diagnostics.
- Versioned configuration structures, redundant external QSPI flash storage with CRC checks, and default-configuration recovery.
- Absolute-timestamp output pulse queues, chronological insertion, and configurable late-pulse policies.
- Foreground, periodic, and event-driven execution, with hardware timers, DMA acquisition, and MDMA-backed memory/CRC transfers.

## Capabilities and maturity

| Area | Current implementation |
| --- | --- |
| Engine synchronization | Regular 60-2 and 36-2 crank patterns; single-pulse camshaft decoding. |
| Ignition | Angle-based scheduling with sequential, semi-sequential, and wasted-spark modes. |
| Injection | Phased and semi-sequential scheduling, phase-aware handover to sequential operation, configurable groups, injector lag compensation, and fuel-pressure/per-cylinder corrections. |
| Output pulse scheduling | Absolute timestamps, chronological queue insertion, per-output reset, and keep-end / keep-width late policies. |
| Engine calculations | Configurable MAP/TPS/MAF-based charge calculations, temperature compensation, AFR targets, ignition and injection calculations, and idle-control logic. |
| Sensors | CKP, CMP, pedal and throttle position, manifold pressure, airflow, intake/coolant/exhaust temperature, vehicle speed, and oil pressure/temperature components. |
| Actuator modules | Electronic throttle, boost-control valve, fuel pump, cooling fan, ignition power, and indication. Integration and validation are ongoing. |
| Cylinder balance | Per-cylinder metrics derived from crankshaft timing around combustion events. |
| Knock acquisition | Crank-angle-triggered ADC sampling windows and DMA buffering. DSP-based knock detection and automatic knock correction are development work. |
| CAN / CAN FD | Configurable arbitration/data bitrates and bit-rate switching; Classic CAN frames up to 8 bytes and CAN FD frames up to 64 bytes. |
| CAN signals | Configurable scaling and packing/decoding, Intel/Motorola layouts, signed/unsigned values, counters, and CRC support. Router integration includes incoming and outgoing signals with frame-format selection. |
| Diagnostics | ISO-TP TX/RX for Classic CAN and CAN FD, extended Single Frame and First Frame length encoding, and selected OBD-II live-data/vehicle-information and UDS read-data services. The current ISO-TP payload buffer is limited to 4095 bytes. |
| Configuration storage | Versioned sections, two flash copies, CRC validation, asynchronous load/save/default operations, and fallback to defaults when stored data cannot be used. |
| VVT / KWP | Configuration and API groundwork; complete operational behavior is still pending. |
| Dual-core execution | CM7 hosts the ECU application. CM4 project scaffolding is present; distributed ECU execution is not yet established. |

## Hardware platform

The configured target is **STM32H745IIT6**, a dual-core Cortex-M7/Cortex-M4 microcontroller. This repository targets a custom ECU board, so peripheral assignments and electrical interfaces are part of the project configuration.

Device integrations include:

| Device | Role |
| --- | --- |
| ST L9966 | Configurable analog/digital input interface. |
| ST L9960 | H-bridge motor control. |
| Infineon TLE6240GP | Low-side output driver. |
| Infineon TLE4729G | Stepper-motor driver. |
| Bosch CJ125 | Wideband lambda sensor interface, with associated heater-control logic. |
| MAX31855 | Thermocouple interface. |
| External QSPI flash | Configuration and calibration storage. |

Start with [AutoECUv2.ioc](AutoECUv2.ioc), [the board pin definitions](CM7/Core/Inc/main.h), and [hardware configuration](CM7/ECU/Config/Hardware) when examining or adapting the hardware integration. A port to another board also needs corresponding device configuration, timer/ADC assignments, linker layout, and startup settings.

## Architecture

The ECU application is organized into layers with distinct responsibilities:

| Layer | Responsibility | Source |
| --- | --- | --- |
| Drivers | GPIO, SPI, CAN, flash, CRC, timekeeping, input capture, queued output pulses, and ADC acquisition. | [Drivers](CM7/ECU/Drivers) |
| Devices | Device-specific protocols, register configuration, and state machines for external ICs. | [Devices](CM7/ECU/Devices) |
| Sensors | Physical measurements, conversion, validity handling, and CKP/CMP decoding. | [Sensors](CM7/ECU/Sensors) |
| Core / CalcData | Engine state, selected data sources, lookup tables, and calculated control targets. | [Core](CM7/ECU/Core) |
| Timings | Crank-angle event scheduling for ignition, injection, cylinder balance, and knock-sampling windows. | [Timings](CM7/ECU/Timings) |
| Modules | Control behavior for throttle, boost, idle-related actuators, and auxiliary outputs. | [Modules](CM7/ECU/Modules) |
| Communication | CAN signals, frame routing, diagnostic transport, and diagnostic services. | [Communication](CM7/ECU/Communication) |
| MiddleLayer | Initialization, component wiring, callbacks, and execution-loop integration. | [MiddleLayer](CM7/ECU/MiddleLayer) |
| Config | Hardware mappings, device settings, sensor definitions, engine parameters, and calibration data. | [Config](CM7/ECU/Config) |

Sensor and device data feed the engine calculations. Calculated targets are consumed by actuator modules and timing components, while CKP/CMP events provide the synchronization needed for angle-dependent work. The communication layer exposes selected internal parameters and handles diagnostic requests.

Execution combines a foreground loop, periodic fast/slow/communication processing, and peripheral callbacks. Timer-backed scheduling handles output events using absolute deadlines. DMA supports acquisition, while MDMA is used for memory copies and transfers into the hardware CRC unit. CRC processing uses aligned word transfers with byte handling for unaligned prefixes and trailing data. The firmware also includes ITCM placement for selected routines, cache-management helpers, and execution-time measurement utilities.

The central integration point is [MiddleLayer](CM7/ECU/MiddleLayer). For a new contributor, following its initialization and loop calls is a useful way to understand how the layers interact.

### Output scheduling

[QueuedPulses](CM7/ECU/Drivers/QueuedPulses) stores absolute output-event timestamps and inserts events in chronological order. Ignition and injection timing use this layer to schedule pulses, with a per-output reset path for invalid timing inputs.

Late-pulse behavior is selected by policy: **keep end** preserves the requested end time and can shorten the pulse; **keep width** preserves its duration when the start must move. Pulse insertion handles available gaps and existing output activity, including requests that arrive out of timestamp order. Scheduling also checks minimum pulse duration and the representable timer horizon.

Injection timing includes a phase-aware handover from semi-sequential to sequential operation when camshaft synchronization becomes available. Switching modes is coordinated per cylinder rather than applied blindly to every pending event.

## Communication

The communication stack separates the CAN driver, signal encoding, routing, transport, and diagnostic services:

| Component | Current behavior |
| --- | --- |
| [CAN driver](CM7/ECU/Drivers/Can) | Classic CAN / CAN FD frame formats, standard/extended identifiers, DLC conversion and validation, configurable nominal/data bitrates, and bit-rate switching. |
| [Signals](CM7/ECU/Communication/Signals) | Configurable incoming/outgoing signal definitions, scaling, bit layout, counters, CRC, and frame payloads up to 64 bytes for CAN FD. |
| [Router](CM7/ECU/Communication/Router) | Incoming signal callbacks, outgoing signal dispatch, diagnostic message routing, and matching by configured frame format. |
| [ISO-TP](CM7/ECU/Communication/ISOTP) | Single Frame and segmented TX/RX, flow control, block size, separation time, sequence checking, timeouts, and error counters. |
| [OBD-II](CM7/ECU/Communication/OBD2) / [UDS](CM7/ECU/Communication/UDS) | Selected diagnostic services above the transport layer; service coverage remains partial. |

Classic CAN uses payload lengths up to 8 bytes. CAN FD additionally supports 12, 16, 20, 24, 32, 48, and 64 bytes; the driver provides helpers to validate or select a legal frame length. CAN FD support must be enabled in the CAN configuration, and signal, route, and ISO-TP frame formats must match the intended connection.

ISO-TP implements the CAN FD extended Single Frame length field and the extended First Frame length field on both TX and RX paths. **Extended First Frame encoding does not currently mean arbitrary-size transfers:** `ISOTP_PAYLOAD_LEN_MAX` is still **4095 bytes**. Oversized incoming transfers follow the receive-overflow path.

The current default CAN configuration enables CAN FD support with a **500 kbit/s nominal bitrate** and **1 Mbit/s data bitrate**. ISO-TP defaults still select **Classic CAN**. These are separate settings; enabling CAN FD at the controller does not switch every diagnostic connection to CAN FD. See [CAN configuration](CM7/ECU/Config/Communication/Src/config_can.c), [ISO-TP configuration](CM7/ECU/Config/Communication/Src/config_isotp.c), and [routing configuration](CM7/ECU/Config/Communication/Src/config_router.c).

KWP integration remains groundwork. RS-422 transport and distributed communication execution on CM4 are development directions.

## Repository layout

| Path | Contents |
| --- | --- |
| [CM7/ECU](CM7/ECU) | Main ECU application and its configuration. |
| [CM7/Core](CM7/Core) | CM7 startup, interrupt handlers, board initialization, and HAL integration. |
| [CM4](CM4) | Cortex-M4 project and startup scaffolding. |
| [Common](Common) | Shared project support code. |
| [Drivers](Drivers) | Bundled STM32 HAL and CMSIS dependencies. |
| [Middlewares](Middlewares) | Bundled middleware dependencies. |
| [Docs](Docs) | Supporting project documentation. |
| [AutoECUv2.ioc](AutoECUv2.ioc) | STM32CubeMX peripheral and project configuration. |
| [LICENSE](LICENSE) | Project license. |

## Getting started

### Prerequisites

- Git.
- [STM32CubeIDE](https://www.st.com/en/development-tools/stm32cubeide.html), including its ARM GCC toolchain.
- A compatible board and debug probe for on-target work.

The committed CubeMX configuration records **STM32CubeMX 6.8.1** and **STM32Cube FW_H7 V1.11.1**. These identify the configuration baseline; they are not a claim that every newer toolchain combination has been tested. HAL, CMSIS, and middleware sources are included in the repository.

### Clone and import

```sh
git clone https://github.com/VHEMaster/AutoECUv2.git
cd AutoECUv2
```

1. Open STM32CubeIDE and choose **File → Import → General → Existing Projects into Workspace**.
2. Select the repository root and import the supplied projects, including `AutoECUv2_CM7` and `AutoECUv2_CM4`.
3. Keep the repository directory structure intact. Project resources refer to sibling `Common` and `Drivers` directories.
4. Select `AutoECUv2_CM7`, activate the `Build_Debug` configuration, and build the project.

The CM7 project includes `Build_Debug` and `Build_Release` configurations and produces an ELF image. Start with the debug configuration when bringing up hardware or investigating behavior.

Build settings are maintained in the committed STM32CubeIDE project files. The repository does not currently provide a standalone CMake or Make-based build entry point.

### Debug on hardware

Use the STM32CubeIDE debugger with a probe supported by your board. Check the target, reset behavior, flash addresses, and core selection against the board setup and committed linker/startup files. The ECU application currently resides on CM7; the presence of a CM4 project does not mean that ECU workloads are automatically shared between cores.

Opening the `.ioc` file is useful for inspecting peripheral configuration. Treat code regeneration as a separate change and review its diff before accepting changes to initialization or project settings.

## Configuration and calibration

Configuration is split by responsibility rather than kept in a single monolithic file:

| Configuration area | What to examine |
| --- | --- |
| [Hardware](CM7/ECU/Config/Hardware) and [Devices](CM7/ECU/Config/Devices) | Physical channels, external devices, and board-specific mappings. |
| [Sensors](CM7/ECU/Config/Sensors) | Sensor instances, conversions, and measurement configuration. |
| [Timings](CM7/ECU/Config/Timings) | Engine synchronization and ignition/injection timing configuration. |
| [Modules](CM7/ECU/Config/Modules) | Actuator and control-module settings. |
| [Calibration](CM7/ECU/Config/Calibration) | Engine calibration values and lookup tables. |
| [Communication](CM7/ECU/Config/Communication) | Signal definitions and communication configuration. |
| [Global](CM7/ECU/Config/Global), [Versioned](CM7/ECU/Config/Versioned), and [Flash](CM7/ECU/Config/Flash) | Configuration structures, stored-data layout, and persistence integration. |

The persistence layer stores configuration sections in two flash copies and checks section metadata and payload CRCs when loading. Its state machines cover loading, saving, restoring defaults, asynchronous transfers, and error/timeout handling. A failed load can fall back to the section's default configuration.

Configuration staging and CRC work use MDMA. CRC transfers handle unaligned addresses and lengths that are not multiples of a word, and the QSPI transfer path supports requests larger than 64 KiB. Review buffer placement, cache handling, and transfer sizes when changing configuration storage.

Defaults are a development starting point and need to match the actual engine, sensors, injectors, actuators, and board. When changing a persistent structure, review its version, size, storage layout, and initialization path together.

## Contributing

Bug reports, hardware-porting notes, focused fixes, and improvements to testing or documentation are welcome through [GitHub issues](https://github.com/VHEMaster/AutoECUv2/issues) and pull requests.

A useful bug report includes:

- The commit and build configuration used.
- Board revision, relevant wiring, and engine/device configuration.
- Steps to reproduce, expected behavior, and observed behavior.
- Logs, traces, register values, or scope captures when available.

For code changes, keep the responsibility in the appropriate layer and explain timing, interrupt, DMA, or persistent-data implications where relevant. State how the change was checked: compilation, host-side reproduction, bench measurement, or engine testing. These checks provide different evidence and should be identified separately.

Changes to timing and control paths benefit especially from measurements of event ordering, latency, synchronization loss/recovery, and output behavior. Changes to flash formats or communication packing should include concrete input/output examples and boundary cases.

## Development directions

Areas for further work include:

- Stronger regression coverage for synchronization, output scheduling, configuration validation, and recovery paths.
- DSP processing of knock-window samples, including filtering and frequency-domain analysis.
- Completion of VVT control and additional diagnostic services.
- Broader interoperability and regression testing of Classic CAN / CAN FD, signal routing, and ISO-TP boundary cases.
- Larger ISO-TP payload buffers and API/storage changes if transfers beyond the current 4095-byte limit are needed.
- Development of KWP support beyond the current API groundwork.
- Measured optimization of periodic routines and clearer execution-time budgets.
- Exploration of CM4 workloads for communication and signal processing.

These are development directions, not a release schedule. Current implementation details and open work are reflected in the source and issue tracker.

## License

AutoECUv2 is distributed under the **GNU General Public License, version 2**. See [LICENSE](LICENSE) for the full text.

Bundled third-party components retain their respective license terms; consult their license files and source headers.

Maintained by [VHEMaster](https://github.com/VHEMaster).

