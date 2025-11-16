/*
 * config_obd2.c
 *
 *  Created on: Oct 23, 2025
 *      Author: vhemaster
 */

#include <string.h>
#include "config_obd2.h"
#include "compiler.h"

typedef struct ecu_comm_obd2_ctx_tag ecu_comm_obd2_ctx_t;

static void ecu_comm_obd2_error_callback(obd2_ctx_t *ctx, obd2_error_code_t code, void *userdata);

typedef struct ecu_comm_obd2_ctx_tag {
    obd2_config_t config_default;
    obd2_init_ctx_t init;
    obd2_ctx_t *ctx;
}ecu_comm_obd2_ctx_t;

static const obd2_config_t ecu_comm_obd2_config_default = {
    .mode_01_linkage = {
        .supported = true,
        .parameters = {
            {
                .supported = true,
            }, // OBD2_PID_01_SUPPORTED_01_20
            {
                .supported = false,
            }, // OBD2_PID_01_MONITOR_STATUS_SINCE_DTC_CLEAR
            {
                .supported = false,
            }, // OBD2_PID_01_FREEZE_DTC
            {
                .supported = false,
            }, // OBD2_PID_01_FUEL_SYSTEM_STATUS
            {
                .supported = false,
            }, // OBD2_PID_01_CALCULATED_ENGINE_LOAD
            {
                .supported = false,
            }, // OBD2_PID_01_COOLANT_TEMPERATURE
            {
                .supported = false,
            }, // OBD2_PID_01_ST_FUEL_TRIM_B1
            {
                .supported = false,
            }, // OBD2_PID_01_LT_FUEL_TRIM_B1
            {
                .supported = false,
            }, // OBD2_PID_01_ST_FUEL_TRIM_B2
            {
                .supported = false,
            }, // OBD2_PID_01_LT_FUEL_TRIM_B2
            {
                .supported = false,
            }, // OBD2_PID_01_FUEL_PRESSURE
            {
                .supported = false,
            }, // OBD2_PID_01_INTAKE_MANIFOLD_PRESSURE
            {
                .supported = false,
            }, // OBD2_PID_01_ENGINE_RPM
            {
                .supported = false,
            }, // OBD2_PID_01_VEHICLE_SPEED
            {
                .supported = false,
            }, // OBD2_PID_01_TIMING_ADVANCE
            {
                .supported = false,
            }, // OBD2_PID_01_INTAKE_AIR_TEMPERATURE
            {
                .supported = false,
            }, // OBD2_PID_01_MAF_AIR_FLOW_RATE
            {
                .supported = false,
            }, // OBD2_PID_01_THROTTLE_POSITION
            {
                .supported = false,
            }, // OBD2_PID_01_COMMANDED_SECONDARY_AIR
            {
                .supported = false,
            }, // OBD2_PID_01_O2_SENSORS_PRESENT_2BANKS
            {
                .supported = false,
            }, // OBD2_PID_01_O2_SENSOR1_V_AND_STFT
            {
                .supported = false,
            }, // OBD2_PID_01_O2_SENSOR2_V_AND_STFT
            {
                .supported = false,
            }, // OBD2_PID_01_O2_SENSOR3_V_AND_STFT
            {
                .supported = false,
            }, // OBD2_PID_01_O2_SENSOR4_V_AND_STFT
            {
                .supported = false,
            }, // OBD2_PID_01_O2_SENSOR5_V_AND_STFT
            {
                .supported = false,
            }, // OBD2_PID_01_O2_SENSOR6_V_AND_STFT
            {
                .supported = false,
            }, // OBD2_PID_01_O2_SENSOR7_V_AND_STFT
            {
                .supported = false,
            }, // OBD2_PID_01_O2_SENSOR8_V_AND_STFT
            {
                .supported = false,
            }, // OBD2_PID_01_OBD2_STANDARDS_IN_USE
            {
                .supported = false,
            }, // OBD2_PID_01_O2_SENSORS_PRESENT_4BANKS
            {
                .supported = false,
            }, // OBD2_PID_01_AUX_INPUT_STATUS
            {
                .supported = false,
            }, // OBD2_PID_01_RUNTIME_SINCE_ENGINE_START

            {
                .supported = true,
            }, // OBD2_PID_01_SUPPORTED_21_40
            {
                .supported = false,
            }, // OBD2_PID_01_DISTANCE_WITH_MIL_ON
            {
                .supported = false,
            }, // OBD2_PID_01_FUEL_RAIL_PRESS_REL_TO_MANIF
            {
                .supported = false,
            }, // OBD2_PID_01_FUEL_RAIL_PRESSURE_DIRECT
            {
                .supported = false,
            }, // OBD2_PID_01_O2_S1_EQUIV_RATIO_AND_VOLT
            {
                .supported = false,
            }, // OBD2_PID_01_O2_S2_EQUIV_RATIO_AND_VOLT
            {
                .supported = false,
            }, // OBD2_PID_01_O2_S3_EQUIV_RATIO_AND_VOLT
            {
                .supported = false,
            }, // OBD2_PID_01_O2_S4_EQUIV_RATIO_AND_VOLT
            {
                .supported = false,
            }, // OBD2_PID_01_O2_S5_EQUIV_RATIO_AND_VOLT
            {
                .supported = false,
            }, // OBD2_PID_01_O2_S6_EQUIV_RATIO_AND_VOLT
            {
                .supported = false,
            }, // OBD2_PID_01_O2_S7_EQUIV_RATIO_AND_VOLT
            {
                .supported = false,
            }, // OBD2_PID_01_O2_S8_EQUIV_RATIO_AND_VOLT
            {
                .supported = false,
            }, // OBD2_PID_01_COMMANDED_EGR
            {
                .supported = false,
            }, // OBD2_PID_01_EGR_ERROR
            {
                .supported = false,
            }, // OBD2_PID_01_COMMANDED_EVAP_PURGE
            {
                .supported = false,
            }, // OBD2_PID_01_FUEL_LEVEL_INPUT
            {
                .supported = false,
            }, // OBD2_PID_01_WARMUPS_SINCE_DTC_CLEAR
            {
                .supported = false,
            }, // OBD2_PID_01_DISTANCE_SINCE_DTC_CLEAR
            {
                .supported = false,
            }, // OBD2_PID_01_EVAP_VAPOR_PRESSURE_ABS_LEGACY
            {
                .supported = false,
            }, // OBD2_PID_01_BAROMETRIC_PRESSURE
            {
                .supported = false,
            }, // OBD2_PID_01_O2_S1_CURRENT_AND_EQUIV
            {
                .supported = false,
            }, // OBD2_PID_01_O2_S2_CURRENT_AND_EQUIV
            {
                .supported = false,
            }, // OBD2_PID_01_O2_S3_CURRENT_AND_EQUIV
            {
                .supported = false,
            }, // OBD2_PID_01_O2_S4_CURRENT_AND_EQUIV
            {
                .supported = false,
            }, // OBD2_PID_01_O2_S5_CURRENT_AND_EQUIV
            {
                .supported = false,
            }, // OBD2_PID_01_O2_S6_CURRENT_AND_EQUIV
            {
                .supported = false,
            }, // OBD2_PID_01_O2_S7_CURRENT_AND_EQUIV
            {
                .supported = false,
            }, // OBD2_PID_01_O2_S8_CURRENT_AND_EQUIV
            {
                .supported = false,
            }, // OBD2_PID_01_CAT_TEMP_B1S1
            {
                .supported = false,
            }, // OBD2_PID_01_CAT_TEMP_B2S1
            {
                .supported = false,
            }, // OBD2_PID_01_CAT_TEMP_B1S2
            {
                .supported = false,
            }, // OBD2_PID_01_CAT_TEMP_B2S2

            {
                .supported = true,
            }, // OBD2_PID_01_SUPPORTED_41_60
            {
                .supported = false,
            }, // OBD2_PID_01_MONITOR_STATUS_THIS_DRIVE_CYCLE
            {
                .supported = false,
            }, // OBD2_PID_01_CONTROL_MODULE_VOLTAGE
            {
                .supported = false,
            }, // OBD2_PID_01_ABSOLUTE_LOAD_VALUE
            {
                .supported = false,
            }, // OBD2_PID_01_COMMANDED_EQUIV_RATIO
            {
                .supported = false,
            }, // OBD2_PID_01_RELATIVE_THROTTLE_POS
            {
                .supported = false,
            }, // OBD2_PID_01_AMBIENT_AIR_TEMPERATURE
            {
                .supported = false,
            }, // OBD2_PID_01_ABS_THROTTLE_POS_B
            {
                .supported = false,
            }, // OBD2_PID_01_ABS_THROTTLE_POS_C
            {
                .supported = false,
            }, // OBD2_PID_01_ACCEL_PEDAL_POS_D
            {
                .supported = false,
            }, // OBD2_PID_01_ACCEL_PEDAL_POS_E
            {
                .supported = false,
            }, // OBD2_PID_01_ACCEL_PEDAL_POS_F
            {
                .supported = false,
            }, // OBD2_PID_01_COMMANDED_THROTTLE_ACTUATOR
            {
                .supported = false,
            }, // OBD2_PID_01_TIME_WITH_MIL_ON
            {
                .supported = false,
            }, // OBD2_PID_01_TIME_SINCE_DTC_CLEAR
            {
                .supported = false,
            }, // OBD2_PID_01_MAX_MAF_SINCE_DTC_CLEAR
            {
                .supported = false,
            }, // OBD2_PID_01_FUEL_TYPE
            {
                .supported = false,
            }, // OBD2_PID_01_ETHANOL_FUEL_PERCENT
            {
                .supported = false,
            }, // OBD2_PID_01_EVAP_VAPOR_PRESSURE_ABSOLUTE
            {
                .supported = false,
            }, // OBD2_PID_01_EVAP_VAPOR_PRESSURE_ALT
            {
                .supported = false,
            }, // OBD2_PID_01_ST_O2_TRIM_B1_S1_S3_S5
            {
                .supported = false,
            }, // OBD2_PID_01_LT_O2_TRIM_B1_S1_S3_S5
            {
                .supported = false,
            }, // OBD2_PID_01_ST_O2_TRIM_B2_S1_S3_S5
            {
                .supported = false,
            }, // OBD2_PID_01_LT_O2_TRIM_B2_S1_S3_S5
            {
                .supported = false,
            }, // OBD2_PID_01_ST_O2_TRIM_B1_S2_S4_S6
            {
                .supported = false,
            }, // OBD2_PID_01_LT_O2_TRIM_B1_S2_S4_S6
            {
                .supported = false,
            }, // OBD2_PID_01_ST_O2_TRIM_B2_S2_S4_S6
            {
                .supported = false,
            }, // OBD2_PID_01_LT_O2_TRIM_B2_S2_S4_S6
            {
                .supported = false,
            }, // OBD2_PID_01_FUEL_RAIL_ABS_PRESSURE
            {
                .supported = false,
            }, // OBD2_PID_01_RELATIVE_ACCELERATOR_POS
            {
                .supported = false,
            }, // OBD2_PID_01_ENGINE_FUEL_RATE
            {
                .supported = false,
            }, // OBD2_PID_01_EMISSION_REQUIREMENTS_DESIGN

            {
                .supported = true,
            }, // OBD2_PID_01_SUPPORTED_61_80
            {
                .supported = false,
            }, // OBD2_PID_01_DRIVERS_DEMAND_TORQUE_PERCENT
            {
                .supported = false,
            }, // OBD2_PID_01_ACTUAL_ENGINE_TORQUE_PERCENT
            {
                .supported = false,
            }, // OBD2_PID_01_ENGINE_REFERENCE_TORQUE
            {
                .supported = false,
            }, // OBD2_PID_01_ENGINE_PERCENT_TORQUE_DATA
            {
                .supported = false,
            }, // OBD2_PID_01_AUX_IO_SUPPORTED
            {
                .supported = false,
            }, // OBD2_PID_01_MAF_SENSOR_A_B
            {
                .supported = false,
            }, // OBD2_PID_01_COOLANT_TEMP_SENSORS_1_2
            {
                .supported = false,
            }, // OBD2_PID_01_IAT_SENSORS_1_2
            {
                .supported = false,
            }, // OBD2_PID_01_EGR_ACT_CMD_ERR
            {
                .supported = false,
            }, // OBD2_PID_01_DIESEL_INTAKE_AIR_FLOW_CTRL
            {
                .supported = false,
            }, // OBD2_PID_01_EGR_TEMPERATURE
            {
                .supported = false,
            }, // OBD2_PID_01_THROTTLE_ACT_CTRL_REL_POS
            {
                .supported = false,
            }, // OBD2_PID_01_FUEL_PRESSURE_CONTROL_SYS
            {
                .supported = false,
            }, // OBD2_PID_01_INJECTION_PRESSURE_CONTROL_SYS
            {
                .supported = false,
            }, // OBD2_PID_01_TURBO_COMPRESSOR_INLET_PRESS
            {
                .supported = false,
            }, // OBD2_PID_01_BOOST_PRESSURE_CONTROL
            {
                .supported = false,
            }, // OBD2_PID_01_VGT_CONTROL
            {
                .supported = false,
            }, // OBD2_PID_01_WASTEGATE_CONTROL
            {
                .supported = false,
            }, // OBD2_PID_01_EXHAUST_PRESSURE
            {
                .supported = false,
            }, // OBD2_PID_01_TURBOCHARGER_RPM
            {
                .supported = false,
            }, // OBD2_PID_01_TURBOCHARGER_TEMPERATURE_1
            {
                .supported = false,
            }, // OBD2_PID_01_TURBOCHARGER_TEMPERATURE_2
            {
                .supported = false,
            }, // OBD2_PID_01_CACT
            {
                .supported = false,
            }, // OBD2_PID_01_EXHAUST_GAS_TEMP_B1
            {
                .supported = false,
            }, // OBD2_PID_01_EXHAUST_GAS_TEMP_B2
            {
                .supported = false,
            }, // OBD2_PID_01_DPF_DIFFERENTIAL_PRESSURE
            {
                .supported = false,
            }, // OBD2_PID_01_DPF
            {
                .supported = false,
            }, // OBD2_PID_01_DPF_TEMPERATURE
            {
                .supported = false,
            }, // OBD2_PID_01_NOX_NTE_STATUS
            {
                .supported = false,
            }, // OBD2_PID_01_PM_NTE_STATUS
            {
                .supported = false,
            }, // OBD2_PID_01_ENGINE_RUNTIME_EXT
        },
    },
};

