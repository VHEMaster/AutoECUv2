/*
 * config_ops.c
 *
 *  Created on: Apr 17, 2024
 *      Author: VHEMaster
 */

#include <string.h>
#include "config_ops.h"
#include "config_extern.h"
#include "compiler.h"

typedef struct {
    ops_config_t config_default;
    ops_init_ctx_t init;
    ops_ctx_t *ctx;
}ecu_sensors_ops_ctx_t;

static const ops_config_t ecu_sensors_ops_config_default = {
    .signal_resistance_to_value = {
        .table = {
            .items = 24,
            .input = {
                0, 3, 4, 5,
                6, 7, 8, 9,
                10, 11, 12, 13,
                14, 15, 16, 17,
            },
            .output = {
                0, 0, 50, 100,
                150, 200, 250, 300,
                350, 400, 450, 500,
                550, 600, 650, 700,
            },
        },
        .input_low = 2,
        .input_high = 25,
    },
    .slew_rate = 10.0f,
    .boot_time = 100 * TIME_US_IN_MS,
    .signal_mode = OPS_SIGNAL_MODE_RESISTANCE,
    .calc_mode = OPS_CALC_MODE_TABLE_REF_VALUE,
};

static const bool ecu_sensors_ops_enabled_default[ECU_SENSOR_OPS_MAX] = {
    false,
};

static const ecu_gpio_input_pin_t ecu_sensors_ops_input_pin_default[ECU_SENSOR_OPS_MAX] = {
    ECU_IN_PORT2_PIN3,
};

static ecu_sensors_ops_ctx_t ecu_sensors_ops_ctx[ECU_SENSOR_OPS_MAX] = {
    {
      .init = {

      },
      .config_default = ecu_sensors_ops_config_default,
    },
    {
      .init = {

      },
      .config_default = ecu_sensors_ops_config_default,
    },
};

error_t ecu_sensors_ops_init(ecu_sensor_ops_t instance, ops_ctx_t *ctx)
{
  error_t err = E_OK;
  ecu_sensors_ops_ctx_t *ops_ctx;

  do {
    BREAK_IF_ACTION(instance <= ECU_SENSOR_NONE || instance >= ECU_SENSOR_OPS_MAX || ctx == NULL, err = E_PARAM);

    ops_ctx = &ecu_sensors_ops_ctx[instance];
    ops_ctx->ctx = ctx;

    ops_ctx->config_default.enabled = ecu_sensors_ops_enabled_default[instance];
    ops_ctx->config_default.input_pin = ecu_sensors_ops_input_pin_default[instance];

    err = ops_init(ops_ctx->ctx, &ops_ctx->init);
    BREAK_IF(err != E_OK);

    memcpy(&ops_ctx->ctx->config, &ops_ctx->config_default, sizeof(ops_config_t));

  } while(0);

  return err;
}

error_t ecu_sensors_ops_get_default_config(ecu_sensor_ops_t instance, ops_config_t *config)
{
  error_t err = E_OK;
  ecu_sensors_ops_ctx_t *ops_ctx;

  do {
    BREAK_IF_ACTION(instance <= ECU_SENSOR_NONE || instance >= ECU_SENSOR_OPS_MAX || config == NULL, err = E_PARAM);

    ops_ctx = &ecu_sensors_ops_ctx[instance];

    memcpy(config, &ops_ctx->config_default, sizeof(ops_config_t));

  } while(0);

  return err;
}

error_t ecu_sensors_ops_configure(ecu_sensor_ops_t instance, const ops_config_t *config)
{
  error_t err = E_OK;
  ecu_sensors_ops_ctx_t *ops_ctx;

  do {
    BREAK_IF_ACTION(instance <= ECU_SENSOR_NONE || instance >= ECU_SENSOR_OPS_MAX || config == NULL, err = E_PARAM);

    ops_ctx = &ecu_sensors_ops_ctx[instance];

    err = ops_configure(ops_ctx->ctx, config);

  } while(0);

  return err;
}

error_t ecu_sensors_ops_reset(ecu_sensor_ops_t instance)
{
  error_t err = E_OK;
  ecu_sensors_ops_ctx_t *ops_ctx;

  do {
    BREAK_IF_ACTION(instance <= ECU_SENSOR_NONE || instance >= ECU_SENSOR_OPS_MAX, err = E_PARAM);

    ops_ctx = &ecu_sensors_ops_ctx[instance];

    err = ops_reset(ops_ctx->ctx);

  } while(0);

  return err;
}

error_t ecu_sensors_ops_get_value(ecu_sensor_ops_t instance, ops_data_t *data)
{
  error_t err = E_OK;
  ecu_sensors_ops_ctx_t *ops_ctx;

  do {
    BREAK_IF_ACTION(instance <= ECU_SENSOR_NONE || instance >= ECU_SENSOR_OPS_MAX, err = E_PARAM);
    BREAK_IF_ACTION(data == NULL, err = E_PARAM);

    ops_ctx = &ecu_sensors_ops_ctx[instance];

    err = ops_get_value(ops_ctx->ctx, data);

  } while(0);

  return err;
}

error_t ecu_sensors_ops_get_diag(ecu_sensor_ops_t instance, ops_diag_t *diag)
{
  error_t err = E_OK;
  ecu_sensors_ops_ctx_t *ops_ctx;

  do {
    BREAK_IF_ACTION(instance <= ECU_SENSOR_NONE || instance >= ECU_SENSOR_OPS_MAX, err = E_PARAM);
    BREAK_IF_ACTION(diag == NULL, err = E_PARAM);

    ops_ctx = &ecu_sensors_ops_ctx[instance];

    err = ops_get_diag(ops_ctx->ctx, diag);

  } while(0);

  return err;
}
