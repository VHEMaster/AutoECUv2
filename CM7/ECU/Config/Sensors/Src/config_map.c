/*
 * config_map.c
 *
 *  Created on: Apr 17, 2024
 *      Author: VHEMaster
 */

#include <string.h>
#include "config_map.h"
#include "config_extern.h"
#include "compiler.h"

typedef struct {
    map_config_t config_default;
    map_init_ctx_t init;
    map_ctx_t *ctx;
}ecu_sensors_map_ctx_t;

static const map_config_t ecu_sensors_map_config_default = {
    .signal_voltage_to_value = {
        .input_low = 0.05f,
        .input_high = 5.00f,
        .input_step = 5.0f / 256,
        .gain = 0.19f,
        .offset = 0.10f,
    },
    .boot_time = 100 * TIME_US_IN_MS,
    .signal_mode = MAP_SIGNAL_MODE_ANALOG,
    .calc_mode = MAP_CALC_MODE_OFFSET_GAIN,
};

static const bool ecu_sensors_map_enabled_default[ECU_SENSOR_MAP_MAX] = {
    true,
};

static const ecu_gpio_input_pin_t ecu_sensors_map_input_pin_default[ECU_SENSOR_MAP_MAX] = {
    ECU_IN_PORT2_PIN10,
};

static ecu_sensors_map_ctx_t ecu_sensors_map_ctx[ECU_SENSOR_MAP_MAX] = {
    {
      .init = {

      },
      .config_default = ecu_sensors_map_config_default,
    },
    {
      .init = {

      },
      .config_default = ecu_sensors_map_config_default,
    },
    {
      .init = {

      },
      .config_default = ecu_sensors_map_config_default,
    },
    {
      .init = {

      },
      .config_default = ecu_sensors_map_config_default,
    },
};

error_t ecu_sensors_map_init(ecu_sensor_map_t instance, map_ctx_t *ctx)
{
  error_t err = E_OK;
  ecu_sensors_map_ctx_t *map_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_SENSOR_MAP_MAX || ctx == NULL, err = E_PARAM);

    map_ctx = &ecu_sensors_map_ctx[instance];
    map_ctx->ctx = ctx;

    map_ctx->config_default.enabled = ecu_sensors_map_enabled_default[instance];
    map_ctx->config_default.input_pin = ecu_sensors_map_input_pin_default[instance];

    err = map_init(map_ctx->ctx, &map_ctx->init);
    BREAK_IF(err != E_OK);

    memcpy(&map_ctx->ctx->config, &map_ctx->config_default, sizeof(map_config_t));

  } while(0);

  return err;
}

error_t ecu_sensors_map_get_default_config(ecu_sensor_map_t instance, map_config_t *config)
{
  error_t err = E_OK;
  ecu_sensors_map_ctx_t *map_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_SENSOR_MAP_MAX || config == NULL, err = E_PARAM);

    map_ctx = &ecu_sensors_map_ctx[instance];

    memcpy(config, &map_ctx->config_default, sizeof(map_config_t));

  } while(0);

  return err;
}

error_t ecu_sensors_map_configure(ecu_sensor_map_t instance, const map_config_t *config)
{
  error_t err = E_OK;
  ecu_sensors_map_ctx_t *map_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_SENSOR_MAP_MAX || config == NULL, err = E_PARAM);

    map_ctx = &ecu_sensors_map_ctx[instance];

    err = map_configure(map_ctx->ctx, config);

  } while(0);

  return err;
}

error_t ecu_sensors_map_reset(ecu_sensor_map_t instance)
{
  error_t err = E_OK;
  ecu_sensors_map_ctx_t *map_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_SENSOR_MAP_MAX, err = E_PARAM);

    map_ctx = &ecu_sensors_map_ctx[instance];

    err = map_reset(map_ctx->ctx);

  } while(0);

  return err;
}

error_t ecu_sensors_map_get_value(ecu_sensor_map_t instance, map_data_t *data)
{
  error_t err = E_OK;
  ecu_sensors_map_ctx_t *map_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_SENSOR_MAP_MAX, err = E_PARAM);
    BREAK_IF_ACTION(data == NULL, err = E_PARAM);

    map_ctx = &ecu_sensors_map_ctx[instance];

    err = map_get_value(map_ctx->ctx, data);

  } while(0);

  return err;
}

error_t ecu_sensors_map_get_diag(ecu_sensor_map_t instance, map_diag_t *diag)
{
  error_t err = E_OK;
  ecu_sensors_map_ctx_t *map_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_SENSOR_MAP_MAX, err = E_PARAM);
    BREAK_IF_ACTION(diag == NULL, err = E_PARAM);

    map_ctx = &ecu_sensors_map_ctx[instance];

    err = map_get_diag(map_ctx->ctx, diag);

  } while(0);

  return err;
}
