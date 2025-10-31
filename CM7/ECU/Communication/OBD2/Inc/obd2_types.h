/*
 * obd2_types.h
 *
 *  Created on: Oct 19, 2025
 *      Author: VHEMaster
 */

#ifndef COMMUNICATION_OBD2_INC_OBD2_TYPES_H_
#define COMMUNICATION_OBD2_INC_OBD2_TYPES_H_

#include "common.h"
#include "time.h"
#include "versioned_obd2.h"

#define OBD2_DATA_LENGTH_MAX           255
#define OBD2_ASCII_LENGTH_MAX          64
#define OBD2_RESPONSE_NEGATIVE_CODE    0x7F
#define OBD2_RESPONSE_POSITIVE_OFFSET  0x40
#define OBD2_DOWNSTREAM_TIMEOUT        (1000 * TIME_US_IN_MS)

typedef enum {
  OBD2_OK = 0,
  OBD2_MAX
}obd2_error_code_t;

typedef enum {
  OBD2_SID_CURRENT_DATA                         = 0x01,
  OBD2_SID_FREEZE_FRAME_DATA                    = 0x02,
  OBD2_SID_DIAGNOSTIC_TROUBLE_CODES             = 0x03,
  OBD2_SID_CLEAR_DIAGNOSTIC_INFORMATION         = 0x04,
  OBD2_SID_OXYGEN_SENSOR_MONITORING             = 0x05,
  OBD2_SID_ON_BOARD_MONITORING_TESTS            = 0x06,
  OBD2_SID_PENDING_DIAGNOSTIC_TROUBLE_CODES     = 0x07,
  OBD2_SID_VEHICLE_INFORMATION                  = 0x09,
  OBD2_SID_PERMANENT_DIAGNOSTIC_TROUBLE_CODES   = 0x0A,
}obd2_sid_t;