static const bool ecu_comm_obd2_enabled_default[ECU_COMM_OBD2_MAX] = {
    true,
    true,
    true
};

static RAM_SECTION ecu_comm_obd2_ctx_t ecu_comm_obd2_ctx[ECU_COMM_OBD2_MAX] = {
    {
      .init = {
          .error_callback = ecu_comm_obd2_error_callback,
          .callback_userdata = NULL,
      },
      .config_default = ecu_comm_obd2_config_default,
    },
    {
      .init = {
          .error_callback = ecu_comm_obd2_error_callback,
          .callback_userdata = NULL,
      },
      .config_default = ecu_comm_obd2_config_default,
    },
    {
      .init = {
          .error_callback = ecu_comm_obd2_error_callback,
          .callback_userdata = NULL,
      },
      .config_default = ecu_comm_obd2_config_default,
    },
};

error_t ecu_comm_obd2_init(ecu_comm_obd2_t instance, obd2_ctx_t *ctx)
{
  error_t err = E_OK;
  ecu_comm_obd2_ctx_t *obd2_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_COMM_OBD2_MAX || ctx == NULL, err = E_PARAM);

    obd2_ctx = &ecu_comm_obd2_ctx[instance];
    obd2_ctx->ctx = ctx;
    obd2_ctx->config_default.enabled = ecu_comm_obd2_enabled_default[instance];

    obd2_ctx->init.callback_userdata = (void *)obd2_ctx;

    err = obd2_init(obd2_ctx->ctx, &obd2_ctx->init);
    BREAK_IF(err != E_OK);

    err = ecu_comm_set_comm_enabled(ECU_COMM_TYPE_OBD2, instance, false);
    BREAK_IF(err != E_OK);

  } while(0);

  return err;
}

