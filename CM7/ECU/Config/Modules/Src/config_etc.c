/*
 * config_etc.c
 *
 *  Created on: May 15, 2024
 *      Author: VHEMaster
 */

#include <string.h>
#include "config_etc.h"
#include "config_extern.h"
#include "compiler.h"


typedef struct {
    etc_config_t config_default;
    etc_init_ctx_t init;
    etc_ctx_t *ctx;
}ecu_modules_etc_ctx_t;

static const etc_config_t ecu_modules_etc_config_default = {
    .power_voltage_pin = ECU_IN_PORT2_VIGN,
    .pwm_freq = 4000,

    .pid_position = {
        .Kp = 150.0,
        .Ki = 50.0,
        .Kd = 1.00f,
    },
    .pid_speed = {
        .Kp = 0.005f,
    },

    .motor_reach_threshold = 4.0f,
    .motor_reach_timeout = 200 * TIME_US_IN_MS,
};

static RAM_SECTION ecu_modules_etc_ctx_t ecu_modules_etc_ctx[ECU_MODULE_ETC_MAX] = {
    {
      .init = {

      },
      .config_default = ecu_modules_etc_config_default,
    },
    {
      .init = {

      },
      .config_default = ecu_modules_etc_config_default,
    },
};

static const bool ecu_sensors_etc_enabled_default[ECU_MODULE_ETC_MAX] = {
    true,
    false
};

static const ecu_sensor_tps_t ecu_sensors_etc_tps_default[ECU_MODULE_ETC_MAX] = {
    ECU_SENSOR_TPS_1,
    ECU_SENSOR_TPS_2
};

static const ecu_device_motor_t ecu_sensors_etc_motor_default[ECU_MODULE_ETC_MAX] = {
    ECU_DEVICE_MOTOR_1,
    ECU_DEVICE_MOTOR_2
};

error_t ecu_modules_etc_init(ecu_module_etc_t instance, etc_ctx_t *ctx)
{
  error_t err = E_OK;
  ecu_modules_etc_ctx_t *etc_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_MODULE_ETC_MAX || ctx == NULL, err = E_PARAM);

    etc_ctx = &ecu_modules_etc_ctx[instance];
    etc_ctx->ctx = ctx;

    etc_ctx->config_default.enabled = ecu_sensors_etc_enabled_default[instance];
    etc_ctx->config_default.sensor_tps = ecu_sensors_etc_tps_default[instance];
    etc_ctx->config_default.device_motor = ecu_sensors_etc_motor_default[instance];

    err = etc_init(etc_ctx->ctx, &etc_ctx->init);
    BREAK_IF(err != E_OK);

    memcpy(&etc_ctx->ctx->config, &etc_ctx->config_default, sizeof(etc_config_t));

  } while(0);

  return err;
}

error_t ecu_modules_etc_get_default_config(ecu_module_etc_t instance, etc_config_t *config)
{
  error_t err = E_OK;
  ecu_modules_etc_ctx_t *etc_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_MODULE_ETC_MAX || config == NULL, err = E_PARAM);

    etc_ctx = &ecu_modules_etc_ctx[instance];

    memcpy(config, &etc_ctx->config_default, sizeof(etc_config_t));

  } while(0);

  return err;
}

error_t ecu_modules_etc_configure(ecu_module_etc_t instance, const etc_config_t *config)
{
  error_t err = E_OK;
  ecu_modules_etc_ctx_t *etc_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_MODULE_ETC_MAX || config == NULL, err = E_PARAM);

    etc_ctx = &ecu_modules_etc_ctx[instance];

    err = etc_configure(etc_ctx->ctx, config);

  } while(0);

  return err;
}

error_t ecu_modules_etc_reset(ecu_module_etc_t instance)
{
  error_t err = E_OK;
  ecu_modules_etc_ctx_t *etc_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_MODULE_ETC_MAX, err = E_PARAM);

    etc_ctx = &ecu_modules_etc_ctx[instance];

    err = etc_reset(etc_ctx->ctx);

  } while(0);

  return err;
}

error_t ecu_modules_etc_set_enabled(ecu_module_etc_t instance, bool enabled)
{
  error_t err = E_OK;
  ecu_modules_etc_ctx_t *etc_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_MODULE_ETC_MAX, err = E_PARAM);

    etc_ctx = &ecu_modules_etc_ctx[instance];

    etc_ctx->ctx->data.enabled = enabled;

  } while(0);

  return err;
}

error_t ecu_modules_etc_set_target_position(ecu_module_etc_t instance, float position)
{
  error_t err = E_OK;
  ecu_modules_etc_ctx_t *etc_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_MODULE_ETC_MAX, err = E_PARAM);

    etc_ctx = &ecu_modules_etc_ctx[instance];

    etc_ctx->ctx->data.target_position = position;

  } while(0);

  return err;
}

error_t ecu_modules_etc_get_data(ecu_module_etc_t instance, etc_data_t *data)
{
  error_t err = E_OK;
  ecu_modules_etc_ctx_t *etc_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_MODULE_ETC_MAX, err = E_PARAM);
    BREAK_IF_ACTION(data == NULL, err = E_PARAM);

    etc_ctx = &ecu_modules_etc_ctx[instance];

    err = etc_get_data(etc_ctx->ctx, data);

  } while(0);

  return err;
}

error_t ecu_modules_etc_get_diag(ecu_module_etc_t instance, etc_diag_t *diag)
{
  error_t err = E_OK;
  ecu_modules_etc_ctx_t *etc_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_MODULE_ETC_MAX, err = E_PARAM);
    BREAK_IF_ACTION(diag == NULL, err = E_PARAM);

    etc_ctx = &ecu_modules_etc_ctx[instance];

    err = etc_get_diag(etc_ctx->ctx, diag);

  } while(0);

  return err;
}
