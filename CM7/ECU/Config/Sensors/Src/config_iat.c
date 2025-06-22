/*
 * config_iat.c
 *
 *  Created on: Apr 17, 2024
 *      Author: VHEMaster
 */

#include <string.h>
#include "config_iat.h"
#include "config_extern.h"
#include "compiler.h"

typedef struct {
    iat_config_t config_default;
    iat_init_ctx_t init;
    iat_ctx_t *ctx;
}ecu_sensors_iat_ctx_t;

/* Parameters for sensor Valeo 96646125 :
    .signal_resistance_to_value = {
        .table = {
            .items = 28,
            .input = {
                317, 407, 529, 698,
                934, 1270, 1760, 2490,
                2980, 3590, 4350, 5300,
                6510, 8040, 10000, 12500,
                15800, 20200, 25900, 33600,
                44000, 58200, 77900, 105000,
                144000, 200000, 282000, 402000
            },
            .output = {
                130, 120, 110, 100,
                90, 80, 70, 60,
                55, 50, 45, 40,
                35, 30, 25, 20,
                15, 10, 5, 0,
                -5, -10, -15, -20,
                -25, -30, -35, -40
            },
        },
        .input_low = 250,
        .input_high = 450000,
    },
    .signal_resistance_calculated = {
        .calibration_resistance = 10000,
        .calibration_temperature = 25,
        .thermistor_beta = 3950,
    },
 */

static const iat_config_t ecu_sensors_iat_config_default = {
    .signal_resistance_to_value = {
        .table = {
            .items = 28,
            .input = {
                89.30f, 112.7f, 144.2f, 186.6f,
                243.2f, 322.5f, 435.7f, 595.5f,
                702.8f, 833.9f, 987.6f, 1175,
                1412, 1707, 2057, 2500,
                3069, 3792, 4712, 5896,
                7415, 9397, 12002, 15462,
                20003, 26114, 34281, 45313
            },
            .output = {
                130, 120, 110, 100,
                90, 80, 70, 60,
                55, 50, 45, 40,
                35, 30, 25, 20,
                15, 10, 5, 0,
                -5, -10, -15, -20,
                -25, -30, -35, -40
            },
        },
        .input_low = 60,
        .input_high = 90000,
    },
    .signal_resistance_calculated = {
        .calibration_resistance = 2057,
        .calibration_temperature = 25,
        .thermistor_beta = 3500,
    },
    .slew_rate = 20.0f,
    .boot_time = 100 * TIME_US_IN_MS,
    .signal_mode = IAT_SIGNAL_MODE_RESISTANCE,
    .calc_mode = IAT_CALC_MODE_TABLE_REF_VALUE,
};

static const bool ecu_sensors_iat_enabled_default[ECU_SENSOR_IAT_MAX] = {
    true,
    true,
};

static const ecu_gpio_input_pin_t ecu_sensors_iat_input_pin_default[ECU_SENSOR_IAT_MAX] = {
    ECU_IN_PORT2_PIN12,
    ECU_IN_PORT1_PIN8
};

static RAM_SECTION ecu_sensors_iat_ctx_t ecu_sensors_iat_ctx[ECU_SENSOR_IAT_MAX] = {
    {
      .init = {

      },
      .config_default = ecu_sensors_iat_config_default,
    },
    {
      .init = {

      },
      .config_default = ecu_sensors_iat_config_default,
    },
};

error_t ecu_sensors_iat_init(ecu_sensor_iat_t instance, iat_ctx_t *ctx)
{
  error_t err = E_OK;
  ecu_sensors_iat_ctx_t *iat_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_SENSOR_IAT_MAX || ctx == NULL, err = E_PARAM);

    iat_ctx = &ecu_sensors_iat_ctx[instance];
    iat_ctx->ctx = ctx;

    iat_ctx->config_default.enabled = ecu_sensors_iat_enabled_default[instance];
    iat_ctx->config_default.input_pin = ecu_sensors_iat_input_pin_default[instance];

    err = iat_init(iat_ctx->ctx, &iat_ctx->init);
    BREAK_IF(err != E_OK);

    memcpy(&iat_ctx->ctx->config, &iat_ctx->config_default, sizeof(iat_config_t));

  } while(0);

  return err;
}

error_t ecu_sensors_iat_get_default_config(ecu_sensor_iat_t instance, iat_config_t *config)
{
  error_t err = E_OK;
  ecu_sensors_iat_ctx_t *iat_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_SENSOR_IAT_MAX || config == NULL, err = E_PARAM);

    iat_ctx = &ecu_sensors_iat_ctx[instance];

    memcpy(config, &iat_ctx->config_default, sizeof(iat_config_t));

  } while(0);

  return err;
}

error_t ecu_sensors_iat_configure(ecu_sensor_iat_t instance, const iat_config_t *config)
{
  error_t err = E_OK;
  ecu_sensors_iat_ctx_t *iat_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_SENSOR_IAT_MAX || config == NULL, err = E_PARAM);

    iat_ctx = &ecu_sensors_iat_ctx[instance];

    err = iat_configure(iat_ctx->ctx, config);

  } while(0);

  return err;
}

error_t ecu_sensors_iat_reset(ecu_sensor_iat_t instance)
{
  error_t err = E_OK;
  ecu_sensors_iat_ctx_t *iat_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_SENSOR_IAT_MAX, err = E_PARAM);

    iat_ctx = &ecu_sensors_iat_ctx[instance];

    err = iat_reset(iat_ctx->ctx);

  } while(0);

  return err;
}

error_t ecu_sensors_iat_get_value(ecu_sensor_iat_t instance, iat_data_t *data)
{
  error_t err = E_OK;
  ecu_sensors_iat_ctx_t *iat_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_SENSOR_IAT_MAX, err = E_PARAM);
    BREAK_IF_ACTION(data == NULL, err = E_PARAM);

    iat_ctx = &ecu_sensors_iat_ctx[instance];

    err = iat_get_value(iat_ctx->ctx, data);

  } while(0);

  return err;
}

error_t ecu_sensors_iat_get_diag(ecu_sensor_iat_t instance, iat_diag_t *diag)
{
  error_t err = E_OK;
  ecu_sensors_iat_ctx_t *iat_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_SENSOR_IAT_MAX, err = E_PARAM);
    BREAK_IF_ACTION(diag == NULL, err = E_PARAM);

    iat_ctx = &ecu_sensors_iat_ctx[instance];

    err = iat_get_diag(iat_ctx->ctx, diag);

  } while(0);

  return err;
}