error_t ecu_comm_obd2_get_default_config(ecu_comm_obd2_t instance, obd2_config_t *config)
{
  error_t err = E_OK;
  ecu_comm_obd2_ctx_t *obd2_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_COMM_OBD2_MAX || config == NULL, err = E_PARAM);

    obd2_ctx = &ecu_comm_obd2_ctx[instance];

    memcpy(config, &obd2_ctx->config_default, sizeof(obd2_config_t));

  } while(0);

  return err;
}

error_t ecu_comm_obd2_configure(ecu_comm_obd2_t instance, const obd2_config_t *config)
{
  error_t err = E_OK;
  ecu_comm_obd2_ctx_t *obd2_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_COMM_OBD2_MAX || config == NULL, err = E_PARAM);

    obd2_ctx = &ecu_comm_obd2_ctx[instance];

    err = obd2_configure(obd2_ctx->ctx, config);
    BREAK_IF(err != E_OK);

    err = ecu_comm_set_comm_enabled(ECU_COMM_TYPE_OBD2, instance, obd2_ctx->ctx->config.enabled);
    BREAK_IF(err != E_OK);

  } while(0);

  return err;
}

error_t ecu_comm_obd2_reset(ecu_comm_obd2_t instance)
{
  error_t err = E_OK;
  ecu_comm_obd2_ctx_t *obd2_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_COMM_OBD2_MAX, err = E_PARAM);

    obd2_ctx = &ecu_comm_obd2_ctx[instance];

    err = obd2_reset(obd2_ctx->ctx);

  } while(0);

  return err;
}

static void ecu_comm_obd2_error_callback(obd2_ctx_t *ctx, obd2_error_code_t code, void *userdata)
{
  // TODO: IMPLEMENT
}
