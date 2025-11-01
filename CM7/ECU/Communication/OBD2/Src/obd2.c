/*
 * obd2.c
 *
 *  Created on: Oct 23, 2025
 *      Author: VHEMaster
 */

#include "obd2.h"
#include "obd2_private.h"

static const obd2_mode1_setup_t obd2_mode1_setup[OBD2_PID_01_MAX] = {
  { .type = OBD2_PID_TYPE_RAW_SINGLE_DWORD }, // OBD2_PID_01_SUPPORTED_01_20 (0x00)
  { .type = OBD2_PID_TYPE_RAW_QUAD_BYTES }, // OBD2_PID_01_MONITOR_STATUS_SINCE_DTC_CLEAR (0x01)
  { .type = OBD2_PID_TYPE_RAW_SINGLE_WORD }, // OBD2_PID_01_FREEZE_DTC (0x02)
  { .type = OBD2_PID_TYPE_RAW_DUAL_BYTES }, // OBD2_PID_01_FUEL_SYSTEM_STATUS (0x03)
  { .type = OBD2_PID_TYPE_SINGLE_BYTE, .gain_offset = { { .gain = 100.0f/255.0f, .offset = 0.0f } } }, // OBD2_PID_01_CALCULATED_ENGINE_LOAD (0x04)
  { .type = OBD2_PID_TYPE_SINGLE_BYTE, .gain_offset = { { .gain = 1.0f, .offset = -40.0f } } }, // OBD2_PID_01_COOLANT_TEMPERATURE (0x05)
  { .type = OBD2_PID_TYPE_SINGLE_BYTE, .gain_offset = { { .gain = 100.0f/128.0f, .offset = -100.0f } } }, // OBD2_PID_01_ST_FUEL_TRIM_B1 (0x06)
  { .type = OBD2_PID_TYPE_SINGLE_BYTE, .gain_offset = { { .gain = 100.0f/128.0f, .offset = -100.0f } } }, // OBD2_PID_01_LT_FUEL_TRIM_B1 (0x07)
  { .type = OBD2_PID_TYPE_SINGLE_BYTE, .gain_offset = { { .gain = 100.0f/128.0f, .offset = -100.0f } } }, // OBD2_PID_01_ST_FUEL_TRIM_B2 (0x08)
  { .type = OBD2_PID_TYPE_SINGLE_BYTE, .gain_offset = { { .gain = 100.0f/128.0f, .offset = -100.0f } } }, // OBD2_PID_01_LT_FUEL_TRIM_B2 (0x09)
  { .type = OBD2_PID_TYPE_SINGLE_BYTE, .gain_offset = { { .gain = 3.0f, .offset = 0.0f } } }, // OBD2_PID_01_FUEL_PRESSURE (0x0A)
  { .type = OBD2_PID_TYPE_SINGLE_BYTE, .gain_offset = { { .gain = 1.0f, .offset = 0.0f } } }, // OBD2_PID_01_INTAKE_MANIFOLD_PRESSURE (0x0B)
  { .type = OBD2_PID_TYPE_SINGLE_WORD, .gain_offset = { { .gain = 0.25f, .offset = 0.0f } } }, // OBD2_PID_01_ENGINE_RPM (0x0C)
  { .type = OBD2_PID_TYPE_SINGLE_BYTE, .gain_offset = { { .gain = 1.0f, .offset = 0.0f } } }, // OBD2_PID_01_VEHICLE_SPEED (0x0D)
  { .type = OBD2_PID_TYPE_SINGLE_BYTE, .gain_offset = { { .gain = 0.5f, .offset = -64.0f } } }, // OBD2_PID_01_TIMING_ADVANCE (0x0E)
  { .type = OBD2_PID_TYPE_SINGLE_BYTE, .gain_offset = { { .gain = 1.0f, .offset = -40.0f } } }, // OBD2_PID_01_INTAKE_AIR_TEMPERATURE (0x0F)
  { .type = OBD2_PID_TYPE_SINGLE_WORD, .gain_offset = { { .gain = 0.01f, .offset = 0.0f } } }, // OBD2_PID_01_MAF_AIR_FLOW_RATE (0x10)
  { .type = OBD2_PID_TYPE_SINGLE_BYTE, .gain_offset = { { .gain = 100.0f/255.0f, .offset = 0.0f } } }, // OBD2_PID_01_THROTTLE_POSITION (0x11)
  { .type = OBD2_PID_TYPE_RAW_SINGLE_BYTE }, // OBD2_PID_01_COMMANDED_SECONDARY_AIR (0x12)
  { .type = OBD2_PID_TYPE_RAW_SINGLE_BYTE }, // OBD2_PID_01_O2_SENSORS_PRESENT_2BANKS (0x13)
  { .type = OBD2_PID_TYPE_DUAL_BYTES, .gain_offset = { { .gain = 1.0f/200.0f, .offset = 0.0f }, { .gain = 100.0f/128.0f, .offset = -100.0f } } }, // OBD2_PID_01_O2_SENSOR1_V_AND_STFT (0x14)
  { .type = OBD2_PID_TYPE_DUAL_BYTES, .gain_offset = { { .gain = 1.0f/200.0f, .offset = 0.0f }, { .gain = 100.0f/128.0f, .offset = -100.0f } } }, // OBD2_PID_01_O2_SENSOR2_V_AND_STFT (0x15)
  { .type = OBD2_PID_TYPE_DUAL_BYTES, .gain_offset = { { .gain = 1.0f/200.0f, .offset = 0.0f }, { .gain = 100.0f/128.0f, .offset = -100.0f } } }, // OBD2_PID_01_O2_SENSOR3_V_AND_STFT (0x16)
  { .type = OBD2_PID_TYPE_DUAL_BYTES, .gain_offset = { { .gain = 1.0f/200.0f, .offset = 0.0f }, { .gain = 100.0f/128.0f, .offset = -100.0f } } }, // OBD2_PID_01_O2_SENSOR4_V_AND_STFT (0x17)
  { .type = OBD2_PID_TYPE_DUAL_BYTES, .gain_offset = { { .gain = 1.0f/200.0f, .offset = 0.0f }, { .gain = 100.0f/128.0f, .offset = -100.0f } } }, // OBD2_PID_01_O2_SENSOR5_V_AND_STFT (0x18)
  { .type = OBD2_PID_TYPE_DUAL_BYTES, .gain_offset = { { .gain = 1.0f/200.0f, .offset = 0.0f }, { .gain = 100.0f/128.0f, .offset = -100.0f } } }, // OBD2_PID_01_O2_SENSOR6_V_AND_STFT (0x19)
  { .type = OBD2_PID_TYPE_DUAL_BYTES, .gain_offset = { { .gain = 1.0f/200.0f, .offset = 0.0f }, { .gain = 100.0f/128.0f, .offset = -100.0f } } }, // OBD2_PID_01_O2_SENSOR7_V_AND_STFT (0x1A)
  { .type = OBD2_PID_TYPE_DUAL_BYTES, .gain_offset = { { .gain = 1.0f/200.0f, .offset = 0.0f }, { .gain = 100.0f/128.0f, .offset = -100.0f } } }, // OBD2_PID_01_O2_SENSOR8_V_AND_STFT (0x1B)
  { .type = OBD2_PID_TYPE_RAW_SINGLE_BYTE }, // OBD2_PID_01_OBD2_STANDARDS_IN_USE (0x1C)
  { .type = OBD2_PID_TYPE_RAW_SINGLE_WORD }, // OBD2_PID_01_O2_SENSORS_PRESENT_4BANKS (0x1D)
  { .type = OBD2_PID_TYPE_RAW_SINGLE_BYTE }, // OBD2_PID_01_AUX_INPUT_STATUS (0x1E)
  { .type = OBD2_PID_TYPE_SINGLE_WORD, .gain_offset = { { .gain = 1.0f, .offset = 0.0f } } }, // OBD2_PID_01_RUNTIME_SINCE_ENGINE_START (0x1F)

  { .type = OBD2_PID_TYPE_RAW_SINGLE_DWORD }, // OBD2_PID_01_SUPPORTED_21_40 (0x20)
  { .type = OBD2_PID_TYPE_SINGLE_WORD, .gain_offset = { { .gain = 1.0f, .offset = 0.0f } } }, // OBD2_PID_01_DISTANCE_WITH_MIL_ON (0x21)
  { .type = OBD2_PID_TYPE_SINGLE_WORD, .gain_offset = { { .gain = 0.079f, .offset = 0.0f } } }, // OBD2_PID_01_FUEL_RAIL_PRESS_REL_TO_MANIF (0x22)
  { .type = OBD2_PID_TYPE_SINGLE_WORD, .gain_offset = { { .gain = 10.0f, .offset = 0.0f } } }, // OBD2_PID_01_FUEL_RAIL_PRESSURE_DIRECT (0x23)
  { .type = OBD2_PID_TYPE_DUAL_WORDS, .gain_offset = { { .gain = 1.0f/32768.0f, .offset = 0.0f }, { .gain = 1.0f/200.0f, .offset = 0.0f } } }, // OBD2_PID_01_O2_S1_EQUIV_RATIO_AND_VOLT (0x24)
  { .type = OBD2_PID_TYPE_DUAL_WORDS, .gain_offset = { { .gain = 1.0f/32768.0f, .offset = 0.0f }, { .gain = 1.0f/200.0f, .offset = 0.0f } } }, // OBD2_PID_01_O2_S2_EQUIV_RATIO_AND_VOLT (0x25)
  { .type = OBD2_PID_TYPE_DUAL_WORDS, .gain_offset = { { .gain = 1.0f/32768.0f, .offset = 0.0f }, { .gain = 1.0f/200.0f, .offset = 0.0f } } }, // OBD2_PID_01_O2_S3_EQUIV_RATIO_AND_VOLT (0x26)
  { .type = OBD2_PID_TYPE_DUAL_WORDS, .gain_offset = { { .gain = 1.0f/32768.0f, .offset = 0.0f }, { .gain = 1.0f/200.0f, .offset = 0.0f } } }, // OBD2_PID_01_O2_S4_EQUIV_RATIO_AND_VOLT (0x27)
  { .type = OBD2_PID_TYPE_DUAL_WORDS, .gain_offset = { { .gain = 1.0f/32768.0f, .offset = 0.0f }, { .gain = 1.0f/200.0f, .offset = 0.0f } } }, // OBD2_PID_01_O2_S5_EQUIV_RATIO_AND_VOLT (0x28)
  { .type = OBD2_PID_TYPE_DUAL_WORDS, .gain_offset = { { .gain = 1.0f/32768.0f, .offset = 0.0f }, { .gain = 1.0f/200.0f, .offset = 0.0f } } }, // OBD2_PID_01_O2_S6_EQUIV_RATIO_AND_VOLT (0x29)
  { .type = OBD2_PID_TYPE_DUAL_WORDS, .gain_offset = { { .gain = 1.0f/32768.0f, .offset = 0.0f }, { .gain = 1.0f/200.0f, .offset = 0.0f } } }, // OBD2_PID_01_O2_S7_EQUIV_RATIO_AND_VOLT (0x2A)
  { .type = OBD2_PID_TYPE_DUAL_WORDS, .gain_offset = { { .gain = 1.0f/32768.0f, .offset = 0.0f }, { .gain = 1.0f/200.0f, .offset = 0.0f } } }, // OBD2_PID_01_O2_S8_EQUIV_RATIO_AND_VOLT (0x2B)
  { .type = OBD2_PID_TYPE_SINGLE_BYTE, .gain_offset = { { .gain = 100.0f/255.0f, .offset = 0.0f } } }, // OBD2_PID_01_COMMANDED_EGR (0x2C)
  { .type = OBD2_PID_TYPE_SINGLE_BYTE, .gain_offset = { { .gain = 100.0f/128.0f, .offset = -100.0f } } }, // OBD2_PID_01_EGR_ERROR (0x2D)
  { .type = OBD2_PID_TYPE_SINGLE_BYTE, .gain_offset = { { .gain = 100.0f/255.0f, .offset = 0.0f } } }, // OBD2_PID_01_COMMANDED_EVAP_PURGE (0x2E)
  { .type = OBD2_PID_TYPE_SINGLE_BYTE, .gain_offset = { { .gain = 100.0f/255.0f, .offset = 0.0f } } }, // OBD2_PID_01_FUEL_LEVEL_INPUT (0x2F)
  { .type = OBD2_PID_TYPE_SINGLE_BYTE, .gain_offset = { { .gain = 1.0f, .offset = 0.0f } } }, // OBD2_PID_01_WARMUPS_SINCE_DTC_CLEAR (0x30)
  { .type = OBD2_PID_TYPE_SINGLE_WORD, .gain_offset = { { .gain = 1.0f, .offset = 0.0f } } }, // OBD2_PID_01_DISTANCE_SINCE_DTC_CLEAR (0x31)
  { .type = OBD2_PID_TYPE_SINGLE_WORD, .gain_offset = { { .gain = 0.25f, .offset = 0.0f } } }, // OBD2_PID_01_EVAP_VAPOR_PRESSURE_ABS_LEGACY (0x32)
  { .type = OBD2_PID_TYPE_SINGLE_BYTE, .gain_offset = { { .gain = 1.0f, .offset = 0.0f } } }, // OBD2_PID_01_BAROMETRIC_PRESSURE (0x33)
  { .type = OBD2_PID_TYPE_DUAL_WORDS, .gain_offset = { { .gain = 1.0f/32768.0f, .offset = 0.0f }, { .gain = 1.0f/256.0f, .offset = -128.0f } } }, // OBD2_PID_01_O2_S1_CURRENT_AND_EQUIV (0x34)
  { .type = OBD2_PID_TYPE_DUAL_WORDS, .gain_offset = { { .gain = 1.0f/32768.0f, .offset = 0.0f }, { .gain = 1.0f/256.0f, .offset = -128.0f } } }, // OBD2_PID_01_O2_S2_CURRENT_AND_EQUIV (0x35)
  { .type = OBD2_PID_TYPE_DUAL_WORDS, .gain_offset = { { .gain = 1.0f/32768.0f, .offset = 0.0f }, { .gain = 1.0f/256.0f, .offset = -128.0f } } }, // OBD2_PID_01_O2_S3_CURRENT_AND_EQUIV (0x36)
  { .type = OBD2_PID_TYPE_DUAL_WORDS, .gain_offset = { { .gain = 1.0f/32768.0f, .offset = 0.0f }, { .gain = 1.0f/256.0f, .offset = -128.0f } } }, // OBD2_PID_01_O2_S4_CURRENT_AND_EQUIV (0x37)
  { .type = OBD2_PID_TYPE_DUAL_WORDS, .gain_offset = { { .gain = 1.0f/32768.0f, .offset = 0.0f }, { .gain = 1.0f/256.0f, .offset = -128.0f } } }, // OBD2_PID_01_O2_S5_CURRENT_AND_EQUIV (0x38)
  { .type = OBD2_PID_TYPE_DUAL_WORDS, .gain_offset = { { .gain = 1.0f/32768.0f, .offset = 0.0f }, { .gain = 1.0f/256.0f, .offset = -128.0f } } }, // OBD2_PID_01_O2_S6_CURRENT_AND_EQUIV (0x39)
  { .type = OBD2_PID_TYPE_DUAL_WORDS, .gain_offset = { { .gain = 1.0f/32768.0f, .offset = 0.0f }, { .gain = 1.0f/256.0f, .offset = -128.0f } } }, // OBD2_PID_01_O2_S7_CURRENT_AND_EQUIV (0x3A)
  { .type = OBD2_PID_TYPE_DUAL_WORDS, .gain_offset = { { .gain = 1.0f/32768.0f, .offset = 0.0f }, { .gain = 1.0f/256.0f, .offset = -128.0f } } }, // OBD2_PID_01_O2_S8_CURRENT_AND_EQUIV (0x3B)
  { .type = OBD2_PID_TYPE_SINGLE_WORD, .gain_offset = { { .gain = 0.1f, .offset = -40.0f } } }, // OBD2_PID_01_CAT_TEMP_B1S1 (0x3C)
  { .type = OBD2_PID_TYPE_SINGLE_WORD, .gain_offset = { { .gain = 0.1f, .offset = -40.0f } } }, // OBD2_PID_01_CAT_TEMP_B2S1 (0x3D)
  { .type = OBD2_PID_TYPE_SINGLE_WORD, .gain_offset = { { .gain = 0.1f, .offset = -40.0f } } }, // OBD2_PID_01_CAT_TEMP_B1S2 (0x3E)
  { .type = OBD2_PID_TYPE_SINGLE_WORD, .gain_offset = { { .gain = 0.1f, .offset = -40.0f } } }, // OBD2_PID_01_CAT_TEMP_B2S2 (0x3F)

  { .type = OBD2_PID_TYPE_RAW_SINGLE_DWORD }, // OBD2_PID_01_SUPPORTED_41_60 (0x40)
  { .type = OBD2_PID_TYPE_RAW_QUAD_BYTES }, // OBD2_PID_01_MONITOR_STATUS_THIS_DRIVE_CYCLE (0x41)
  { .type = OBD2_PID_TYPE_SINGLE_WORD, .gain_offset = { { .gain = 0.001f, .offset = 0.0f } } }, // OBD2_PID_01_CONTROL_MODULE_VOLTAGE (0x42)
  { .type = OBD2_PID_TYPE_SINGLE_WORD, .gain_offset = { { .gain = 100.0f/255.0f, .offset = 0.0f } } }, // OBD2_PID_01_ABSOLUTE_LOAD_VALUE (0x43)
  { .type = OBD2_PID_TYPE_SINGLE_WORD, .gain_offset = { { .gain = 1.0f/32768.0f, .offset = 0.0f } } }, // OBD2_PID_01_COMMANDED_EQUIV_RATIO (0x44)
  { .type = OBD2_PID_TYPE_SINGLE_BYTE, .gain_offset = { { .gain = 100.0f/255.0f, .offset = 0.0f } } }, // OBD2_PID_01_RELATIVE_THROTTLE_POS (0x45)
  { .type = OBD2_PID_TYPE_SINGLE_BYTE, .gain_offset = { { .gain = 1.0f, .offset = -40.0f } } }, // OBD2_PID_01_AMBIENT_AIR_TEMPERATURE (0x46)
  { .type = OBD2_PID_TYPE_SINGLE_BYTE, .gain_offset = { { .gain = 100.0f/255.0f, .offset = 0.0f } } }, // OBD2_PID_01_ABS_THROTTLE_POS_B (0x47)
  { .type = OBD2_PID_TYPE_SINGLE_BYTE, .gain_offset = { { .gain = 100.0f/255.0f, .offset = 0.0f } } }, // OBD2_PID_01_ABS_THROTTLE_POS_C (0x48)
  { .type = OBD2_PID_TYPE_SINGLE_BYTE, .gain_offset = { { .gain = 100.0f/255.0f, .offset = 0.0f } } }, // OBD2_PID_01_ACCEL_PEDAL_POS_D (0x49)
  { .type = OBD2_PID_TYPE_SINGLE_BYTE, .gain_offset = { { .gain = 100.0f/255.0f, .offset = 0.0f } } }, // OBD2_PID_01_ACCEL_PEDAL_POS_E (0x4A)
  { .type = OBD2_PID_TYPE_SINGLE_BYTE, .gain_offset = { { .gain = 100.0f/255.0f, .offset = 0.0f } } }, // OBD2_PID_01_ACCEL_PEDAL_POS_F (0x4B)
  { .type = OBD2_PID_TYPE_SINGLE_BYTE, .gain_offset = { { .gain = 100.0f/255.0f, .offset = 0.0f } } }, // OBD2_PID_01_COMMANDED_THROTTLE_ACTUATOR (0x4C)
  { .type = OBD2_PID_TYPE_SINGLE_WORD, .gain_offset = { { .gain = 1.0f, .offset = 0.0f } } }, // OBD2_PID_01_TIME_WITH_MIL_ON (0x4D)
  { .type = OBD2_PID_TYPE_SINGLE_WORD, .gain_offset = { { .gain = 1.0f, .offset = 0.0f } } }, // OBD2_PID_01_TIME_SINCE_DTC_CLEAR (0x4E)
  { .type = OBD2_PID_TYPE_SINGLE_WORD, .gain_offset = { { .gain = 0.01f, .offset = 0.0f } } }, // OBD2_PID_01_MAX_MAF_SINCE_DTC_CLEAR (0x4F)
  { .type = OBD2_PID_TYPE_RAW_SINGLE_BYTE }, // OBD2_PID_01_FUEL_TYPE (0x50)
  { .type = OBD2_PID_TYPE_SINGLE_BYTE, .gain_offset = { { .gain = 100.0f/255.0f, .offset = 0.0f } } }, // OBD2_PID_01_ETHANOL_FUEL_PERCENT (0x51)
  { .type = OBD2_PID_TYPE_SINGLE_WORD, .gain_offset = { { .gain = 1.0f/200.0f, .offset = -8192.0f } } }, // OBD2_PID_01_EVAP_VAPOR_PRESSURE_ABSOLUTE (0x52)
  { .type = OBD2_PID_TYPE_SINGLE_WORD, .gain_offset = { { .gain = 0.25f, .offset = -8192.0f } } }, // OBD2_PID_01_EVAP_VAPOR_PRESSURE_ALT (0x53)
  { .type = OBD2_PID_TYPE_SINGLE_BYTE, .gain_offset = { { .gain = 100.0f/128.0f, .offset = -100.0f } } }, // OBD2_PID_01_ST_O2_TRIM_B1_S1_S3_S5 (0x54)
  { .type = OBD2_PID_TYPE_SINGLE_BYTE, .gain_offset = { { .gain = 100.0f/128.0f, .offset = -100.0f } } }, // OBD2_PID_01_LT_O2_TRIM_B1_S1_S3_S5 (0x55)
  { .type = OBD2_PID_TYPE_SINGLE_BYTE, .gain_offset = { { .gain = 100.0f/128.0f, .offset = -100.0f } } }, // OBD2_PID_01_ST_O2_TRIM_B2_S1_S3_S5 (0x56)
  { .type = OBD2_PID_TYPE_SINGLE_BYTE, .gain_offset = { { .gain = 100.0f/128.0f, .offset = -100.0f } } }, // OBD2_PID_01_LT_O2_TRIM_B2_S1_S3_S5 (0x57)
  { .type = OBD2_PID_TYPE_SINGLE_BYTE, .gain_offset = { { .gain = 100.0f/128.0f, .offset = -100.0f } } }, // OBD2_PID_01_ST_O2_TRIM_B1_S2_S4_S6 (0x58)
  { .type = OBD2_PID_TYPE_SINGLE_BYTE, .gain_offset = { { .gain = 100.0f/128.0f, .offset = -100.0f } } }, // OBD2_PID_01_LT_O2_TRIM_B1_S2_S4_S6 (0x59)
  { .type = OBD2_PID_TYPE_SINGLE_BYTE, .gain_offset = { { .gain = 100.0f/128.0f, .offset = -100.0f } } }, // OBD2_PID_01_ST_O2_TRIM_B2_S2_S4_S6 (0x5A)
  { .type = OBD2_PID_TYPE_SINGLE_BYTE, .gain_offset = { { .gain = 100.0f/128.0f, .offset = -100.0f } } }, // OBD2_PID_01_LT_O2_TRIM_B2_S2_S4_S6 (0x5B)
  { .type = OBD2_PID_TYPE_SINGLE_WORD, .gain_offset = { { .gain = 10.0f, .offset = 0.0f } } }, // OBD2_PID_01_FUEL_RAIL_ABS_PRESSURE (0x5C)
  { .type = OBD2_PID_TYPE_SINGLE_BYTE, .gain_offset = { { .gain = 100.0f/255.0f, .offset = 0.0f } } }, // OBD2_PID_01_RELATIVE_ACCELERATOR_POS (0x5D)
  { .type = OBD2_PID_TYPE_SINGLE_WORD, .gain_offset = { { .gain = 0.05f, .offset = 0.0f } } }, // OBD2_PID_01_ENGINE_FUEL_RATE (0x5E)
  { .type = OBD2_PID_TYPE_RAW_SINGLE_BYTE }, // OBD2_PID_01_EMISSION_REQUIREMENTS_DESIGN (0x5F)

  { .type = OBD2_PID_TYPE_RAW_QUAD_BYTES }, // OBD2_PID_01_SUPPORTED_61_80 (0x60)
  { .type = OBD2_PID_TYPE_SINGLE_BYTE, .gain_offset = { { .gain = 1.0f, .offset = -125.0f } } }, // OBD2_PID_01_DRIVERS_DEMAND_TORQUE_PERCENT (0x61)
  { .type = OBD2_PID_TYPE_SINGLE_BYTE, .gain_offset = { { .gain = 1.0f, .offset = -125.0f } } }, // OBD2_PID_01_ACTUAL_ENGINE_TORQUE_PERCENT (0x62)
  { .type = OBD2_PID_TYPE_SINGLE_WORD, .gain_offset = { { .gain = 1.0f, .offset = 0.0f } } }, // OBD2_PID_01_ENGINE_REFERENCE_TORQUE (0x63)
  { .type = OBD2_PID_TYPE_UNDEFINED }, // OBD2_PID_01_ENGINE_PERCENT_TORQUE_DATA (0x64)
  { .type = OBD2_PID_TYPE_RAW_DUAL_BYTES }, // OBD2_PID_01_AUX_IO_SUPPORTED (0x65)
  { .type = OBD2_PID_TYPE_PREFIX_BYTE_PLUS_DUAL_WORDS,
    .gain_offset = {
      { .gain = 1.0f/32.0f, .offset = 0.0f }, // MAF A (g/s) = (256*B + C)/32
      { .gain = 1.0f/32.0f, .offset = 0.0f }, // MAF B (g/s) = (256*D + E)/32
    } }, // OBD2_PID_01_MAF_SENSOR_A_B (0x66)
  { .type = OBD2_PID_TYPE_PREFIX_BYTE_PLUS_DUAL_BYTES,
    .gain_offset = {
      { .gain = 1.0f, .offset = -40.0f }, // Coolant T1 = B - 40
      { .gain = 1.0f, .offset = -40.0f }, // Coolant T2 = C - 40
    } }, // OBD2_PID_01_COOLANT_TEMP_SENSORS_1_2 (0x67)
  { .type = OBD2_PID_TYPE_PREFIX_BYTE_PLUS_SEXTUPLE_BYTES,
    .gain_offset = {
      { .gain = 1.0f, .offset = -40.0f }, // IAT1 = B - 40
      { .gain = 1.0f, .offset = -40.0f }, // IAT2 = C - 40
      { .gain = 1.0f, .offset = -40.0f }, // IAT2 = D - 40
      { .gain = 1.0f, .offset = -40.0f }, // IAT2 = E - 40
      { .gain = 1.0f, .offset = -40.0f }, // IAT2 = F - 40
      { .gain = 1.0f, .offset = -40.0f }, // IAT2 = G - 40
    } }, // OBD2_PID_01_IAT_SENSORS_1_2 (0x68)
  { .type = OBD2_PID_TYPE_UNDEFINED }, // OBD2_PID_01_EGR_ACT_CMD_ERR (0x69)
  { .type = OBD2_PID_TYPE_UNDEFINED }, // OBD2_PID_01_DIESEL_INTAKE_AIR_FLOW_CTRL (0x6A)
  { .type = OBD2_PID_TYPE_UNDEFINED }, // OBD2_PID_01_EGR_TEMPERATURE (0x6B)
  { .type = OBD2_PID_TYPE_UNDEFINED }, // OBD2_PID_01_THROTTLE_ACT_CTRL_REL_POS (0x6C)
  { .type = OBD2_PID_TYPE_UNDEFINED }, // OBD2_PID_01_FUEL_PRESSURE_CONTROL_SYS (0x6D)
  { .type = OBD2_PID_TYPE_UNDEFINED }, // OBD2_PID_01_INJECTION_PRESSURE_CONTROL_SYS (0x6E)
  { .type = OBD2_PID_TYPE_UNDEFINED }, // OBD2_PID_01_TURBO_COMPRESSOR_INLET_PRESS (0x6F)
  { .type = OBD2_PID_TYPE_UNDEFINED }, // OBD2_PID_01_BOOST_PRESSURE_CONTROL (0x70)
  { .type = OBD2_PID_TYPE_UNDEFINED }, // OBD2_PID_01_VGT_CONTROL (0x71)
  { .type = OBD2_PID_TYPE_UNDEFINED }, // OBD2_PID_01_WASTEGATE_CONTROL (0x72)
  { .type = OBD2_PID_TYPE_UNDEFINED }, // OBD2_PID_01_EXHAUST_PRESSURE (0x73)
  { .type = OBD2_PID_TYPE_UNDEFINED }, // OBD2_PID_01_TURBOCHARGER_RPM (0x74)
  { .type = OBD2_PID_TYPE_UNDEFINED }, // OBD2_PID_01_TURBOCHARGER_TEMPERATURE_1 (0x75)
  { .type = OBD2_PID_TYPE_UNDEFINED }, // OBD2_PID_01_TURBOCHARGER_TEMPERATURE_2 (0x76)
  { .type = OBD2_PID_TYPE_UNDEFINED }, // OBD2_PID_01_CACT (0x77)
  { .type = OBD2_PID_TYPE_PREFIX_BYTE_PLUS_QUAD_WORDS,
    .gain_offset = {
      { .gain = 0.1f, .offset = -40.0f },
      { .gain = 0.1f, .offset = -40.0f },
      { .gain = 0.1f, .offset = -40.0f },
      { .gain = 0.1f, .offset = -40.0f },
    } }, // OBD2_PID_01_EXHAUST_GAS_TEMP_B1 (0x78)
  { .type = OBD2_PID_TYPE_PREFIX_BYTE_PLUS_QUAD_WORDS,
    .gain_offset = {
      { .gain = 0.1f, .offset = -40.0f },
      { .gain = 0.1f, .offset = -40.0f },
      { .gain = 0.1f, .offset = -40.0f },
      { .gain = 0.1f, .offset = -40.0f },
    } }, // OBD2_PID_01_EXHAUST_GAS_TEMP_B2 (0x79)
  { .type = OBD2_PID_TYPE_UNDEFINED }, // OBD2_PID_01_DPF_DIFFERENTIAL_PRESSURE (0x7A)
  { .type = OBD2_PID_TYPE_UNDEFINED }, // OBD2_PID_01_DPF (0x7B)
  { .type = OBD2_PID_TYPE_PREFIX_BYTE_PLUS_QUAD_WORDS,
    .gain_offset = {
      { .gain = 0.1f, .offset = -40.0f },
      { .gain = 0.1f, .offset = -40.0f },
      { .gain = 0.1f, .offset = -40.0f },
      { .gain = 0.1f, .offset = -40.0f },
    } }, // OBD2_PID_01_DPF_TEMPERATURE (0x7C)
  { .type = OBD2_PID_TYPE_RAW_SINGLE_BYTE }, // OBD2_PID_01_NOX_NTE_STATUS (0x7D)
  { .type = OBD2_PID_TYPE_RAW_SINGLE_BYTE }, // OBD2_PID_01_PM_NTE_STATUS (0x7E)
  { .type = OBD2_PID_TYPE_UNDEFINED }, // OBD2_PID_01_ENGINE_RUNTIME_EXT (0x7F)
};


