/*
 * config_tps.c
 *
 *  Created on: Apr 17, 2024
 *      Author: VHEMaster
 */

#include <string.h>
#include "config_tps.h"
#include "config_extern.h"
#include "compiler.h"

typedef struct {
    tps_config_t config_default;
    tps_init_ctx_t init;
    tps_ctx_t *ctx;
}ecu_sensors_tps_ctx_t;

static const tps_config_t ecu_sensors_tps_config_default = {
    .signals_used_count = 2,
    .signals = {
        {
            .voltage_pos_min = 0.485f,
            .voltage_pos_max = 4.682f,

            .voltage_low_thr = 0.40f,
            .voltage_high_thr = 4.76f,
        },
        {
            .voltage_pos_min = 4.524f,
            .voltage_pos_max = 0.328f,

            .voltage_low_thr = 0.26f,
            .voltage_high_thr = 4.60f,
        },
    },
    .signals_position_imbalance_max = 0.5f,
    .position_min = 0.0f,
    .position_max = 107.0f,

    .position_min_clamp = 0.0f,
    .position_max_clamp = 100.0f,

    .slew_rate = 4000.0f,
    .dead_zone = 0.0f,

    .boot_time = 50 * TIME_US_IN_MS,
};

static const bool ecu_sensors_tps_enabled_default[ECU_SENSOR_TPS_MAX] = {
    true,
};

static const ecu_gpio_input_pin_t ecu_sensors_tps_input_pin_default[ECU_SENSOR_TPS_MAX][TPS_CONFIG_SIGNALS_MAX] = {
    { ECU_IN_PORT1_PIN9, ECU_IN_PORT1_PIN10 },
};

static ecu_sensors_tps_ctx_t ecu_sensors_tps_ctx[ECU_SENSOR_TPS_MAX] = {
    {
      .init = {

      },
      .config_default = ecu_sensors_tps_config_default,
    },
    {
      .init = {

      },
      .config_default = ecu_sensors_tps_config_default,
    },
};

error_t ecu_sensors_tps_init(ecu_sensor_tps_t instance, tps_ctx_t *ctx)
{
  error_t err = E_OK;
  ecu_sensors_tps_ctx_t *tps_ctx;

  do {
    BREAK_IF_ACTION(instance <= ECU_SENSOR_NONE || instance >= ECU_SENSOR_TPS_MAX || ctx == NULL, err = E_PARAM);

    tps_ctx = &ecu_sensors_tps_ctx[instance];
    tps_ctx->ctx = ctx;

    tps_ctx->config_default.enabled = ecu_sensors_tps_enabled_default[instance];
    for(int i = 0; i < TPS_CONFIG_SIGNALS_MAX; i++) {
      tps_ctx->config_default.signals[i].input_pin = ecu_sensors_tps_input_pin_default[instance][i];
    }

    err = tps_init(tps_ctx->ctx, &tps_ctx->init);
    BREAK_IF(err != E_OK);

    memcpy(&tps_ctx->ctx->config, &tps_ctx->config_default, sizeof(tps_config_t));

  } while(0);

  return err;
}

error_t ecu_sensors_tps_get_default_config(ecu_sensor_tps_t instance, tps_config_t *config)
{
  error_t err = E_OK;
  ecu_sensors_tps_ctx_t *tps_ctx;

  do {
    BREAK_IF_ACTION(instance <= ECU_SENSOR_NONE || instance >= ECU_SENSOR_TPS_MAX || config == NULL, err = E_PARAM);

    tps_ctx = &ecu_sensors_tps_ctx[instance];

    memcpy(config, &tps_ctx->config_default, sizeof(tps_config_t));

  } while(0);

  return err;
}

error_t ecu_sensors_tps_configure(ecu_sensor_tps_t instance, const tps_config_t *config)
{
  error_t err = E_OK;
  ecu_sensors_tps_ctx_t *tps_ctx;

  do {
    BREAK_IF_ACTION(instance <= ECU_SENSOR_NONE || instance >= ECU_SENSOR_TPS_MAX || config == NULL, err = E_PARAM);

    tps_ctx = &ecu_sensors_tps_ctx[instance];

    err = tps_configure(tps_ctx->ctx, config);

  } while(0);

  return err;
}

error_t ecu_sensors_tps_reset(ecu_sensor_tps_t instance)
{
  error_t err = E_OK;
  ecu_sensors_tps_ctx_t *tps_ctx;

  do {
    BREAK_IF_ACTION(instance <= ECU_SENSOR_NONE || instance >= ECU_SENSOR_TPS_MAX, err = E_PARAM);

    tps_ctx = &ecu_sensors_tps_ctx[instance];

    err = tps_reset(tps_ctx->ctx);

  } while(0);

  return err;
}

error_t ecu_sensors_tps_get_value(ecu_sensor_tps_t instance, tps_data_t *data)
{
  error_t err = E_OK;
  ecu_sensors_tps_ctx_t *tps_ctx;

  do {
    BREAK_IF_ACTION(instance <= ECU_SENSOR_NONE || instance >= ECU_SENSOR_TPS_MAX, err = E_PARAM);
    BREAK_IF_ACTION(data == NULL, err = E_PARAM);

    tps_ctx = &ecu_sensors_tps_ctx[instance];

    err = tps_get_value(tps_ctx->ctx, data);

  } while(0);

  return err;
}

error_t ecu_sensors_tps_get_diag(ecu_sensor_tps_t instance, tps_diag_t *diag)
{
  error_t err = E_OK;
  ecu_sensors_tps_ctx_t *tps_ctx;

  do {
    BREAK_IF_ACTION(instance <= ECU_SENSOR_NONE || instance >= ECU_SENSOR_TPS_MAX, err = E_PARAM);
    BREAK_IF_ACTION(diag == NULL, err = E_PARAM);

    tps_ctx = &ecu_sensors_tps_ctx[instance];

    err = tps_get_diag(tps_ctx->ctx, diag);

  } while(0);

  return err;
}