typedef enum
{
  /* 0x00–0x1F */
  OBD2_PID_01_SUPPORTED_01_20                  = 0x00,  // PIDs supported [01–20] (bitmask)
  OBD2_PID_01_MONITOR_STATUS_SINCE_DTC_CLEAR   = 0x01,  // Monitor status since DTCs cleared (bitfield)
  OBD2_PID_01_FREEZE_DTC                       = 0x02,  // Freeze DTC (stored DTC for freeze frame)
  OBD2_PID_01_FUEL_SYSTEM_STATUS               = 0x03,  // Fuel system status (enum/bitfield)
  OBD2_PID_01_CALCULATED_ENGINE_LOAD           = 0x04,  // Calculated engine load (%)
  OBD2_PID_01_COOLANT_TEMPERATURE              = 0x05,  // Engine coolant temperature (°C)
  OBD2_PID_01_ST_FUEL_TRIM_B1                  = 0x06,  // Short-term fuel trim — Bank 1 (%)
  OBD2_PID_01_LT_FUEL_TRIM_B1                  = 0x07,  // Long-term  fuel trim — Bank 1 (%)
  OBD2_PID_01_ST_FUEL_TRIM_B2                  = 0x08,  // Short-term fuel trim — Bank 2 (%)
  OBD2_PID_01_LT_FUEL_TRIM_B2                  = 0x09,  // Long-term  fuel trim — Bank 2 (%)
  OBD2_PID_01_FUEL_PRESSURE                    = 0x0A,  // Fuel pressure (kPa gauge)
  OBD2_PID_01_INTAKE_MANIFOLD_PRESSURE         = 0x0B,  // Intake manifold absolute pressure (kPa)
  OBD2_PID_01_ENGINE_RPM                       = 0x0C,  // Engine speed (rpm)
  OBD2_PID_01_VEHICLE_SPEED                    = 0x0D,  // Vehicle speed (km/h)
  OBD2_PID_01_TIMING_ADVANCE                   = 0x0E,  // Ignition timing advance (°BTDC)
  OBD2_PID_01_INTAKE_AIR_TEMPERATURE           = 0x0F,  // Intake air temperature (°C)
  OBD2_PID_01_MAF_AIR_FLOW_RATE                = 0x10,  // Mass air flow rate (g/s)
  OBD2_PID_01_THROTTLE_POSITION                = 0x11,  // Throttle position (%)
  OBD2_PID_01_COMMANDED_SECONDARY_AIR          = 0x12,  // Commanded secondary air (enum/bitfield)
  OBD2_PID_01_O2_SENSORS_PRESENT_2BANKS        = 0x13,  // O2 sensors present (up to 2 banks), bitmask
  OBD2_PID_01_O2_SENSOR1_V_AND_STFT            = 0x14,  // O2 B1S1: voltage + STFT
  OBD2_PID_01_O2_SENSOR2_V_AND_STFT            = 0x15,  // O2 B1S2: voltage + STFT
  OBD2_PID_01_O2_SENSOR3_V_AND_STFT            = 0x16,  // O2 B1S3: voltage + STFT
  OBD2_PID_01_O2_SENSOR4_V_AND_STFT            = 0x17,  // O2 B1S4: voltage + STFT
  OBD2_PID_01_O2_SENSOR5_V_AND_STFT            = 0x18,  // O2 B2S1: voltage + STFT
  OBD2_PID_01_O2_SENSOR6_V_AND_STFT            = 0x19,  // O2 B2S2: voltage + STFT
  OBD2_PID_01_O2_SENSOR7_V_AND_STFT            = 0x1A,  // O2 B2S3: voltage + STFT
  OBD2_PID_01_O2_SENSOR8_V_AND_STFT            = 0x1B,  // O2 B2S4: voltage + STFT
  OBD2_PID_01_OBD2_STANDARDS_IN_USE            = 0x1C,  // OBD standards this vehicle conforms to (enum)
  OBD2_PID_01_O2_SENSORS_PRESENT_4BANKS        = 0x1D,  // O2 sensors present (up to 4 banks), 2-byte bitmask
  OBD2_PID_01_AUX_INPUT_STATUS                 = 0x1E,  // Auxiliary input status (bitfield)
  OBD2_PID_01_RUNTIME_SINCE_ENGINE_START       = 0x1F,  // Run time since engine start (s)

  /* 0x20–0x3F */
  OBD2_PID_01_SUPPORTED_21_40                  = 0x20,  // PIDs supported [21–40] (bitmask)
  OBD2_PID_01_DISTANCE_WITH_MIL_ON             = 0x21,  // Distance traveled with MIL on (km)
  OBD2_PID_01_FUEL_RAIL_PRESS_REL_TO_MANIF     = 0x22,  // Fuel rail pressure relative to MAP (kPa)
  OBD2_PID_01_FUEL_RAIL_PRESSURE_DIRECT        = 0x23,  // Fuel rail pressure (direct) for GDI (kPa)
  OBD2_PID_01_O2_S1_EQUIV_RATIO_AND_VOLT       = 0x24,  // Wideband O2 B1S1: equivalence ratio + voltage
  OBD2_PID_01_O2_S2_EQUIV_RATIO_AND_VOLT       = 0x25,  // Wideband O2 B1S2: equivalence ratio + voltage
  OBD2_PID_01_O2_S3_EQUIV_RATIO_AND_VOLT       = 0x26,  // Wideband O2 B1S3: equivalence ratio + voltage
  OBD2_PID_01_O2_S4_EQUIV_RATIO_AND_VOLT       = 0x27,  // Wideband O2 B1S4: equivalence ratio + voltage
  OBD2_PID_01_O2_S5_EQUIV_RATIO_AND_VOLT       = 0x28,  // Wideband O2 B2S1: equivalence ratio + voltage
  OBD2_PID_01_O2_S6_EQUIV_RATIO_AND_VOLT       = 0x29,  // Wideband O2 B2S2: equivalence ratio + voltage
  OBD2_PID_01_O2_S7_EQUIV_RATIO_AND_VOLT       = 0x2A,  // Wideband O2 B2S3: equivalence ratio + voltage
  OBD2_PID_01_O2_S8_EQUIV_RATIO_AND_VOLT       = 0x2B,  // Wideband O2 B2S4: equivalence ratio + voltage
  OBD2_PID_01_COMMANDED_EGR                    = 0x2C,  // Commanded EGR (%)
  OBD2_PID_01_EGR_ERROR                        = 0x2D,  // EGR error (%)
  OBD2_PID_01_COMMANDED_EVAP_PURGE             = 0x2E,  // Commanded evaporative purge (%)
  OBD2_PID_01_FUEL_LEVEL_INPUT                 = 0x2F,  // Fuel level input (%)
  OBD2_PID_01_WARMUPS_SINCE_DTC_CLEAR          = 0x30,  // Number of warm-ups since DTC clear
  OBD2_PID_01_DISTANCE_SINCE_DTC_CLEAR         = 0x31,  // Distance since DTC clear (km)
  OBD2_PID_01_EVAP_VAPOR_PRESSURE_ABS_LEGACY   = 0x32,  // Evap system vapor pressure (legacy format, Pa)
  OBD2_PID_01_BAROMETRIC_PRESSURE              = 0x33,  // Barometric pressure (kPa)
  OBD2_PID_01_O2_S1_CURRENT_AND_EQUIV          = 0x34,  // Wideband O2 B1S1: λ and current (mA)
  OBD2_PID_01_O2_S2_CURRENT_AND_EQUIV          = 0x35,  // Wideband O2 B1S2: λ and current (mA)
  OBD2_PID_01_O2_S3_CURRENT_AND_EQUIV          = 0x36,  // Wideband O2 B1S3: λ and current (mA)
  OBD2_PID_01_O2_S4_CURRENT_AND_EQUIV          = 0x37,  // Wideband O2 B1S4: λ and current (mA)
  OBD2_PID_01_O2_S5_CURRENT_AND_EQUIV          = 0x38,  // Wideband O2 B2S1: λ and current (mA)
  OBD2_PID_01_O2_S6_CURRENT_AND_EQUIV          = 0x39,  // Wideband O2 B2S2: λ and current (mA)
  OBD2_PID_01_O2_S7_CURRENT_AND_EQUIV          = 0x3A,  // Wideband O2 B2S3: λ and current (mA)
  OBD2_PID_01_O2_S8_CURRENT_AND_EQUIV          = 0x3B,  // Wideband O2 B2S4: λ and current (mA)
  OBD2_PID_01_CAT_TEMP_B1S1                    = 0x3C,  // Catalyst temperature — Bank 1 Sensor 1 (°C)
  OBD2_PID_01_CAT_TEMP_B2S1                    = 0x3D,  // Catalyst temperature — Bank 2 Sensor 1 (°C)
  OBD2_PID_01_CAT_TEMP_B1S2                    = 0x3E,  // Catalyst temperature — Bank 1 Sensor 2 (°C)
  OBD2_PID_01_CAT_TEMP_B2S2                    = 0x3F,  // Catalyst temperature — Bank 2 Sensor 2 (°C)

  /* 0x40–0x5F */
  OBD2_PID_01_SUPPORTED_41_60                  = 0x40,  // PIDs supported [41–60] (bitmask)
  OBD2_PID_01_MONITOR_STATUS_THIS_DRIVE_CYCLE  = 0x41,  // Monitor status this drive cycle (bitfield)
  OBD2_PID_01_CONTROL_MODULE_VOLTAGE           = 0x42,  // Control module voltage (V)
  OBD2_PID_01_ABSOLUTE_LOAD_VALUE              = 0x43,  // Absolute load value (%)
  OBD2_PID_01_COMMANDED_EQUIV_RATIO            = 0x44,  // Commanded equivalence ratio (λ)
  OBD2_PID_01_RELATIVE_THROTTLE_POS            = 0x45,  // Relative throttle position (%)
  OBD2_PID_01_AMBIENT_AIR_TEMPERATURE          = 0x46,  // Ambient air temperature (°C)
  OBD2_PID_01_ABS_THROTTLE_POS_B               = 0x47,  // Absolute throttle position B (%)
  OBD2_PID_01_ABS_THROTTLE_POS_C               = 0x48,  // Absolute throttle position C (%)
  OBD2_PID_01_ACCEL_PEDAL_POS_D                = 0x49,  // Accelerator pedal position D (%)
  OBD2_PID_01_ACCEL_PEDAL_POS_E                = 0x4A,  // Accelerator pedal position E (%)
  OBD2_PID_01_ACCEL_PEDAL_POS_F                = 0x4B,  // Accelerator pedal position F (%)
  OBD2_PID_01_COMMANDED_THROTTLE_ACTUATOR      = 0x4C,  // Commanded throttle actuator (%)
  OBD2_PID_01_TIME_WITH_MIL_ON                 = 0x4D,  // Time run with MIL on (min)
  OBD2_PID_01_TIME_SINCE_DTC_CLEAR             = 0x4E,  // Time since DTCs cleared (min)
  OBD2_PID_01_MAX_MAF_SINCE_DTC_CLEAR          = 0x4F,  // Maximum value for MAF since DTC clear (g/s)
  OBD2_PID_01_FUEL_TYPE                        = 0x50,  // Fuel type (enum)
  OBD2_PID_01_ETHANOL_FUEL_PERCENT             = 0x51,  // Ethanol fuel % (%)
  OBD2_PID_01_EVAP_VAPOR_PRESSURE_ABSOLUTE     = 0x52,  // Evap system vapor pressure (absolute, kPa)
  OBD2_PID_01_EVAP_VAPOR_PRESSURE_ALT          = 0x53,  // Evap system vapor pressure (alternate, Pa signed)
  OBD2_PID_01_ST_O2_TRIM_B1_S1_S3_S5           = 0x54,  // Short-term secondary O2 trim (A: B1/B3) (%)
  OBD2_PID_01_LT_O2_TRIM_B1_S1_S3_S5           = 0x55,  // Long-term  secondary O2 trim (A: B1/B3) (%)
  OBD2_PID_01_ST_O2_TRIM_B2_S1_S3_S5           = 0x56,  // Short-term secondary O2 trim (A: B2/B4) (%)
  OBD2_PID_01_LT_O2_TRIM_B2_S1_S3_S5           = 0x57,  // Long-term  secondary O2 trim (A: B2/B4) (%)
  OBD2_PID_01_ST_O2_TRIM_B1_S2_S4_S6           = 0x58,  // Short-term secondary O2 trim (B1S2/S4/S6) (%)
  OBD2_PID_01_LT_O2_TRIM_B1_S2_S4_S6           = 0x59,  // Long-term  secondary O2 trim (B1S2/S4/S6) (%)
  OBD2_PID_01_ST_O2_TRIM_B2_S2_S4_S6           = 0x5A,  // Short-term secondary O2 trim (B2S2/S4/S6) (%)
  OBD2_PID_01_LT_O2_TRIM_B2_S2_S4_S6           = 0x5B,  // Long-term  secondary O2 trim (B2S2/S4/S6) (%)
  OBD2_PID_01_FUEL_RAIL_ABS_PRESSURE           = 0x5C,  // Fuel rail absolute pressure (kPa)
  OBD2_PID_01_RELATIVE_ACCELERATOR_POS         = 0x5D,  // Relative accelerator pedal position (%)
  OBD2_PID_01_ENGINE_FUEL_RATE                 = 0x5E,  // Engine fuel rate (L/h)
  OBD2_PID_01_EMISSION_REQUIREMENTS_DESIGN     = 0x5F,  // Emission requirements to which vehicle is designed (enum)

  /* 0x60–0x7F */
  OBD2_PID_01_SUPPORTED_61_80                  = 0x60, // PIDs supported [61–80] (bitmask)
  OBD2_PID_01_DRIVERS_DEMAND_TORQUE_PERCENT    = 0x61, // Driver’s demanded engine torque (%   = A−125)
  OBD2_PID_01_ACTUAL_ENGINE_TORQUE_PERCENT     = 0x62, // Actual engine torque (%   = A−125)
  OBD2_PID_01_ENGINE_REFERENCE_TORQUE          = 0x63, // Engine reference torque (N·m, word)
  OBD2_PID_01_ENGINE_PERCENT_TORQUE_DATA       = 0x64, // Engine percent torque data (grouped bytes)
  OBD2_PID_01_AUX_IO_SUPPORTED                 = 0x65, // Supported auxiliary I/O (bitfield/word)
  OBD2_PID_01_MAF_SENSOR_A_B                   = 0x66, // MAF sensor A & B (prefix + two words)
  OBD2_PID_01_COOLANT_TEMP_SENSORS_1_2         = 0x67, // Coolant temperature sensors 1 & 2 (prefix + two bytes)
  OBD2_PID_01_IAT_SENSORS_1_2                  = 0x68, // Intake air temperature sensors 1 & 2 (prefix + two bytes)
  OBD2_PID_01_EGR_ACT_CMD_ERR                  = 0x69, // EGR commanded/actual/error (implementation-specific)
  OBD2_PID_01_DIESEL_INTAKE_AIR_FLOW_CTRL      = 0x6A, // Diesel intake air flow control (implementation-specific)
  OBD2_PID_01_EGR_TEMPERATURE                  = 0x6B, // EGR temperature (implementation-specific)
  OBD2_PID_01_THROTTLE_ACT_CTRL_REL_POS        = 0x6C, // Throttle actuator control relative position (%)
  OBD2_PID_01_FUEL_PRESSURE_CONTROL_SYS        = 0x6D, // Fuel pressure control system status/values
  OBD2_PID_01_INJECTION_PRESSURE_CONTROL_SYS   = 0x6E, // Injection pressure control system status/values
  OBD2_PID_01_TURBO_COMPRESSOR_INLET_PRESS     = 0x6F, // Turbo compressor inlet pressure
  OBD2_PID_01_BOOST_PRESSURE_CONTROL           = 0x70, // Boost pressure control
  OBD2_PID_01_VGT_CONTROL                      = 0x71, // Variable-geometry turbo (VGT) control
  OBD2_PID_01_WASTEGATE_CONTROL                = 0x72, // Wastegate control
  OBD2_PID_01_EXHAUST_PRESSURE                 = 0x73, // Exhaust pressure
  OBD2_PID_01_TURBOCHARGER_RPM                 = 0x74, // Turbocharger speed (RPM)
  OBD2_PID_01_TURBOCHARGER_TEMPERATURE_1       = 0x75, // Turbocharger temperature #1
  OBD2_PID_01_TURBOCHARGER_TEMPERATURE_2       = 0x76, // Turbocharger temperature #2
  OBD2_PID_01_CACT                             = 0x77, // Charge air cooler temperature (°C)
  OBD2_PID_01_EXHAUST_GAS_TEMP_B1              = 0x78, // Exhaust gas temperature bank 1 (mask + 4 words)
  OBD2_PID_01_EXHAUST_GAS_TEMP_B2              = 0x79, // Exhaust gas temperature bank 2 (mask + 4 words)
  OBD2_PID_01_DPF_DIFFERENTIAL_PRESSURE        = 0x7A, // DPF differential pressure
  OBD2_PID_01_DPF                              = 0x7B, // DPF parameters (grouped)
  OBD2_PID_01_DPF_TEMPERATURE                  = 0x7C, // DPF temperature (mask + 4 words)
  OBD2_PID_01_NOX_NTE_STATUS                   = 0x7D, // NOx NTE control area status
  OBD2_PID_01_PM_NTE_STATUS                    = 0x7E, // PM  NTE control area status
  OBD2_PID_01_ENGINE_RUNTIME_EXT               = 0x7F, // Engine runtime (extended/grouped)

  OBD2_PID_01_MAX
} obd2_pid_mode_01_t;