error_t obd2_init(obd2_ctx_t *ctx, const obd2_init_ctx_t *init)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(init == NULL, err = E_PARAM);

    memset(ctx, 0u, sizeof(obd2_ctx_t));
    memcpy(&ctx->init, init, sizeof(obd2_init_ctx_t));

    ctx->mode1_setup = obd2_mode1_setup;

    ctx->mode1_data[OBD2_PID_01_SUPPORTED_01_20].supported = true;

    ctx->mode1_data[OBD2_PID_01_MONITOR_STATUS_SINCE_DTC_CLEAR].supported = true;

    ctx->mode1_data[OBD2_PID_01_CALCULATED_ENGINE_LOAD].supported = true;
    ctx->mode1_data[OBD2_PID_01_CALCULATED_ENGINE_LOAD].value[0].flt = 25.0f;

    ctx->mode1_data[OBD2_PID_01_COOLANT_TEMPERATURE].supported = true;
    ctx->mode1_data[OBD2_PID_01_COOLANT_TEMPERATURE].value[0].flt = 89.0f;

    ctx->mode1_data[OBD2_PID_01_ENGINE_RPM].supported = true;
    ctx->mode1_data[OBD2_PID_01_ENGINE_RPM].value[0].flt = 860.0f;

    ctx->mode1_data[OBD2_PID_01_VEHICLE_SPEED].supported = true;
    ctx->mode1_data[OBD2_PID_01_VEHICLE_SPEED].value[0].flt = 35.4f;

    ctx->mode1_data[OBD2_PID_01_INTAKE_AIR_TEMPERATURE].supported = true;
    ctx->mode1_data[OBD2_PID_01_INTAKE_AIR_TEMPERATURE].value[0].flt = 28.0f;

    ctx->mode1_data[OBD2_PID_01_MAF_AIR_FLOW_RATE].supported = true;
    ctx->mode1_data[OBD2_PID_01_MAF_AIR_FLOW_RATE].value[0].flt = 24.0f;

    ctx->mode1_data[OBD2_PID_01_THROTTLE_POSITION].supported = true;
    ctx->mode1_data[OBD2_PID_01_THROTTLE_POSITION].value[0].flt = 20.0f;

    ctx->mode1_data[OBD2_PID_01_FUEL_SYSTEM_STATUS].supported = true;

    ctx->mode1_data[OBD2_PID_01_FUEL_PRESSURE].supported = true;
    ctx->mode1_data[OBD2_PID_01_FUEL_PRESSURE].value[0].flt = 300.0f;

    ctx->mode1_data[OBD2_PID_01_INTAKE_MANIFOLD_PRESSURE].supported = true;
    ctx->mode1_data[OBD2_PID_01_INTAKE_MANIFOLD_PRESSURE].value[0].flt = 35.0f;

    ctx->mode1_data[OBD2_PID_01_TIMING_ADVANCE].supported = true;
    ctx->mode1_data[OBD2_PID_01_TIMING_ADVANCE].value[0].flt = 10.0f;

    ctx->mode1_data[OBD2_PID_01_RUNTIME_SINCE_ENGINE_START].supported = true;

    ctx->mode1_data[OBD2_PID_01_SUPPORTED_21_40].supported = true;
    ctx->mode1_data[OBD2_PID_01_FUEL_LEVEL_INPUT].supported = true;

    ctx->mode1_data[OBD2_PID_01_BAROMETRIC_PRESSURE].supported = true;
    ctx->mode1_data[OBD2_PID_01_BAROMETRIC_PRESSURE].value[0].flt = 101.5f;

    ctx->mode1_data[OBD2_PID_01_SUPPORTED_41_60].supported = true;
    ctx->mode1_data[OBD2_PID_01_AMBIENT_AIR_TEMPERATURE].supported = true;
    ctx->mode1_data[OBD2_PID_01_AMBIENT_AIR_TEMPERATURE].value[0].flt = 23.0f;

    ctx->mode1_data[OBD2_PID_01_ENGINE_FUEL_RATE].supported = true;

    ctx->mode1_data[OBD2_PID_01_SUPPORTED_61_80].supported = true;

    ctx->mode1_data[OBD2_PID_01_EXHAUST_GAS_TEMP_B1].supported = true;
    ctx->mode1_data[OBD2_PID_01_EXHAUST_GAS_TEMP_B1].prefix_byte = 0x01;
    ctx->mode1_data[OBD2_PID_01_EXHAUST_GAS_TEMP_B1].value[0].flt = 450.0f;

    ctx->mode1_data[OBD2_PID_01_EXHAUST_GAS_TEMP_B2].supported = true;
    ctx->mode1_data[OBD2_PID_01_EXHAUST_GAS_TEMP_B2].prefix_byte = 0x01;
    ctx->mode1_data[OBD2_PID_01_EXHAUST_GAS_TEMP_B2].value[0].flt = 820.0f;

    ctx->mode1_data[OBD2_PID_01_MAF_SENSOR_A_B].supported = true;
    ctx->mode1_data[OBD2_PID_01_MAF_SENSOR_A_B].prefix_byte = 0x03;
    ctx->mode1_data[OBD2_PID_01_MAF_SENSOR_A_B].value[0].flt = 505.0f;
    ctx->mode1_data[OBD2_PID_01_MAF_SENSOR_A_B].value[1].flt = 500.0f;

    ctx->mode1_data[OBD2_PID_01_COOLANT_TEMP_SENSORS_1_2].supported = true;
    ctx->mode1_data[OBD2_PID_01_COOLANT_TEMP_SENSORS_1_2].prefix_byte = 0x03;
    ctx->mode1_data[OBD2_PID_01_COOLANT_TEMP_SENSORS_1_2].value[0].flt = 92.0f;
    ctx->mode1_data[OBD2_PID_01_COOLANT_TEMP_SENSORS_1_2].value[1].flt = 94.0f;

    ctx->mode1_data[OBD2_PID_01_IAT_SENSORS_1_2].supported = true;
    ctx->mode1_data[OBD2_PID_01_IAT_SENSORS_1_2].prefix_byte = 0x03F;
    ctx->mode1_data[OBD2_PID_01_IAT_SENSORS_1_2].value[0].flt = 52.0f;
    ctx->mode1_data[OBD2_PID_01_IAT_SENSORS_1_2].value[1].flt = 58.0f;
    ctx->mode1_data[OBD2_PID_01_IAT_SENSORS_1_2].value[2].flt = 60.0f;
    ctx->mode1_data[OBD2_PID_01_IAT_SENSORS_1_2].value[3].flt = 62.0f;
    ctx->mode1_data[OBD2_PID_01_IAT_SENSORS_1_2].value[4].flt = 64.0f;
    ctx->mode1_data[OBD2_PID_01_IAT_SENSORS_1_2].value[5].flt = 66.0f;
    ctx->mode1_data[OBD2_PID_01_IAT_SENSORS_1_2].value[6].flt = 68.0f;

    ctx->mode1_data[OBD2_PID_01_O2_SENSORS_PRESENT_4BANKS].supported = true;
    ctx->mode1_data[OBD2_PID_01_O2_SENSORS_PRESENT_4BANKS].value[0].raw = 0x0011;
    ctx->mode1_data[OBD2_PID_01_O2_SENSORS_PRESENT_2BANKS].supported = true;
    ctx->mode1_data[OBD2_PID_01_O2_SENSORS_PRESENT_2BANKS].value[0].raw = 0x11;

    ctx->mode1_data[OBD2_PID_01_O2_S1_EQUIV_RATIO_AND_VOLT].supported = true;
    ctx->mode1_data[OBD2_PID_01_O2_S5_EQUIV_RATIO_AND_VOLT].supported = true;

    uint8_t pid;

    ctx->mode9_data[OBD2_PID_09_SUPPORTED_01_20].supported = true;

    pid = OBD2_PID_09_VIN;
    ctx->mode9_data[pid].supported = true;
    ctx->mode9_data[pid].count = 1;
    ctx->mode9_data[pid].value[0] = (const uint8_t *)"VIN0123456789012";
    ctx->mode9_data[pid].length[0] = strlen((const char *)ctx->mode9_data[pid].value[0]);

    pid = OBD2_PID_09_CALID;
    ctx->mode9_data[pid].supported = true;
    ctx->mode9_data[pid - 1].supported = true;
    ctx->mode9_data[pid].count = 2;
    ctx->mode9_data[pid].value[0] = (const uint8_t *)"v1.23.45.678";
    ctx->mode9_data[pid].length[0] = strlen((const char *)ctx->mode9_data[pid].value[0]);
    ctx->mode9_data[pid].value[1] = (const uint8_t *)"v7.89.01.234";
    ctx->mode9_data[pid].length[1] = strlen((const char *)ctx->mode9_data[pid].value[1]);

    pid = OBD2_PID_09_ECU_NAME;
    ctx->mode9_data[pid].supported = true;
    ctx->mode9_data[pid].count = 1;
    ctx->mode9_data[pid].value[0] = (const uint8_t *)"AutoECUv2";
    ctx->mode9_data[pid].length[0] = strlen((const char *)ctx->mode9_data[pid].value[0]);

    pid = OBD2_PID_09_HARDWARE_NUMBER;
    ctx->mode9_data[pid].supported = true;
    ctx->mode9_data[pid].count = 1;
    ctx->mode9_data[pid].value[0] = (const uint8_t *)"HW v1.0.0";
    ctx->mode9_data[pid].length[0] = strlen((const char *)ctx->mode9_data[pid].value[0]);

    pid = OBD2_PID_09_VIN_DUPLICATE_FRAME;
    ctx->mode9_data[pid].supported = true;
    ctx->mode9_data[pid].count = 1;
    ctx->mode9_data[pid].value[0] = (const uint8_t *)"VD10123456789012";
    ctx->mode9_data[pid].length[0] = strlen((const char *)ctx->mode9_data[pid].value[0]);



    ctx->initialized = true;

  } while(0);

  return err;
}

