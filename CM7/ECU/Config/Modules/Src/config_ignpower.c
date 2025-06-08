/*
 * config_ignpower.c
 *
 *  Created on: Jun 23, 2024
 *      Author: VHEMaster
 */

#include <string.h>
#include "config_ignpower.h"
#include "config_ckp.h"
#include "config_extern.h"
#include "compiler.h"

static void ecu_modules_ignpower_signal_update_cb(void *usrdata, bool ignpower_active);
static void ecu_modules_ignpower_ckp_signal_update_cb(void *usrdata, const ckp_data_t *data, const ckp_diag_t *diag);


typedef struct {
    ignpower_signal_update_cb_t callback;
    void *usrdata;
}ecu_modules_ignpower_cb_t;

typedef struct {
    ignpower_config_t config_default;
    ignpower_init_ctx_t init;
    ignpower_ctx_t *ctx;
    ecu_modules_ignpower_cb_t signal_update_callbacks[ECU_MODULES_IGNPOWER_CALLBACKS_MAX];
}ecu_modules_ignpower_ctx_t;

static const ignpower_config_t ecu_modules_ignpower_config_default = {
    .sensor_ckp = ECU_SENSOR_CKP_1,
    .input_debounce_off_to_on = 100 * TIME_US_IN_MS,
    .input_debounce_on_to_off = 100 * TIME_US_IN_MS,
    .components_operating_trigger = true,
    .crankshaft_operating_trigger = true,
    .output_shutdown_min_delay = 3000 * TIME_US_IN_MS,
    .output_shutdown_operation_delay = 500 * TIME_US_IN_MS,
    .output_shutdown_crankshaft_delay = 1000 * TIME_US_IN_MS,
    .input_gpio_inverted = false,
    .output_gpio_inverted = false,
};

static ecu_modules_ignpower_ctx_t ecu_modules_ignpower_ctx[ECU_MODULE_IGNPOWER_MAX] = {
    {
      .init = {
          .signal_update_cb = ecu_modules_ignpower_signal_update_cb,
          .signal_update_usrdata = &ecu_modules_ignpower_ctx[0],
      },
      .config_default = ecu_modules_ignpower_config_default,
    },
};

static const bool ecu_modules_ignpower_enabled_default[ECU_MODULE_IGNPOWER_MAX] = {
    true,
};

static const ecu_gpio_input_pin_t ecu_modules_ignpower_input_signal_pin_default[ECU_MODULE_IGNPOWER_MAX] = {
    ECU_IN_PORT2_PIN15,
};

static const ecu_gpio_output_pin_t ecu_modules_ignpower_output_relay_pin_default[ECU_MODULE_IGNPOWER_MAX] = {
    ECU_OUT_PORT1_PIN16,
};

error_t ecu_modules_ignpower_init(ecu_module_ignpower_t instance, ignpower_ctx_t *ctx)
{
  error_t err = E_OK;
  ecu_modules_ignpower_ctx_t *ignpower_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_MODULE_IGNPOWER_MAX || ctx == NULL, err = E_PARAM);

    ignpower_ctx = &ecu_modules_ignpower_ctx[instance];
    ignpower_ctx->ctx = ctx;

    ignpower_ctx->config_default.enabled = ecu_modules_ignpower_enabled_default[instance];
    ignpower_ctx->config_default.input_signal_pin = ecu_modules_ignpower_input_signal_pin_default[instance];
    ignpower_ctx->config_default.output_relay_pin = ecu_modules_ignpower_output_relay_pin_default[instance];

    err = ignpower_init(ignpower_ctx->ctx, &ignpower_ctx->init);
    BREAK_IF(err != E_OK);

    memcpy(&ignpower_ctx->ctx->config, &ignpower_ctx->config_default, sizeof(ignpower_config_t));

  } while(0);

  return err;
}

error_t ecu_modules_ignpower_get_default_config(ecu_module_ignpower_t instance, ignpower_config_t *config)
{
  error_t err = E_OK;
  ecu_modules_ignpower_ctx_t *ignpower_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_MODULE_IGNPOWER_MAX || config == NULL, err = E_PARAM);

    ignpower_ctx = &ecu_modules_ignpower_ctx[instance];

    memcpy(config, &ignpower_ctx->config_default, sizeof(ignpower_config_t));

  } while(0);

  return err;
}

error_t ecu_modules_ignpower_configure(ecu_module_ignpower_t instance, const ignpower_config_t *config)
{
  error_t err = E_OK;
  ecu_modules_ignpower_ctx_t *ignpower_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_MODULE_IGNPOWER_MAX || config == NULL, err = E_PARAM);

    ignpower_ctx = &ecu_modules_ignpower_ctx[instance];

    err = ecu_sensors_ckp_register_cb(config->sensor_ckp, ecu_modules_ignpower_ckp_signal_update_cb, ignpower_ctx);
    BREAK_IF(err != E_OK);

    err = ignpower_configure(ignpower_ctx->ctx, config);

  } while(0);

  return err;
}