typedef enum
{
  OBD2_PID_09_SUPPORTED_01_20                = 0x00,  // N/A (bitmask)
  OBD2_PID_09_VIN_MESSAGE_COUNT              = 0x01,  // N/A (count)
  OBD2_PID_09_VIN                            = 0x02,  // N/A (17 ASCII chars)
  OBD2_PID_09_CALID_MESSAGE_COUNT            = 0x03,  // N/A (count)
  OBD2_PID_09_CALID                          = 0x04,  // N/A (ASCII string ≤16)
  OBD2_PID_09_CVN_MESSAGE_COUNT              = 0x05,  // N/A (count)
  OBD2_PID_09_CVN                            = 0x06,  // N/A (4-byte checksum value)
  OBD2_PID_09_IUPR_MESSAGE_COUNT             = 0x07,  // N/A (count)
  OBD2_PID_09_IUPR_SPARK                     = 0x08,  // N/A (structured counters)
  OBD2_PID_09_ECU_NAME                       = 0x09,  // N/A (ASCII)
  OBD2_PID_09_HARDWARE_NUMBER                = 0x0A,  // N/A (ASCII / OEM-specific)
  OBD2_PID_09_OBD2_COMPLIANCE                = 0x0B,  // N/A (enum)
  OBD2_PID_09_RESERVED_0x0C                  = 0x0C,  // N/A
  OBD2_PID_09_RESERVED_0x0D                  = 0x0D,  // N/A
  OBD2_PID_09_RESERVED_0x0E                  = 0x0E,  // N/A
  OBD2_PID_09_RESERVED_0x0F                  = 0x0F,  // N/A
  OBD2_PID_09_VIN_DUPLICATE_FRAME            = 0x10,  // N/A (legacy VIN frame)
  OBD2_PID_09_SUPPORTED_21_40                = 0x20,  // N/A (bitmask)

  OBD2_PID_09_RESERVED_0x11                  = 0x11,  // N/A
  OBD2_PID_09_RESERVED_0x12                  = 0x12,  // N/A
  OBD2_PID_09_RESERVED_0x13                  = 0x13,  // N/A
  OBD2_PID_09_RESERVED_0x14                  = 0x14,  // N/A
  OBD2_PID_09_RESERVED_0x15                  = 0x15,  // N/A
  OBD2_PID_09_RESERVED_0x16                  = 0x16,  // N/A
  OBD2_PID_09_RESERVED_0x17                  = 0x17,  // N/A
  OBD2_PID_09_RESERVED_0x18                  = 0x18,  // N/A
  OBD2_PID_09_RESERVED_0x19                  = 0x19,  // N/A
  OBD2_PID_09_RESERVED_0x1A                  = 0x1A,  // N/A
  OBD2_PID_09_RESERVED_0x1B                  = 0x1B,  // N/A
  OBD2_PID_09_RESERVED_0x1C                  = 0x1C,  // N/A
  OBD2_PID_09_RESERVED_0x1D                  = 0x1D,  // N/A
  OBD2_PID_09_RESERVED_0x1E                  = 0x1E,  // N/A
  OBD2_PID_09_RESERVED_0x1F                  = 0x1F,  // N/A

  OBD2_PID_09_MANUFACTURER_DEFINED_START     = 0x30,  // N/A
  OBD2_PID_09_MANUFACTURER_DEFINED_END       = 0x7F,  // N/A
  OBD2_PID_09_MAX
}obd2_pid_mode_09_t;

