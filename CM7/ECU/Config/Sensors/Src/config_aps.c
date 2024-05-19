/*
 * config_aps.c
 *
 *  Created on: Apr 17, 2024
 *      Author: VHEMaster
 */

#include <string.h>
#include "config_aps.h"
#include "config_extern.h"
#include "compiler.h"

typedef struct {
    aps_config_t config_default;
    aps_init_ctx_t init;
    aps_ctx_t *ctx;
}ecu_sensors_aps_ctx_t;

static const aps_config_t ecu_sensors_aps_config_default = {
    .signals_used_count = 2,
    .signals = {
        {
            .voltage_pos_min = 1.252f,
            .voltage_pos_max = 4.360f,

            .voltage_low_thr = 1.15f,
            .voltage_high_thr = 4.45f,
        },
        {
            .voltage_pos_min = 0.884f,
            .voltage_pos_max = 2.519f,

            .voltage_low_thr = 0.80f,
            .voltage_high_thr = 2.60f,
        },
    },
    .signals_position_imbalance_max = 1.0f,
    .position_min = 0.0f,
    .position_max = 100.0f,

    .position_min_clamp = 0.0f,
    .position_max_clamp = 100.0f,

    .slew_rate = 4000.0f,
    .dead_zone = 2.0f,

    .boot_time = 50 * TIME_US_IN_MS,
};

static const bool ecu_sensors_aps_enabled_default[ECU_SENSOR_APS_MAX] = {
    true,
};

static const ecu_gpio_input_pin_t ecu_sensors_aps_input_pin_default[ECU_SENSOR_APS_MAX][APS_CONFIG_SIGNALS_MAX] = {
    { ECU_IN_PORT1_PIN11, ECU_IN_PORT1_PIN12 },
};

static ecu_sensors_aps_ctx_t ecu_sensors_aps_ctx[ECU_SENSOR_APS_MAX] = {
    {
      .init = {

      },
      .config_default = ecu_sensors_aps_config_default,
    },
};

error_t ecu_sensors_aps_init(ecu_sensor_aps_t instance, aps_ctx_t *ctx)
{
  error_t err = E_OK;
  ecu_sensors_aps_ctx_t *aps_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_SENSOR_APS_MAX || ctx == NULL, err = E_PARAM);

    aps_ctx = &ecu_sensors_aps_ctx[instance];
    aps_ctx->ctx = ctx;

    aps_ctx->config_default.enabled = ecu_sensors_aps_enabled_default[instance];
    for(int i = 0; i < APS_CONFIG_SIGNALS_MAX; i++) {
      aps_ctx->config_default.signals[i].input_pin = ecu_sensors_aps_input_pin_default[instance][i];
    }

    err = aps_init(aps_ctx->ctx, &aps_ctx->init);
    BREAK_IF(err != E_OK);

    memcpy(&aps_ctx->ctx->config, &aps_ctx->config_default, sizeof(aps_config_t));

  } while(0);

  return err;
}

error_t ecu_sensors_aps_get_default_config(ecu_sensor_aps_t instance, aps_config_t *config)
{
  error_t err = E_OK;
  ecu_sensors_aps_ctx_t *aps_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_SENSOR_APS_MAX || config == NULL, err = E_PARAM);

    aps_ctx = &ecu_sensors_aps_ctx[instance];

    memcpy(config, &aps_ctx->config_default, sizeof(aps_config_t));

  } while(0);

  return err;
}

error_t ecu_sensors_aps_configure(ecu_sensor_aps_t instance, const aps_config_t *config)
{
  error_t err = E_OK;
  ecu_sensors_aps_ctx_t *aps_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_SENSOR_APS_MAX || config == NULL, err = E_PARAM);

    aps_ctx = &ecu_sensors_aps_ctx[instance];

    err = aps_configure(aps_ctx->ctx, config);

  } while(0);

  return err;
}

error_t ecu_sensors_aps_reset(ecu_sensor_aps_t instance)
{
  error_t err = E_OK;
  ecu_sensors_aps_ctx_t *aps_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_SENSOR_APS_MAX, err = E_PARAM);

    aps_ctx = &ecu_sensors_aps_ctx[instance];

    err = aps_reset(aps_ctx->ctx);

  } while(0);

  return err;
}

error_t ecu_sensors_aps_get_value(ecu_sensor_aps_t instance, aps_data_t *data)
{
  error_t err = E_OK;
  ecu_sensors_aps_ctx_t *aps_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_SENSOR_APS_MAX, err = E_PARAM);
    BREAK_IF_ACTION(data == NULL, err = E_PARAM);

    aps_ctx = &ecu_sensors_aps_ctx[instance];

    err = aps_get_value(aps_ctx->ctx, data);

  } while(0);

  return err;
}

error_t ecu_sensors_aps_get_diag(ecu_sensor_aps_t instance, aps_diag_t *diag)
{
  error_t err = E_OK;
  ecu_sensors_aps_ctx_t *aps_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_SENSOR_APS_MAX, err = E_PARAM);
    BREAK_IF_ACTION(diag == NULL, err = E_PARAM);

    aps_ctx = &ecu_sensors_aps_ctx[instance];

    err = aps_get_diag(aps_ctx->ctx, diag);

  } while(0);

  return err;
}
