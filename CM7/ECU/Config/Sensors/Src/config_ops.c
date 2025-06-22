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
            .items = 20,
            .input = {
                0.0f, 3.0f, 6.6f, 12.1f,
                16.4f, 18.7f, 23.1f, 29.8f,
                32.1f, 35.2f, 39.9f, 48.3f,
                56.6f, 65.0, 81.0f, 97.0f,
                115.0f, 132.0f, 150.0f, 160.0f
            },
            .output = {
                0.0f, 0.0f, 0.4f, 0.6f,
                0.8f, 1.0f, 1.4f, 1.8f,
                2.0f, 2.2f, 2.5f, 3.0f,
                3.5f, 4.0f, 5.0f, 6.0f,
                7.0f, 8.0f, 9.0f, 10.0f
            },
        },
        .input_low = 1.5f,
        .input_high = 200,
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

static RAM_SECTION ecu_sensors_ops_ctx_t ecu_sensors_ops_ctx[ECU_SENSOR_OPS_MAX] = {
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
    BREAK_IF_ACTION(instance >= ECU_SENSOR_OPS_MAX || ctx == NULL, err = E_PARAM);

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
    BREAK_IF_ACTION(instance >= ECU_SENSOR_OPS_MAX || config == NULL, err = E_PARAM);

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
    BREAK_IF_ACTION(instance >= ECU_SENSOR_OPS_MAX || config == NULL, err = E_PARAM);

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
    BREAK_IF_ACTION(instance >= ECU_SENSOR_OPS_MAX, err = E_PARAM);

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
    BREAK_IF_ACTION(instance >= ECU_SENSOR_OPS_MAX, err = E_PARAM);
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
    BREAK_IF_ACTION(instance >= ECU_SENSOR_OPS_MAX, err = E_PARAM);
    BREAK_IF_ACTION(diag == NULL, err = E_PARAM);

    ops_ctx = &ecu_sensors_ops_ctx[instance];

    err = ops_get_diag(ops_ctx->ctx, diag);

  } while(0);

  return err;
}
