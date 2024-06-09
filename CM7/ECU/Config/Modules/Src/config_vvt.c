/*
 * config_vvt.c
 *
 *  Created on: Jun 9, 2024
 *      Author: VHEMaster
 */

#include <string.h>
#include "config_vvt.h"
#include "config_cmp.h"
#include "config_extern.h"
#include "compiler.h"

typedef struct {
    vvt_config_t config_default;
    vvt_init_ctx_t init;
    vvt_ctx_t *ctx;
}ecu_modules_vvt_ctx_t;

static void ecu_modules_vvt_cmp_signal_update_cb(void *usrdata, const cmp_data_t *data, const cmp_diag_t *diag);

static const vvt_config_t ecu_modules_vvt_config_default = {
    .power_voltage_pin = ECU_IN_PORT1_VIGN,
    .pwm_freq = 100,

    .pwm_dutycycle_min = 0.05f,
    .pwm_dutycycle_max = 0.95f,

    .pid_position = {
        .Kp = 50.0,
        .Ki = 20.0,
        .Kd = 0.30f,
    },
    .pid_speed = {
        .Kp = 0.002f,
    },

    .pos_reach_threshold = 3.0f,
    .pos_reach_timeout = 500 * TIME_US_IN_MS,
};

static ecu_modules_vvt_ctx_t ecu_modules_vvt_ctx[ECU_MODULE_VVT_MAX] = {
    {
      .init = {

      },
      .config_default = ecu_modules_vvt_config_default,
    },
    {
      .init = {

      },
      .config_default = ecu_modules_vvt_config_default,
    },
    {
      .init = {

      },
      .config_default = ecu_modules_vvt_config_default,
    },
    {
      .init = {

      },
      .config_default = ecu_modules_vvt_config_default,
    },
};

static const bool ecu_sensors_vvt_enabled_default[ECU_MODULE_VVT_MAX] = {
    false,
    false,
    false,
    false
};

static const ecu_gpio_output_pin_t ecu_sensors_vvt_output_pwm_pin_default[ECU_MODULE_VVT_MAX] = {
    ECU_OUT_PORT1_PIN1,
    ECU_OUT_PORT1_PIN2,
    ECU_OUT_PORT1_PIN3,
    ECU_OUT_PORT1_PIN4
};

static const ecu_sensor_cmp_t ecu_sensors_vvt_sensor_cmp_default[ECU_MODULE_VVT_MAX] = {
    ECU_SENSOR_CMP_1,
    ECU_SENSOR_CMP_2,
    ECU_SENSOR_CMP_3,
    ECU_SENSOR_CMP_4
};

error_t ecu_modules_vvt_init(ecu_module_vvt_t instance, vvt_ctx_t *ctx)
{
  error_t err = E_OK;
  ecu_modules_vvt_ctx_t *vvt_ctx;

  do {
    BREAK_IF_ACTION(instance <= ECU_MODULE_NONE || instance >= ECU_MODULE_VVT_MAX || ctx == NULL, err = E_PARAM);

    vvt_ctx = &ecu_modules_vvt_ctx[instance];
    vvt_ctx->ctx = ctx;

    vvt_ctx->config_default.enabled = ecu_sensors_vvt_enabled_default[instance];
    vvt_ctx->config_default.output_pwm_pin = ecu_sensors_vvt_output_pwm_pin_default[instance];
    vvt_ctx->config_default.sensor_cmp = ecu_sensors_vvt_sensor_cmp_default[instance];

    err = vvt_init(vvt_ctx->ctx, &vvt_ctx->init);
    BREAK_IF(err != E_OK);

    memcpy(&vvt_ctx->ctx->config, &vvt_ctx->config_default, sizeof(vvt_config_t));

  } while(0);

  return err;
}

error_t ecu_modules_vvt_get_default_config(ecu_module_vvt_t instance, vvt_config_t *config)
{
  error_t err = E_OK;
  ecu_modules_vvt_ctx_t *vvt_ctx;

  do {
    BREAK_IF_ACTION(instance <= ECU_MODULE_NONE || instance >= ECU_MODULE_VVT_MAX || config == NULL, err = E_PARAM);

    vvt_ctx = &ecu_modules_vvt_ctx[instance];

    memcpy(config, &vvt_ctx->config_default, sizeof(vvt_config_t));

  } while(0);

  return err;
}