error_t ecu_modules_ignpower_reset(ecu_module_ignpower_t instance)
{
  error_t err = E_OK;
  ecu_modules_ignpower_ctx_t *ignpower_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_MODULE_IGNPOWER_MAX, err = E_PARAM);

    ignpower_ctx = &ecu_modules_ignpower_ctx[instance];

    err = ignpower_reset(ignpower_ctx->ctx);

  } while(0);

  return err;
}

error_t ecu_modules_ignpower_get_data(ecu_module_ignpower_t instance, ignpower_data_t *data)
{
  error_t err = E_OK;
  ecu_modules_ignpower_ctx_t *ignpower_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_MODULE_IGNPOWER_MAX, err = E_PARAM);
    BREAK_IF_ACTION(data == NULL, err = E_PARAM);

    ignpower_ctx = &ecu_modules_ignpower_ctx[instance];

    err = ignpower_get_data(ignpower_ctx->ctx, data);

  } while(0);

  return err;
}

error_t ecu_modules_ignpower_get_diag(ecu_module_ignpower_t instance, ignpower_diag_t *diag)
{
  error_t err = E_OK;
  ecu_modules_ignpower_ctx_t *ignpower_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_MODULE_IGNPOWER_MAX, err = E_PARAM);
    BREAK_IF_ACTION(diag == NULL, err = E_PARAM);

    ignpower_ctx = &ecu_modules_ignpower_ctx[instance];

    err = ignpower_get_diag(ignpower_ctx->ctx, diag);

  } while(0);

  return err;
}


error_t ecu_modules_ignpower_trigger_operating_signal(ecu_module_ignpower_t instance)
{
  error_t err = E_OK;
  ecu_modules_ignpower_ctx_t *ignpower_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_MODULE_IGNPOWER_MAX, err = E_PARAM);
    ignpower_ctx = &ecu_modules_ignpower_ctx[instance];

    ignpower_components_trigger_operating_signal(ignpower_ctx->ctx);

  } while(0);

  return err;
}

error_t ecu_modules_ignpower_set_operating(ecu_module_ignpower_t instance, bool components_operating)
{
  error_t err = E_OK;
  ecu_modules_ignpower_ctx_t *ignpower_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_MODULE_IGNPOWER_MAX, err = E_PARAM);
    ignpower_ctx = &ecu_modules_ignpower_ctx[instance];

    ignpower_components_set_operating(ignpower_ctx->ctx, components_operating);

  } while(0);

  return err;
}

error_t ecu_modules_ignpower_is_active(ecu_module_ignpower_t instance, bool *ignpower_active)
{
  error_t err = E_OK;
  ecu_modules_ignpower_ctx_t *ignpower_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_MODULE_IGNPOWER_MAX, err = E_PARAM);
    BREAK_IF_ACTION(ignpower_active == NULL, err = E_PARAM);

    ignpower_ctx = &ecu_modules_ignpower_ctx[instance];

    err = ignpower_is_active(ignpower_ctx->ctx, ignpower_active);

  } while(0);

  return err;
}

error_t ecu_modules_ignpower_register_cb(ecu_module_ignpower_t instance, ignpower_signal_update_cb_t callback, void *usrdata)
{
  error_t err = E_OK;
  ecu_modules_ignpower_ctx_t *ignpower_ctx;
  ecu_modules_ignpower_cb_t *cb;

  do {
    BREAK_IF_ACTION(instance >= ECU_MODULE_IGNPOWER_MAX || callback == NULL, err = E_PARAM);

    ignpower_ctx = &ecu_modules_ignpower_ctx[instance];

    err = E_OVERFLOW;

    for(int i = 0; i < ECU_MODULES_IGNPOWER_CALLBACKS_MAX; i++) {
      cb = &ignpower_ctx->signal_update_callbacks[i];
      if(cb->callback == callback && cb->usrdata == usrdata) {
        err = E_OK;
        break;
      } else if(cb->callback == NULL) {
        cb->callback = callback;
        cb->usrdata = usrdata;
        err = E_OK;
        break;
      }
    }

  } while(0);

  return err;
}


ITCM_FUNC static void ecu_modules_ignpower_signal_update_cb(void *usrdata, bool ignpower_active)
{
  ecu_modules_ignpower_ctx_t *ctx = (ecu_modules_ignpower_ctx_t *)usrdata;
  ecu_modules_ignpower_cb_t *cb;

  for(int i = 0; i < ECU_MODULES_IGNPOWER_CALLBACKS_MAX; i++) {
    cb = &ctx->signal_update_callbacks[i];
    if(cb->callback != NULL) {
      cb->callback(cb->usrdata, ignpower_active);
    } else {
      break;
    }
  }
}

ITCM_FUNC static void ecu_modules_ignpower_ckp_signal_update_cb(void *usrdata, const ckp_data_t *data, const ckp_diag_t *diag)
{
  ecu_modules_ignpower_ctx_t *module_ctx = (ecu_modules_ignpower_ctx_t *)usrdata;
  ignpower_ctx_t *ctx;

  do {
    BREAK_IF(module_ctx == NULL);
    ctx = module_ctx->ctx;
    BREAK_IF(ctx == NULL);

    ignpower_ckp_signal_update(ctx, data, diag);

  } while(0);
}