typedef enum
{
  OBD2_RESPONSE_OK                                            = 0x00,  // Not an actual NRC, used for convenience

  OBD2_RESPONSE_GENERAL_REJECT                                = 0x10,  // General reject
  OBD2_RESPONSE_SERVICE_NOT_SUPPORTED                         = 0x11,  // Service not supported
  OBD2_RESPONSE_SUBFUNCTION_NOT_SUPPORTED                     = 0x12,  // Sub-function not supported
  OBD2_RESPONSE_INVALID_MESSAGE_LENGTH_OR_FORMAT              = 0x13,  // Invalid message length or format
  OBD2_RESPONSE_REQUEST_OUT_OF_RANGE                          = 0x31,  // Parameter or DID out of range

  OBD2_RESPONSE_REQUEST_CORRECTLY_RECEIVED_RESPONSE_PENDING   = 0x78,  // ECU accepted request but processing is pending (Response Pending)
}obd2_response_code_t;

typedef enum
{
  OBD2_MON_MISFIRE            = 1 << 0,
  OBD2_MON_FUEL_SYSTEM        = 1 << 1,
  OBD2_MON_COMPONENTS         = 1 << 2,
  OBD2_MON_CATALYST           = 1 << 3,
  OBD2_MON_HEATED_CATALYST    = 1 << 4,
  OBD2_MON_EVAP_SYSTEM        = 1 << 5,
  OBD2_MON_SECONDARY_AIR      = 1 << 6,
  OBD2_MON_AC_REFRIGERANT     = 1 << 7,
  OBD2_MON_O2_SENSOR          = 1 << 8,
  OBD2_MON_O2_HEATER          = 1 << 9,
  OBD2_MON_EGR_SYSTEM         = 1 << 10
}obd2_monitor_bit_t;

