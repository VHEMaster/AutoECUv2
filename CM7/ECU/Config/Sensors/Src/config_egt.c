/*
 * config_egt.c
 *
 *  Created on: Apr 17, 2024
 *      Author: VHEMaster
 */

#include <string.h>
#include "config_egt.h"
#include "config_extern.h"
#include "compiler.h"

typedef struct {
    egt_config_t config_default;
    egt_init_ctx_t init;
    egt_ctx_t *ctx;
}ecu_sensors_egt_ctx_t;

static const egt_config_t ecu_sensors_egt_config_default = {
    .boot_time = 100 * TIME_US_IN_MS,
    .temperature_too_low = -100.0f,
    .temperature_too_high = 1200.0f,
};

static const bool ecu_sensors_egt_enabled_default[ECU_SENSOR_EGT_MAX] = {
    true,
    true,
};

static const ecu_device_tcs_t ecu_sensors_egt_tcs_default[ECU_SENSOR_EGT_MAX] = {
    ECU_DEVICE_TCS_1,
    ECU_DEVICE_TCS_2,
};

static RAM_SECTION ecu_sensors_egt_ctx_t ecu_sensors_egt_ctx[ECU_SENSOR_EGT_MAX] = {
    {
      .init = {

      },
      .config_default = ecu_sensors_egt_config_default,
    },
    {
      .init = {

      },
      .config_default = ecu_sensors_egt_config_default,
    },
};

error_t ecu_sensors_egt_init(ecu_sensor_egt_t instance, egt_ctx_t *ctx)
{
  error_t err = E_OK;
  ecu_sensors_egt_ctx_t *egt_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_SENSOR_EGT_MAX || ctx == NULL, err = E_PARAM);

    egt_ctx = &ecu_sensors_egt_ctx[instance];
    egt_ctx->ctx = ctx;

    egt_ctx->config_default.enabled = ecu_sensors_egt_enabled_default[instance];
    egt_ctx->config_default.tcs_instance = ecu_sensors_egt_tcs_default[instance];

    err = egt_init(egt_ctx->ctx, &egt_ctx->init);
    BREAK_IF(err != E_OK);

    memcpy(&egt_ctx->ctx->config, &egt_ctx->config_default, sizeof(egt_config_t));

  } while(0);

  return err;
}

error_t ecu_sensors_egt_get_default_config(ecu_sensor_egt_t instance, egt_config_t *config)
{
  error_t err = E_OK;
  ecu_sensors_egt_ctx_t *egt_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_SENSOR_EGT_MAX || config == NULL, err = E_PARAM);

    egt_ctx = &ecu_sensors_egt_ctx[instance];

    memcpy(config, &egt_ctx->config_default, sizeof(egt_config_t));

  } while(0);

  return err;
}

error_t ecu_sensors_egt_configure(ecu_sensor_egt_t instance, const egt_config_t *config)
{
  error_t err = E_OK;
  ecu_sensors_egt_ctx_t *egt_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_SENSOR_EGT_MAX || config == NULL, err = E_PARAM);

    egt_ctx = &ecu_sensors_egt_ctx[instance];

    err = egt_configure(egt_ctx->ctx, config);

  } while(0);

  return err;
}

error_t ecu_sensors_egt_reset(ecu_sensor_egt_t instance)
{
  error_t err = E_OK;
  ecu_sensors_egt_ctx_t *egt_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_SENSOR_EGT_MAX, err = E_PARAM);

    egt_ctx = &ecu_sensors_egt_ctx[instance];

    err = egt_reset(egt_ctx->ctx);

  } while(0);

  return err;
}

error_t ecu_sensors_egt_get_value(ecu_sensor_egt_t instance, egt_data_t *data)
{
  error_t err = E_OK;
  ecu_sensors_egt_ctx_t *egt_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_SENSOR_EGT_MAX, err = E_PARAM);
    BREAK_IF_ACTION(data == NULL, err = E_PARAM);

    egt_ctx = &ecu_sensors_egt_ctx[instance];

    err = egt_get_value(egt_ctx->ctx, data);

  } while(0);

  return err;
}

error_t ecu_sensors_egt_get_diag(ecu_sensor_egt_t instance, egt_diag_t *diag)
{
  error_t err = E_OK;
  ecu_sensors_egt_ctx_t *egt_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_SENSOR_EGT_MAX, err = E_PARAM);
    BREAK_IF_ACTION(diag == NULL, err = E_PARAM);

    egt_ctx = &ecu_sensors_egt_ctx[instance];

    err = egt_get_diag(egt_ctx->ctx, diag);

  } while(0);

  return err;
}