error_t ecu_modules_vvt_configure(ecu_module_vvt_t instance, const vvt_config_t *config)
{
  error_t err = E_OK;
  ecu_modules_vvt_ctx_t *vvt_ctx;

  do {
    BREAK_IF_ACTION(instance <= ECU_MODULE_NONE || instance >= ECU_MODULE_VVT_MAX || config == NULL, err = E_PARAM);

    vvt_ctx = &ecu_modules_vvt_ctx[instance];

    err = ecu_sensors_cmp_register_cb(config->sensor_cmp, ecu_modules_vvt_cmp_signal_update_cb, vvt_ctx);
    BREAK_IF(err != E_OK);

    err = vvt_configure(vvt_ctx->ctx, config);

  } while(0);

  return err;
}

error_t ecu_modules_vvt_reset(ecu_module_vvt_t instance)
{
  error_t err = E_OK;
  ecu_modules_vvt_ctx_t *vvt_ctx;

  do {
    BREAK_IF_ACTION(instance <= ECU_MODULE_NONE || instance >= ECU_MODULE_VVT_MAX, err = E_PARAM);

    vvt_ctx = &ecu_modules_vvt_ctx[instance];

    err = vvt_reset(vvt_ctx->ctx);

  } while(0);

  return err;
}

error_t ecu_modules_vvt_set_enabled(ecu_module_vvt_t instance, bool enabled)
{
  error_t err = E_OK;
  ecu_modules_vvt_ctx_t *vvt_ctx;

  do {
    BREAK_IF_ACTION(instance <= ECU_MODULE_NONE || instance >= ECU_MODULE_VVT_MAX, err = E_PARAM);

    vvt_ctx = &ecu_modules_vvt_ctx[instance];

    err = vvt_set_enabled(vvt_ctx->ctx, enabled);

  } while(0);

  return err;
}

error_t ecu_modules_vvt_set_target_position(ecu_module_vvt_t instance, float position)
{
  error_t err = E_OK;
  ecu_modules_vvt_ctx_t *vvt_ctx;

  do {
    BREAK_IF_ACTION(instance <= ECU_MODULE_NONE || instance >= ECU_MODULE_VVT_MAX, err = E_PARAM);

    vvt_ctx = &ecu_modules_vvt_ctx[instance];

    err = vvt_set_target_position(vvt_ctx->ctx, position);

  } while(0);

  return err;
}

error_t ecu_modules_vvt_set_target_dutycycle(ecu_module_vvt_t instance, float dutycycle)
{
  error_t err = E_OK;
  ecu_modules_vvt_ctx_t *vvt_ctx;

  do {
    BREAK_IF_ACTION(instance <= ECU_MODULE_NONE || instance >= ECU_MODULE_VVT_MAX, err = E_PARAM);

    vvt_ctx = &ecu_modules_vvt_ctx[instance];


    err = vvt_set_target_dutycycle(vvt_ctx->ctx, dutycycle);

  } while(0);

  return err;
}

error_t ecu_modules_vvt_get_data(ecu_module_vvt_t instance, vvt_data_t *data)
{
  error_t err = E_OK;
  ecu_modules_vvt_ctx_t *vvt_ctx;

  do {
    BREAK_IF_ACTION(instance <= ECU_MODULE_NONE || instance >= ECU_MODULE_VVT_MAX, err = E_PARAM);
    BREAK_IF_ACTION(data == NULL, err = E_PARAM);

    vvt_ctx = &ecu_modules_vvt_ctx[instance];

    err = vvt_get_data(vvt_ctx->ctx, data);

  } while(0);

  return err;
}

error_t ecu_modules_vvt_get_diag(ecu_module_vvt_t instance, vvt_diag_t *diag)
{
  error_t err = E_OK;
  ecu_modules_vvt_ctx_t *vvt_ctx;

  do {
    BREAK_IF_ACTION(instance <= ECU_MODULE_NONE || instance >= ECU_MODULE_VVT_MAX, err = E_PARAM);
    BREAK_IF_ACTION(diag == NULL, err = E_PARAM);

    vvt_ctx = &ecu_modules_vvt_ctx[instance];

    err = vvt_get_diag(vvt_ctx->ctx, diag);

  } while(0);

  return err;
}

ITCM_FUNC static void ecu_modules_vvt_cmp_signal_update_cb(void *usrdata, const cmp_data_t *data, const cmp_diag_t *diag)
{
  ecu_modules_vvt_ctx_t *module_ctx = (ecu_modules_vvt_ctx_t *)usrdata;
  vvt_ctx_t *ctx;

  do {
    BREAK_IF(module_ctx == NULL);
    ctx = module_ctx->ctx;
    BREAK_IF(ctx == NULL);

    vvt_cmp_signal_update(ctx, data, diag);

  } while(0);
}
