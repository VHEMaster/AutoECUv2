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
  OBD2_PID_01_SUPPORTED_01_20                  = 0x00,  // N/A (bitmask of supported PIDs)
  OBD2_PID_01_MONITOR_STATUS_SINCE_DTC_CLEAR   = 0x01,  // N/A (bitfield)
  OBD2_PID_01_FREEZE_DTC                       = 0x02,  // N/A (DTC code)
  OBD2_PID_01_FUEL_SYSTEM_STATUS               = 0x03,  // N/A (enum)
  OBD2_PID_01_CALCULATED_ENGINE_LOAD           = 0x04,  // PHYS[%] = A * (100/255) + 0   (GAIN=100/255, OFFSET=0)
  OBD2_PID_01_COOLANT_TEMPERATURE              = 0x05,  // PHYS[°C] = A * 1 + (-40)      (GAIN=1, OFFSET=-40)
  OBD2_PID_01_ST_FUEL_TRIM_B1                  = 0x06,  // PHYS[%] = (A - 128) * (100/128) + 0   (GAIN=100/128, OFFSET= -100*128/128)
  OBD2_PID_01_LT_FUEL_TRIM_B1                  = 0x07,  // PHYS[%] = (A - 128) * (100/128) + 0
  OBD2_PID_01_ST_FUEL_TRIM_B2                  = 0x08,  // PHYS[%] = (A - 128) * (100/128) + 0
  OBD2_PID_01_LT_FUEL_TRIM_B2                  = 0x09,  // PHYS[%] = (A - 128) * (100/128) + 0
  OBD2_PID_01_FUEL_PRESSURE                    = 0x0A,  // PHYS[kPa] = A * 3 + 0          (GAIN=3, OFFSET=0)
  OBD2_PID_01_INTAKE_MANIFOLD_PRESSURE         = 0x0B,  // PHYS[kPa] = A * 1 + 0          (GAIN=1, OFFSET=0)
  OBD2_PID_01_ENGINE_RPM                       = 0x0C,  // PHYS[rpm] = ((256*A + B) * 1/4) + 0   (GAIN=0.25, OFFSET=0)
  OBD2_PID_01_VEHICLE_SPEED                    = 0x0D,  // PHYS[km/h] = A * 1 + 0         (GAIN=1, OFFSET=0)
  OBD2_PID_01_TIMING_ADVANCE                   = 0x0E,  // PHYS[°BTDC] = (A * 0.5) + (-64) (GAIN=0.5, OFFSET=-64)
  OBD2_PID_01_INTAKE_AIR_TEMPERATURE           = 0x0F,  // PHYS[°C] = A * 1 + (-40)       (GAIN=1, OFFSET=-40)
  OBD2_PID_01_MAF_AIR_FLOW_RATE                = 0x10,  // PHYS[g/s] = ((256*A + B) * 1/100) + 0  (GAIN=0.01, OFFSET=0)
  OBD2_PID_01_THROTTLE_POSITION                = 0x11,  // PHYS[%] = A * (100/255) + 0    (GAIN=100/255, OFFSET=0)
  OBD2_PID_01_COMMANDED_SECONDARY_AIR          = 0x12,  // N/A (enum)
  OBD2_PID_01_O2_SENSORS_PRESENT_2BANKS        = 0x13,  // N/A (bitmask)
  OBD2_PID_01_O2_SENSOR1_V_AND_STFT            = 0x14,  // V[Volt] = A * (1/200) + 0;  STFT[%] = (B - 128)*(100/128)
  OBD2_PID_01_O2_SENSOR2_V_AND_STFT            = 0x15,  // V = A/200; STFT = (B-128)*100/128
  OBD2_PID_01_O2_SENSOR3_V_AND_STFT            = 0x16,  // V = A/200; STFT = (B-128)*100/128
  OBD2_PID_01_O2_SENSOR4_V_AND_STFT            = 0x17,  // V = A/200; STFT = (B-128)*100/128
  OBD2_PID_01_O2_SENSOR5_V_AND_STFT            = 0x18,  // V = A/200; STFT = (B-128)*100/128
  OBD2_PID_01_O2_SENSOR6_V_AND_STFT            = 0x19,  // V = A/200; STFT = (B-128)*100/128
  OBD2_PID_01_O2_SENSOR7_V_AND_STFT            = 0x1A,  // V = A/200; STFT = (B-128)*100/128
  OBD2_PID_01_O2_SENSOR8_V_AND_STFT            = 0x1B,  // V = A/200; STFT = (B-128)*100/128
  OBD2_PID_01_OBD2_STANDARDS_IN_USE            = 0x1C,  // N/A (enum)
  OBD2_PID_01_O2_SENSORS_PRESENT_4BANKS        = 0x1D,  // N/A (bitmask)
  OBD2_PID_01_AUX_INPUT_STATUS                 = 0x1E,  // N/A (bitfield)
  OBD2_PID_01_RUNTIME_SINCE_ENGINE_START       = 0x1F,  // PHYS[s] = ((256*A + B) * 1) + 0  (GAIN=1, OFFSET=0)

  /* 0x20–0x3F */
  OBD2_PID_01_SUPPORTED_21_40                  = 0x20,  // N/A (bitmask)
  OBD2_PID_01_DISTANCE_WITH_MIL_ON             = 0x21,  // PHYS[km] = ((256*A + B) * 1) + 0     (GAIN=1, OFFSET=0)
  OBD2_PID_01_FUEL_RAIL_PRESS_REL_TO_MANIF     = 0x22,  // PHYS[kPa] = ((256*A + B) * 0.079) + 0 (GAIN=0.079, OFFSET=0)
  OBD2_PID_01_FUEL_RAIL_PRESSURE_DIRECT        = 0x23,  // PHYS[kPa] = ((256*A + B) * 10) + 0    (GAIN=10, OFFSET=0)
  OBD2_PID_01_O2_S1_EQUIV_RATIO_AND_VOLT       = 0x24,  // λ = ((256*A + B) * 1/32768) + 0;  U[V] = ((256*C + D) * 1/200) + 0
  OBD2_PID_01_O2_S2_EQUIV_RATIO_AND_VOLT       = 0x25,  // λ = AB/32768; U = CD/200
  OBD2_PID_01_O2_S3_EQUIV_RATIO_AND_VOLT       = 0x26,  // λ = AB/32768; U = CD/200
  OBD2_PID_01_O2_S4_EQUIV_RATIO_AND_VOLT       = 0x27,  // λ = AB/32768; U = CD/200
  OBD2_PID_01_O2_S5_EQUIV_RATIO_AND_VOLT       = 0x28,  // λ = AB/32768; U = CD/200
  OBD2_PID_01_O2_S6_EQUIV_RATIO_AND_VOLT       = 0x29,  // λ = AB/32768; U = CD/200
  OBD2_PID_01_O2_S7_EQUIV_RATIO_AND_VOLT       = 0x2A,  // λ = AB/32768; U = CD/200
  OBD2_PID_01_O2_S8_EQUIV_RATIO_AND_VOLT       = 0x2B,  // λ = AB/32768; U = CD/200
  OBD2_PID_01_COMMANDED_EGR                    = 0x2C,  // PHYS[%] = A * (100/255) + 0
  OBD2_PID_01_EGR_ERROR                        = 0x2D,  // PHYS[%] = (A - 128) * (100/128) + 0
  OBD2_PID_01_COMMANDED_EVAP_PURGE             = 0x2E,  // PHYS[%] = A * (100/255) + 0
  OBD2_PID_01_FUEL_LEVEL_INPUT                 = 0x2F,  // PHYS[%] = A * (100/255) + 0
  OBD2_PID_01_WARMUPS_SINCE_DTC_CLEAR          = 0x30,  // PHYS[count] = A * 1 + 0
  OBD2_PID_01_DISTANCE_SINCE_DTC_CLEAR         = 0x31,  // PHYS[km] = ((256*A + B) * 1) + 0
  OBD2_PID_01_EVAP_VAPOR_PRESSURE_ABS_LEGACY   = 0x32,  // PHYS[Pa] = ((256*A + B) * 1/4) + 0    (GAIN=0.25, OFFSET=0)
  OBD2_PID_01_BAROMETRIC_PRESSURE              = 0x33,  // PHYS[kPa] = A * 1 + 0
  OBD2_PID_01_O2_S1_CURRENT_AND_EQUIV          = 0x34,  // λ = ((256*A + B)/32768);  I[mA] = ((256*C + D) * 1/256) + (-128)
  OBD2_PID_01_O2_S2_CURRENT_AND_EQUIV          = 0x35,  // λ = AB/32768; I = (CD/256) - 128
  OBD2_PID_01_O2_S3_CURRENT_AND_EQUIV          = 0x36,  // λ = AB/32768; I = (CD/256) - 128
  OBD2_PID_01_O2_S4_CURRENT_AND_EQUIV          = 0x37,  // λ = AB/32768; I = (CD/256) - 128
  OBD2_PID_01_O2_S5_CURRENT_AND_EQUIV          = 0x38,  // λ = AB/32768; I = (CD/256) - 128
  OBD2_PID_01_O2_S6_CURRENT_AND_EQUIV          = 0x39,  // λ = AB/32768; I = (CD/256) - 128
  OBD2_PID_01_O2_S7_CURRENT_AND_EQUIV          = 0x3A,  // λ = AB/32768; I = (CD/256) - 128
  OBD2_PID_01_O2_S8_CURRENT_AND_EQUIV          = 0x3B,  // λ = AB/32768; I = (CD/256) - 128
  OBD2_PID_01_CAT_TEMP_B1S1                    = 0x3C,  // PHYS[°C] = ((256*A + B) * 1/10) + (-40)   (GAIN=0.1, OFFSET=-40)
  OBD2_PID_01_CAT_TEMP_B2S1                    = 0x3D,  // PHYS[°C] = AB/10 - 40
  OBD2_PID_01_CAT_TEMP_B1S2                    = 0x3E,  // PHYS[°C] = AB/10 - 40
  OBD2_PID_01_CAT_TEMP_B2S2                    = 0x3F,  // PHYS[°C] = AB/10 - 40

  /* 0x40–0x5F */
  OBD2_PID_01_SUPPORTED_41_60                  = 0x40,  // N/A (bitmask)
  OBD2_PID_01_MONITOR_STATUS_THIS_DRIVE_CYCLE  = 0x41,  // N/A (bitfield)
  OBD2_PID_01_CONTROL_MODULE_VOLTAGE           = 0x42,  // PHYS[V] = ((256*A + B) * 1/1000) + 0  (GAIN=0.001, OFFSET=0)
  OBD2_PID_01_ABSOLUTE_LOAD_VALUE              = 0x43,  // PHYS[%] = ((256*A + B) * 100/255) + 0 (GAIN=100/255, OFFSET=0)
  OBD2_PID_01_COMMANDED_EQUIV_RATIO            = 0x44,  // PHYS[λ] = ((256*A + B) * 1/32768) + 0 (GAIN=1/32768, OFFSET=0)
  OBD2_PID_01_RELATIVE_THROTTLE_POS            = 0x45,  // PHYS[%] = A * (100/255) + 0
  OBD2_PID_01_AMBIENT_AIR_TEMPERATURE          = 0x46,  // PHYS[°C] = A * 1 + (-40)
  OBD2_PID_01_ABS_THROTTLE_POS_B               = 0x47,  // PHYS[%] = A * (100/255) + 0
  OBD2_PID_01_ABS_THROTTLE_POS_C               = 0x48,  // PHYS[%] = A * (100/255) + 0
  OBD2_PID_01_ACCEL_PEDAL_POS_D                = 0x49,  // PHYS[%] = A * (100/255) + 0
  OBD2_PID_01_ACCEL_PEDAL_POS_E                = 0x4A,  // PHYS[%] = A * (100/255) + 0
  OBD2_PID_01_ACCEL_PEDAL_POS_F                = 0x4B,  // PHYS[%] = A * (100/255) + 0
  OBD2_PID_01_COMMANDED_THROTTLE_ACTUATOR      = 0x4C,  // PHYS[%] = A * (100/255) + 0
  OBD2_PID_01_TIME_WITH_MIL_ON                 = 0x4D,  // PHYS[min] = ((256*A + B) * 1) + 0
  OBD2_PID_01_TIME_SINCE_DTC_CLEAR             = 0x4E,  // PHYS[min] = ((256*A + B) * 1) + 0
  OBD2_PID_01_MAX_MAF_SINCE_DTC_CLEAR          = 0x4F,  // PHYS[g/s] = ((256*A + B) * 1/100) + 0
  OBD2_PID_01_FUEL_TYPE                        = 0x50,  // N/A (enum)
  OBD2_PID_01_ETHANOL_FUEL_PERCENT             = 0x51,  // PHYS[%] = A * (100/255) + 0
  OBD2_PID_01_EVAP_VAPOR_PRESSURE_ABSOLUTE     = 0x52,  // PHYS[Pa] = ((256*A + B) * 1/200) + (-8192)   (GAIN=1/200, OFFSET=-8192)
  OBD2_PID_01_EVAP_VAPOR_PRESSURE_ALT          = 0x53,  // PHYS[Pa] = ((256*A + B) * 1/4)   + (-8192)   (GAIN=1/4,   OFFSET=-8192)
  OBD2_PID_01_ST_O2_TRIM_B1_S1_S3_S5           = 0x54,  // PHYS[%] = (A - 128) * (100/128) + 0 (per-sensor grouping)
  OBD2_PID_01_LT_O2_TRIM_B1_S1_S3_S5           = 0x55,  // PHYS[%] = (A - 128) * (100/128) + 0
  OBD2_PID_01_ST_O2_TRIM_B2_S1_S3_S5           = 0x56,  // PHYS[%] = (A - 128) * (100/128) + 0
  OBD2_PID_01_LT_O2_TRIM_B2_S1_S3_S5           = 0x57,  // PHYS[%] = (A - 128) * (100/128) + 0
  OBD2_PID_01_ST_O2_TRIM_B1_S2_S4_S6           = 0x58,  // PHYS[%] = (A - 128) * (100/128) + 0
  OBD2_PID_01_LT_O2_TRIM_B1_S2_S4_S6           = 0x59,  // PHYS[%] = (A - 128) * (100/128) + 0
  OBD2_PID_01_ST_O2_TRIM_B2_S2_S4_S6           = 0x5A,  // PHYS[%] = (A - 128) * (100/128) + 0
  OBD2_PID_01_LT_O2_TRIM_B2_S2_S4_S6           = 0x5B,  // PHYS[%] = (A - 128) * (100/128) + 0
  OBD2_PID_01_FUEL_RAIL_ABS_PRESSURE           = 0x5C,  // PHYS[kPa] = ((256*A + B) * 10) + 0
  OBD2_PID_01_RELATIVE_ACCELERATOR_POS         = 0x5D,  // PHYS[%] = A * (100/255) + 0
  OBD2_PID_01_ENGINE_FUEL_RATE                 = 0x5E,  // PHYS[L/h] = ((256*A + B) * 1/20) + 0 (GAIN=0.05, OFFSET=0)
  OBD2_PID_01_EMISSION_REQUIREMENTS_DESIGN     = 0x5F,  // N/A (enum)

  OBD2_PID_01_SUPPORTED_61_80                  = 0x60,  // N/A (bitmask)
  OBD2_PID_01_MANUFACTURER_DEFINED_0x61        = 0x61,  // N/A (OEM-defined)
  OBD2_PID_01_MANUFACTURER_DEFINED_0x62        = 0x62,  // N/A
  OBD2_PID_01_MANUFACTURER_DEFINED_0x63        = 0x63,  // N/A
  OBD2_PID_01_MANUFACTURER_DEFINED_0x64        = 0x64,  // N/A
  OBD2_PID_01_MANUFACTURER_DEFINED_0x65        = 0x65,  // N/A
  OBD2_PID_01_MANUFACTURER_DEFINED_0x66        = 0x66,  // N/A
  OBD2_PID_01_MANUFACTURER_DEFINED_0x67        = 0x67,  // N/A
  OBD2_PID_01_MANUFACTURER_DEFINED_0x68        = 0x68,  // N/A
  OBD2_PID_01_MANUFACTURER_DEFINED_0x69        = 0x69,  // N/A
  OBD2_PID_01_MANUFACTURER_DEFINED_0x6A        = 0x6A,  // N/A
  OBD2_PID_01_MANUFACTURER_DEFINED_0x6B        = 0x6B,  // N/A
  OBD2_PID_01_MANUFACTURER_DEFINED_0x6C        = 0x6C,  // N/A
  OBD2_PID_01_MANUFACTURER_DEFINED_0x6D        = 0x6D,  // N/A
  OBD2_PID_01_MANUFACTURER_DEFINED_0x6E        = 0x6E,  // N/A
  OBD2_PID_01_MANUFACTURER_DEFINED_0x6F        = 0x6F,  // N/A
  OBD2_PID_01_MANUFACTURER_DEFINED_0x70        = 0x70,  // N/A
  OBD2_PID_01_MANUFACTURER_DEFINED_0x71        = 0x71,  // N/A
  OBD2_PID_01_MANUFACTURER_DEFINED_0x72        = 0x72,  // N/A
  OBD2_PID_01_MANUFACTURER_DEFINED_0x73        = 0x73,  // N/A
  OBD2_PID_01_MANUFACTURER_DEFINED_0x74        = 0x74,  // N/A
  OBD2_PID_01_MANUFACTURER_DEFINED_0x75        = 0x75,  // N/A
  OBD2_PID_01_MANUFACTURER_DEFINED_0x76        = 0x76,  // N/A
  OBD2_PID_01_MANUFACTURER_DEFINED_0x77        = 0x77,  // N/A
  OBD2_PID_01_EXHAUST_GAS_TEMP_B1              = 0x78,  // PHYS[°C] = ((256*A + B) * 1/10) + 0
  OBD2_PID_01_EXHAUST_GAS_TEMP_B2              = 0x79,  // PHYS[°C] = AB/10
  OBD2_PID_01_MANUFACTURER_DEFINED_0x7A        = 0x7A,  // N/A
  OBD2_PID_01_MANUFACTURER_DEFINED_0x7B        = 0x7B,  // N/A
  OBD2_PID_01_MANUFACTURER_DEFINED_0x7C        = 0x7C,  // N/A
  OBD2_PID_01_MANUFACTURER_DEFINED_0x7D        = 0x7D,  // N/A
  OBD2_PID_01_MANUFACTURER_DEFINED_0x7E        = 0x7E,  // N/A
  OBD2_PID_01_MANUFACTURER_DEFINED_0x7F        = 0x7F,  // N/A

  OBD2_PID_01_MAX
}obd2_pid_mode_01_t;

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

typedef struct
{
  obd2_fuel_status_t bank1;
  obd2_fuel_status_t bank2;
}obd2_fuel_system_t;

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