typedef struct
{
  bool mil_on;
  uint8_t dtc_count;
  obd2_monitor_bit_t monitors_ready;
}obd2_monitor_status_t;

typedef enum
{
  OBD2_FUEL_CLOSED_LOOP_O2        = 0x01,
  OBD2_FUEL_OPEN_LOOP_COLD        = 0x02,
  OBD2_FUEL_OPEN_LOOP_LOAD        = 0x04,
  OBD2_FUEL_OPEN_LOOP_FAILURE     = 0x08,
  OBD2_FUEL_CLOSED_LOOP_OTHER     = 0x10
}obd2_fuel_status_t;

typedef enum
{
  OBD2_SEC_AIR_UPSTREAM_CAT       = 0,
  OBD2_SEC_AIR_DOWNSTREAM_CAT     = 1,
  OBD2_SEC_AIR_FROM_ATMOSPHERE    = 2,
  OBD2_SEC_AIR_OFF                = 3,
  OBD2_SEC_AIR_NOT_PRESENT        = 4
}obd2_secondary_air_t;

typedef enum
{
  OBD2_STD_NONE                   = 0,
  OBD2_STD_OBD_II_CARB            = 1,
  OBD2_STD_OBD_EPA                = 2,
  OBD2_STD_OBD_OBDII              = 3,
  OBD2_STD_EOBD                   = 6,
  OBD2_STD_EOBD_OBDII             = 7,
  OBD2_STD_JOBD                   = 10,
  OBD2_STD_HD_OBD_DIESEL          = 17
}obd2_standard_t;