error_t obd2_configure(obd2_ctx_t *ctx, const obd2_config_t *config)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->initialized == false, err = E_INVALACT);
    BREAK_IF_ACTION(config == NULL, err = E_PARAM);

    memcpy(&ctx->config, config, sizeof(obd2_config_t));

    ctx->configured = true;

  } while(0);

  return err;
}

error_t obd2_data_get_error(obd2_ctx_t *ctx, obd2_error_code_t *code)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->configured == false, err = E_INVALACT);
    BREAK_IF_ACTION(code == NULL, err = E_PARAM);

    *code = ctx->error_code;

  } while(0);

  return err;
}

error_t obd2_reset(obd2_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->configured == false, err = E_INVALACT);

    ctx->reset_trigger = true;

  } while(0);

  return err;
}

void obd2_loop(obd2_ctx_t *ctx)
{
  do {
    BREAK_IF(ctx == NULL);
    BREAK_IF(ctx->configured == false);

    obd2_loop_handler(ctx);

  } while(0);
}

error_t obd2_message_write_upstream(obd2_ctx_t *ctx, const uint8_t *payload, uint16_t length)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(payload == NULL || length == 0, err = E_PARAM);
    BREAK_IF_ACTION(ctx->configured == false, err = E_INVALACT);
    BREAK_IF_ACTION(length > OBD2_DATA_LENGTH_MAX, err = E_OVERFLOW);
    BREAK_IF_ACTION(ctx->upstream_available == true, err = E_AGAIN);

    memcpy(ctx->upstream_data, payload, length);
    ctx->upstream_data_len = length;
    ctx->upstream_available = true;

  } while(0);

  return err;
}

error_t obd2_message_read_downstream(obd2_ctx_t *ctx, uint8_t *payload, uint16_t *length)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(payload == NULL || length == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->configured == false, err = E_INVALACT);
    BREAK_IF_ACTION(ctx->downstream_available == false, err = E_AGAIN);

    *length = ctx->downstream_data_len;
    memcpy(payload, ctx->downstream_data, ctx->downstream_data_len);
    ctx->downstream_available = false;

  } while(0);

  return err;
}
