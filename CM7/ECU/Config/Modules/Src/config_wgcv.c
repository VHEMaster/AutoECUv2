/*
 * config_wgcv.c
 *
 *  Created on: Jun 9, 2024
 *      Author: VHEMaster
 */

#include <string.h>
#include "config_wgcv.h"
#include "config_extern.h"
#include "compiler.h"

typedef struct {
    wgcv_config_t config_default;
    wgcv_init_ctx_t init;
    wgcv_ctx_t *ctx;
}ecu_modules_wgcv_ctx_t;

static const wgcv_config_t ecu_modules_wgcv_config_default = {
    .power_voltage_pin = ECU_IN_PORT1_VIGN,
    .pwm_freq = 100,

    .voltage_to_pwm_dutycycle = {
        .full_closed = {
            .items = 10,
            .input = {
                6.0f, 8.0f, 10.0f, 11.0f, 12.0f,
                13.0f, 14.0f, 15.0f, 16.0f, 18.0f,
            },
            .output = {
                0.98f, 0.79f, 0.52f, 0.45f, 0.41f,
                0.36f, 0.32f, 0.30f, 0.29f, 0.23f,
            },
        },
        .full_open = {
            .items = 10,
            .input = {
                6.0f, 8.0f, 10.0f, 11.0f, 12.0f,
                13.0f, 14.0f, 15.0f, 16.0f, 18.0f,
            },
            .output = {
                0.99f, 0.88f, 0.86f, 0.84f, 0.84f,
                0.83f, 0.83f, 0.82f, 0.81f, 0.80f,
            },
        },
    },

    .pwm_dutycycle_min = 0.05f,
    .pwm_dutycycle_max = 0.98f,

    .input_dutycycle_min = 0.05f,
    .input_dutycycle_max = 0.95f,

    .pid_boost = {
        .Kp = 0.0,
        .Ki = 0.0,
        .Kd = 0.0,
    },
    .pid_speed = {
        .Kp = 0.0f,
    },
};

static ecu_modules_wgcv_ctx_t ecu_modules_wgcv_ctx[ECU_MODULE_WGCV_MAX] = {
    {
      .init = {

      },
      .config_default = ecu_modules_wgcv_config_default,
    },
    {
      .init = {

      },
      .config_default = ecu_modules_wgcv_config_default,
    },
};

static const bool ecu_sensors_wgcv_enabled_default[ECU_MODULE_WGCV_MAX] = {
    true,
    false,
};

static const ecu_gpio_output_pin_t ecu_sensors_wgcv_output_pwm_pin_default[ECU_MODULE_WGCV_MAX] = {
    ECU_OUT_PORT1_PIN1,
    ECU_OUT_PORT1_PIN2,
};

error_t ecu_modules_wgcv_init(ecu_module_wgcv_t instance, wgcv_ctx_t *ctx)
{
  error_t err = E_OK;
  ecu_modules_wgcv_ctx_t *wgcv_ctx;

  do {
    BREAK_IF_ACTION(instance <= ECU_MODULE_NONE || instance >= ECU_MODULE_WGCV_MAX || ctx == NULL, err = E_PARAM);

    wgcv_ctx = &ecu_modules_wgcv_ctx[instance];
    wgcv_ctx->ctx = ctx;

    wgcv_ctx->config_default.enabled = ecu_sensors_wgcv_enabled_default[instance];
    wgcv_ctx->config_default.output_pwm_pin = ecu_sensors_wgcv_output_pwm_pin_default[instance];

    err = wgcv_init(wgcv_ctx->ctx, &wgcv_ctx->init);
    BREAK_IF(err != E_OK);

    memcpy(&wgcv_ctx->ctx->config, &wgcv_ctx->config_default, sizeof(wgcv_config_t));

  } while(0);

  return err;
}

error_t ecu_modules_wgcv_get_default_config(ecu_module_wgcv_t instance, wgcv_config_t *config)
{
  error_t err = E_OK;
  ecu_modules_wgcv_ctx_t *wgcv_ctx;

  do {
    BREAK_IF_ACTION(instance <= ECU_MODULE_NONE || instance >= ECU_MODULE_WGCV_MAX || config == NULL, err = E_PARAM);

    wgcv_ctx = &ecu_modules_wgcv_ctx[instance];

    memcpy(config, &wgcv_ctx->config_default, sizeof(wgcv_config_t));

  } while(0);

  return err;
}

error_t ecu_modules_wgcv_configure(ecu_module_wgcv_t instance, const wgcv_config_t *config)
{
  error_t err = E_OK;
  ecu_modules_wgcv_ctx_t *wgcv_ctx;

  do {
    BREAK_IF_ACTION(instance <= ECU_MODULE_NONE || instance >= ECU_MODULE_WGCV_MAX || config == NULL, err = E_PARAM);

    wgcv_ctx = &ecu_modules_wgcv_ctx[instance];

    err = wgcv_configure(wgcv_ctx->ctx, config);

  } while(0);

  return err;
}

error_t ecu_modules_wgcv_reset(ecu_module_wgcv_t instance)
{
  error_t err = E_OK;
  ecu_modules_wgcv_ctx_t *wgcv_ctx;

  do {
    BREAK_IF_ACTION(instance <= ECU_MODULE_NONE || instance >= ECU_MODULE_WGCV_MAX, err = E_PARAM);

    wgcv_ctx = &ecu_modules_wgcv_ctx[instance];

    err = wgcv_reset(wgcv_ctx->ctx);

  } while(0);

  return err;
}