typedef struct
{
  bool pto_active;
  bool ac_clutch;
}obd2_aux_input_t;

typedef enum
{
  OBD2_FUEL_GASOLINE              = 1,
  OBD2_FUEL_METHANOL              = 2,
  OBD2_FUEL_ETHANOL               = 3,
  OBD2_FUEL_DIESEL                = 4,
  OBD2_FUEL_LPG                   = 5,
  OBD2_FUEL_CNG                   = 6,
  OBD2_FUEL_PROPANE               = 7,
  OBD2_FUEL_ELECTRIC              = 8,
  OBD2_FUEL_BIFUEL_GASOLINE       = 9,
  OBD2_FUEL_BIFUEL_METHANOL       = 10,
  OBD2_FUEL_BIFUEL_ETHANOL        = 11,
  OBD2_FUEL_BIFUEL_LPG            = 12,
  OBD2_FUEL_BIFUEL_CNG            = 13,
  OBD2_FUEL_BIFUEL_PROPANE        = 14,
  OBD2_FUEL_BIFUEL_ELECTRIC       = 15,
  OBD2_FUEL_HYBRID_GASOLINE       = 17,
  OBD2_FUEL_HYBRID_DIESEL         = 19,
  OBD2_FUEL_HYBRID_ELECTRIC       = 20,
  OBD2_FUEL_HYDROGEN              = 24
}obd2_fuel_type_t;

