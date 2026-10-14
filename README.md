# AutoECUv2

**Open-source engine control firmware for a custom STM32H745-based ECU.**

AutoECUv2 brings crank-angle scheduling, engine calculations, sensor acquisition, actuator control, and CAN communication into a modular C firmware project. It is an independent automotive engineering project built around dedicated ECU hardware and the STM32H7 platform.

The project emphasizes explicit hardware integration, configurable engine behavior, and predictable execution of time-critical work. Its main application currently runs on the Cortex-M7; the repository also contains a Cortex-M4 project for further development of the dual-core platform.

**Status: active development.** Core engine-control components are implemented, while some communication services and control modules remain partial or experimental. The feature descriptions below distinguish existing functionality from development directions; they do not imply completed vehicle validation or standards certification.

## Highlights

- Crankshaft and camshaft synchronization with angle-based ignition, injection, and ADC acquisition scheduling.
- Layered integration of automotive interface ICs, physical sensors, engine calculations, and control modules.
- Configurable engine calculations with lookup tables, source selection, fallback values, and per-bank/per-cylinder corrections.
- Electronic throttle, boost-control valve, idle-control, fuel-pump, and cooling-fan logic.
- CAN signal packing and decoding, routing, ISO-TP transport, and a subset of OBD-II and UDS diagnostics.
- Versioned configuration structures and external QSPI flash storage with CRC metadata.
- Foreground, periodic, and event-driven execution, with hardware timers and DMA used where timing or throughput matters.

## Capabilities and maturity

| Area | Current implementation |
| --- | --- |
| Engine synchronization | Regular 60-2 and 36-2 crank patterns; single-pulse camshaft decoding. |
| Ignition | Angle-based scheduling with sequential, semi-sequential, and wasted-spark modes. |
| Injection | Phased and semi-sequential scheduling, configurable groups, injector lag compensation, and fuel-pressure/per-cylinder corrections. |
| Engine calculations | Configurable MAP/TPS/MAF-based charge calculations, temperature compensation, AFR targets, ignition and injection calculations, and idle-control logic. |
| Sensors | CKP, CMP, pedal and throttle position, manifold pressure, airflow, intake/coolant/exhaust temperature, vehicle speed, and oil pressure/temperature components. |
| Actuator modules | Electronic throttle, boost-control valve, fuel pump, cooling fan, ignition power, and indication. Integration and validation are ongoing. |
| Cylinder balance | Per-cylinder metrics derived from crankshaft timing around combustion events. |
| Knock acquisition | Crank-angle-triggered ADC sampling windows and DMA buffering. DSP-based knock detection and automatic knock correction are development work. |
| CAN signals | Configurable scaling and packing/decoding, Intel/Motorola layouts, signed/unsigned values, counters, and CRC support. Outgoing signals are routed; incoming signal routing integration is incomplete. |
| Diagnostics | ISO-TP transport and selected OBD-II live-data/vehicle-information and UDS read-data services. Broader service coverage is incomplete. |
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

Execution combines a foreground loop, periodic fast/slow/communication processing, and peripheral callbacks. Timer-backed scheduling handles output events; DMA supports acquisition and data transfer. The firmware also includes ITCM placement for selected routines, cache-management helpers, and execution-time measurement utilities.

The central integration point is [MiddleLayer](CM7/ECU/MiddleLayer). For a new contributor, following its initialization and loop calls is a useful way to understand how the layers interact.

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
- Completion of VVT control, incoming signal routing, and additional diagnostic services.
- Development of KWP support beyond the current API groundwork.
- Measured optimization of periodic routines and clearer execution-time budgets.
- Exploration of CM4 workloads for communication and signal processing.

These are development directions, not a release schedule. Current implementation details and open work are reflected in the source and issue tracker.

## License

AutoECUv2 is distributed under the **GNU General Public License, version 2**. See [LICENSE](LICENSE) for the full text.

Bundled third-party components retain their respective license terms; consult their license files and source headers.

Maintained by [VHEMaster](https://github.com/VHEMaster).
