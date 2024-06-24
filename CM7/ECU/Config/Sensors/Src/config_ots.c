/*
 * config_ots.c
 *
 *  Created on: Jun 24, 2024
 *      Author: VHEMaster
 */

#include <string.h>
#include "config_ots.h"
#include "config_extern.h"
#include "compiler.h"

typedef struct {
    ots_config_t config_default;
    ots_init_ctx_t init;
    ots_ctx_t *ctx;
}ecu_sensors_ots_ctx_t;

static const ots_config_t ecu_sensors_ots_config_default = {
    .signal_resistance_to_value = {
        .table = {
            .items = 24,
            .input = {
                80, 102, 133, 177,
                241, 332, 467, 667,
                973, 1188, 1459, 2238,
                2796, 3520, 4450, 5670,
                7280, 9420, 12300, 16180,
                21450, 28680, 52700, 100700
            },
            .output = {
                128, 120, 110, 100,
                90, 80, 70, 60,
                50, 45, 40, 30,
                25, 20, 15, 10,
                5, 0, -5, -10,
                -15, -20, -30, -40
            },
        },
        .input_low = 60,
        .input_high = 130000,
    },
    .signal_resistance_calculated = {
        .calibration_resistance = 2796,
        .calibration_temperature = 25,
        .thermistor_beta = 4100,
    },
    .slew_rate = 10.0f,
    .boot_time = 100 * TIME_US_IN_MS,
    .signal_mode = OTS_SIGNAL_MODE_RESISTANCE,
    .calc_mode = OTS_CALC_MODE_TABLE_REF_VALUE,
};

static const bool ecu_sensors_ots_enabled_default[ECU_SENSOR_OTS_MAX] = {
    false,
};

static const ecu_gpio_input_pin_t ecu_sensors_ots_input_pin_default[ECU_SENSOR_OTS_MAX] = {
    ECU_IN_PORT2_PIN3,
};

static ecu_sensors_ots_ctx_t ecu_sensors_ots_ctx[ECU_SENSOR_OTS_MAX] = {
    {
      .init = {

      },
      .config_default = ecu_sensors_ots_config_default,
    },
    {
      .init = {

      },
      .config_default = ecu_sensors_ots_config_default,
    },
};

error_t ecu_sensors_ots_init(ecu_sensor_ots_t instance, ots_ctx_t *ctx)
{
  error_t err = E_OK;
  ecu_sensors_ots_ctx_t *ots_ctx;

  do {
    BREAK_IF_ACTION(instance <= ECU_SENSOR_NONE || instance >= ECU_SENSOR_OTS_MAX || ctx == NULL, err = E_PARAM);

    ots_ctx = &ecu_sensors_ots_ctx[instance];
    ots_ctx->ctx = ctx;

    ots_ctx->config_default.enabled = ecu_sensors_ots_enabled_default[instance];
    ots_ctx->config_default.input_pin = ecu_sensors_ots_input_pin_default[instance];

    err = ots_init(ots_ctx->ctx, &ots_ctx->init);
    BREAK_IF(err != E_OK);

    memcpy(&ots_ctx->ctx->config, &ots_ctx->config_default, sizeof(ots_config_t));

  } while(0);

  return err;
}

error_t ecu_sensors_ots_get_default_config(ecu_sensor_ots_t instance, ots_config_t *config)
{
  error_t err = E_OK;
  ecu_sensors_ots_ctx_t *ots_ctx;

  do {
    BREAK_IF_ACTION(instance <= ECU_SENSOR_NONE || instance >= ECU_SENSOR_OTS_MAX || config == NULL, err = E_PARAM);

    ots_ctx = &ecu_sensors_ots_ctx[instance];

    memcpy(config, &ots_ctx->config_default, sizeof(ots_config_t));

  } while(0);

  return err;
}

error_t ecu_sensors_ots_configure(ecu_sensor_ots_t instance, const ots_config_t *config)
{
  error_t err = E_OK;
  ecu_sensors_ots_ctx_t *ots_ctx;

  do {
    BREAK_IF_ACTION(instance <= ECU_SENSOR_NONE || instance >= ECU_SENSOR_OTS_MAX || config == NULL, err = E_PARAM);

    ots_ctx = &ecu_sensors_ots_ctx[instance];

    err = ots_configure(ots_ctx->ctx, config);

  } while(0);

  return err;
}

error_t ecu_sensors_ots_reset(ecu_sensor_ots_t instance)
{
  error_t err = E_OK;
  ecu_sensors_ots_ctx_t *ots_ctx;

  do {
    BREAK_IF_ACTION(instance <= ECU_SENSOR_NONE || instance >= ECU_SENSOR_OTS_MAX, err = E_PARAM);

    ots_ctx = &ecu_sensors_ots_ctx[instance];

    err = ots_reset(ots_ctx->ctx);

  } while(0);

  return err;
}

error_t ecu_sensors_ots_get_value(ecu_sensor_ots_t instance, ots_data_t *data)
{
  error_t err = E_OK;
  ecu_sensors_ots_ctx_t *ots_ctx;

  do {
    BREAK_IF_ACTION(instance <= ECU_SENSOR_NONE || instance >= ECU_SENSOR_OTS_MAX, err = E_PARAM);
    BREAK_IF_ACTION(data == NULL, err = E_PARAM);

    ots_ctx = &ecu_sensors_ots_ctx[instance];

    err = ots_get_value(ots_ctx->ctx, data);

  } while(0);

  return err;
}

error_t ecu_sensors_ots_get_diag(ecu_sensor_ots_t instance, ots_diag_t *diag)
{
  error_t err = E_OK;
  ecu_sensors_ots_ctx_t *ots_ctx;

  do {
    BREAK_IF_ACTION(instance <= ECU_SENSOR_NONE || instance >= ECU_SENSOR_OTS_MAX, err = E_PARAM);
    BREAK_IF_ACTION(diag == NULL, err = E_PARAM);

    ots_ctx = &ecu_sensors_ots_ctx[instance];

    err = ots_get_diag(ots_ctx->ctx, diag);

  } while(0);

  return err;
}