typedef struct obd2_ctx_tag obd2_ctx_t;

typedef void (*obd2_error_callback_t)(obd2_ctx_t *ctx, obd2_error_code_t code, void *userdata);


typedef enum
{
  OBD2_PID_TYPE_UNDEFINED = 0,
  OBD2_PID_TYPE_RAW_SINGLE_BYTE,
  OBD2_PID_TYPE_RAW_SINGLE_WORD,
  OBD2_PID_TYPE_RAW_SINGLE_DWORD,
  OBD2_PID_TYPE_RAW_DUAL_BYTES,
  OBD2_PID_TYPE_RAW_DUAL_WORD,
  OBD2_PID_TYPE_RAW_QUAD_BYTES,
  OBD2_PID_TYPE_SINGLE_BYTE,
  OBD2_PID_TYPE_SINGLE_WORD,
  OBD2_PID_TYPE_SINGLE_DWORD,
  OBD2_PID_TYPE_DUAL_BYTES,
  OBD2_PID_TYPE_DUAL_WORDS,
  OBD2_PID_TYPE_QUAD_BYTES,
  OBD2_PID_TYPE_PREFIX_BYTE_PLUS_DUAL_BYTES,
  OBD2_PID_TYPE_PREFIX_BYTE_PLUS_QUAD_BYTES,
  OBD2_PID_TYPE_PREFIX_BYTE_PLUS_DUAL_WORDS,
  OBD2_PID_TYPE_PREFIX_BYTE_PLUS_QUAD_WORDS,
  OBD2_PID_TYPE_MAX
}obd2_mode1_pid_type_t;