error_t ecu_modules_wgcv_set_enabled(ecu_module_wgcv_t instance, bool enabled)
{
  error_t err = E_OK;
  ecu_modules_wgcv_ctx_t *wgcv_ctx;

  do {
    BREAK_IF_ACTION(instance <= ECU_MODULE_NONE || instance >= ECU_MODULE_WGCV_MAX, err = E_PARAM);

    wgcv_ctx = &ecu_modules_wgcv_ctx[instance];

    err = wgcv_set_enabled(wgcv_ctx->ctx, enabled);

  } while(0);

  return err;
}

error_t ecu_modules_wgcv_set_dutycycle(ecu_module_wgcv_t instance, float dutycycle)
{
  error_t err = E_OK;
  ecu_modules_wgcv_ctx_t *wgcv_ctx;

  do {
    BREAK_IF_ACTION(instance <= ECU_MODULE_NONE || instance >= ECU_MODULE_WGCV_MAX, err = E_PARAM);

    wgcv_ctx = &ecu_modules_wgcv_ctx[instance];


    err = wgcv_set_dutycycle(wgcv_ctx->ctx, dutycycle);

  } while(0);

  return err;
}

error_t ecu_modules_wgcv_set_actual_boost(ecu_module_wgcv_t instance, float actual_boost)
{
  error_t err = E_OK;
  ecu_modules_wgcv_ctx_t *wgcv_ctx;

  do {
    BREAK_IF_ACTION(instance <= ECU_MODULE_NONE || instance >= ECU_MODULE_WGCV_MAX, err = E_PARAM);

    wgcv_ctx = &ecu_modules_wgcv_ctx[instance];


    err = wgcv_set_actual_boost(wgcv_ctx->ctx, actual_boost);

  } while(0);

  return err;
}

error_t ecu_modules_wgcv_set_target_boost(ecu_module_wgcv_t instance, float target_boost)
{
  error_t err = E_OK;
  ecu_modules_wgcv_ctx_t *wgcv_ctx;

  do {
    BREAK_IF_ACTION(instance <= ECU_MODULE_NONE || instance >= ECU_MODULE_WGCV_MAX, err = E_PARAM);

    wgcv_ctx = &ecu_modules_wgcv_ctx[instance];


    err = wgcv_set_target_boost(wgcv_ctx->ctx, target_boost);

  } while(0);

  return err;
}

error_t ecu_modules_wgcv_force_input_reset(ecu_module_wgcv_t instance)
{
  error_t err = E_OK;
  ecu_modules_wgcv_ctx_t *wgcv_ctx;

  do {
    BREAK_IF_ACTION(instance <= ECU_MODULE_NONE || instance >= ECU_MODULE_WGCV_MAX, err = E_PARAM);

    wgcv_ctx = &ecu_modules_wgcv_ctx[instance];

    err = wgcv_force_input_reset(wgcv_ctx->ctx);

  } while(0);

  return err;
}

error_t ecu_modules_wgcv_force_input_set(ecu_module_wgcv_t instance, float dutycycle)
{
  error_t err = E_OK;
  ecu_modules_wgcv_ctx_t *wgcv_ctx;

  do {
    BREAK_IF_ACTION(instance <= ECU_MODULE_NONE || instance >= ECU_MODULE_WGCV_MAX, err = E_PARAM);

    wgcv_ctx = &ecu_modules_wgcv_ctx[instance];

    err = wgcv_force_input_set(wgcv_ctx->ctx, dutycycle);

  } while(0);

  return err;
}

error_t ecu_modules_wgcv_force_pwm_reset(ecu_module_wgcv_t instance)
{
  error_t err = E_OK;
  ecu_modules_wgcv_ctx_t *wgcv_ctx;

  do {
    BREAK_IF_ACTION(instance <= ECU_MODULE_NONE || instance >= ECU_MODULE_WGCV_MAX, err = E_PARAM);

    wgcv_ctx = &ecu_modules_wgcv_ctx[instance];

    err = wgcv_force_pwm_reset(wgcv_ctx->ctx);

  } while(0);

  return err;
}

error_t ecu_modules_wgcv_force_pwm_set(ecu_module_wgcv_t instance, float dutycycle)
{
  error_t err = E_OK;
  ecu_modules_wgcv_ctx_t *wgcv_ctx;

  do {
    BREAK_IF_ACTION(instance <= ECU_MODULE_NONE || instance >= ECU_MODULE_WGCV_MAX, err = E_PARAM);

    wgcv_ctx = &ecu_modules_wgcv_ctx[instance];

    err = wgcv_force_pwm_set(wgcv_ctx->ctx, dutycycle);

  } while(0);

  return err;
}

error_t ecu_modules_wgcv_get_data(ecu_module_wgcv_t instance, wgcv_data_t *data)
{
  error_t err = E_OK;
  ecu_modules_wgcv_ctx_t *wgcv_ctx;

  do {
    BREAK_IF_ACTION(instance <= ECU_MODULE_NONE || instance >= ECU_MODULE_WGCV_MAX, err = E_PARAM);
    BREAK_IF_ACTION(data == NULL, err = E_PARAM);

    wgcv_ctx = &ecu_modules_wgcv_ctx[instance];

    err = wgcv_get_data(wgcv_ctx->ctx, data);

  } while(0);

  return err;
}

error_t ecu_modules_wgcv_get_diag(ecu_module_wgcv_t instance, wgcv_diag_t *diag)
{
  error_t err = E_OK;
  ecu_modules_wgcv_ctx_t *wgcv_ctx;

  do {
    BREAK_IF_ACTION(instance <= ECU_MODULE_NONE || instance >= ECU_MODULE_WGCV_MAX, err = E_PARAM);
    BREAK_IF_ACTION(diag == NULL, err = E_PARAM);

    wgcv_ctx = &ecu_modules_wgcv_ctx[instance];

    err = wgcv_get_diag(wgcv_ctx->ctx, diag);

  } while(0);

  return err;
}