typedef struct {
    float gain;
    float offset;
}obd2_mode1_setup_gain_offset_t;

typedef struct {
    obd2_mode1_pid_type_t type;
    obd2_mode1_setup_gain_offset_t gain_offset[4];
}obd2_mode1_setup_t;

typedef struct {
    float flt;
    uint32_t raw;
}obd2_mode1_data_value_t;

typedef struct {
    bool supported;
    uint8_t prefix_byte;
    obd2_mode1_data_value_t value[4];
}obd2_mode1_data_t;

typedef struct {
    bool supported;
    uint8_t count;
    uint16_t length[4];
    const uint8_t *value[4];
}obd2_mode9_data_t;

typedef struct {
    obd2_error_callback_t error_callback;
    void *callback_userdata;
}obd2_init_ctx_t;

typedef struct obd2_ctx_tag {
    obd2_config_t config;
    obd2_init_ctx_t init;
    bool initialized;
    bool configured;

    const obd2_mode1_setup_t *mode1_setup;
    obd2_mode1_data_t mode1_data[OBD2_PID_01_MAX];
    obd2_mode9_data_t mode9_data[OBD2_PID_09_MAX];

    obd2_error_code_t error_code;
    bool reset_trigger;

    uint8_t upstream_data[OBD2_DATA_LENGTH_MAX];
    uint16_t upstream_data_len;
    bool upstream_available;

    uint8_t downstream_data[OBD2_DATA_LENGTH_MAX];
    uint16_t downstream_data_len;
    bool downstream_available;

    time_us_t downstream_time;

}obd2_ctx_t;

#endif /* COMMUNICATION_OBD2_INC_OBD2_TYPES_H_ */
