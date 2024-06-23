/*
 * config_ignition.c
 *
 *  Created on: Jun 23, 2024
 *      Author: VHEMaster
 */

#include <string.h>
#include "config_ignition.h"
#include "config_extern.h"
#include "compiler.h"

static void ecu_modules_ignition_signal_update_cb(void *usrdata, bool ignition_active);

typedef struct {
    ignition_signal_update_cb_t callback;
    void *usrdata;
}ecu_modules_ignition_cb_t;

typedef struct {
    ignition_config_t config_default;
    ignition_init_ctx_t init;
    ignition_ctx_t *ctx;
    ecu_modules_ignition_cb_t signal_update_callbacks[ECU_MODULES_IGNITION_CALLBACKS_MAX];
}ecu_modules_ignition_ctx_t;

static const ignition_config_t ecu_modules_ignition_config_default = {
    .input_debounce_off_to_on = 100 * TIME_US_IN_MS,
    .input_debounce_on_to_off = 100 * TIME_US_IN_MS,
    .input_gpio_inverted = false,
};

static ecu_modules_ignition_ctx_t ecu_modules_ignition_ctx[ECU_MODULE_IGNITION_MAX] = {
    {
      .init = {
          .signal_update_cb = ecu_modules_ignition_signal_update_cb,
          .signal_update_usrdata = &ecu_modules_ignition_ctx[0],
      },
      .config_default = ecu_modules_ignition_config_default,
    },
};

static const bool ecu_modules_ignition_enabled_default[ECU_MODULE_IGNITION_MAX] = {
    true,
};

static const ecu_gpio_input_pin_t ecu_modules_ignition_input_pin_default[ECU_MODULE_IGNITION_MAX] = {
    ECU_IN_PORT2_PIN15,
};

error_t ecu_modules_ignition_init(ecu_module_ignition_t instance, ignition_ctx_t *ctx)
{
  error_t err = E_OK;
  ecu_modules_ignition_ctx_t *ignition_ctx;

  do {
    BREAK_IF_ACTION(instance <= ECU_MODULE_NONE || instance >= ECU_MODULE_IGNITION_MAX || ctx == NULL, err = E_PARAM);

    ignition_ctx = &ecu_modules_ignition_ctx[instance];
    ignition_ctx->ctx = ctx;

    ignition_ctx->config_default.enabled = ecu_modules_ignition_enabled_default[instance];
    ignition_ctx->config_default.input_pin = ecu_modules_ignition_input_pin_default[instance];

    err = ignition_init(ignition_ctx->ctx, &ignition_ctx->init);
    BREAK_IF(err != E_OK);

    memcpy(&ignition_ctx->ctx->config, &ignition_ctx->config_default, sizeof(ignition_config_t));

  } while(0);

  return err;
}

error_t ecu_modules_ignition_get_default_config(ecu_module_ignition_t instance, ignition_config_t *config)
{
  error_t err = E_OK;
  ecu_modules_ignition_ctx_t *ignition_ctx;

  do {
    BREAK_IF_ACTION(instance <= ECU_MODULE_NONE || instance >= ECU_MODULE_IGNITION_MAX || config == NULL, err = E_PARAM);

    ignition_ctx = &ecu_modules_ignition_ctx[instance];

    memcpy(config, &ignition_ctx->config_default, sizeof(ignition_config_t));

  } while(0);

  return err;
}

error_t ecu_modules_ignition_configure(ecu_module_ignition_t instance, const ignition_config_t *config)
{
  error_t err = E_OK;
  ecu_modules_ignition_ctx_t *ignition_ctx;

  do {
    BREAK_IF_ACTION(instance <= ECU_MODULE_NONE || instance >= ECU_MODULE_IGNITION_MAX || config == NULL, err = E_PARAM);

    ignition_ctx = &ecu_modules_ignition_ctx[instance];

    err = ignition_configure(ignition_ctx->ctx, config);

  } while(0);

  return err;
}

error_t ecu_modules_ignition_reset(ecu_module_ignition_t instance)
{
  error_t err = E_OK;
  ecu_modules_ignition_ctx_t *ignition_ctx;

  do {
    BREAK_IF_ACTION(instance <= ECU_MODULE_NONE || instance >= ECU_MODULE_IGNITION_MAX, err = E_PARAM);

    ignition_ctx = &ecu_modules_ignition_ctx[instance];

    err = ignition_reset(ignition_ctx->ctx);

  } while(0);

  return err;
}

error_t ecu_modules_ignition_is_active(ecu_module_ignition_t instance, bool *ignition_active)
{
  error_t err = E_OK;
  ecu_modules_ignition_ctx_t *ignition_ctx;

  do {
    BREAK_IF_ACTION(instance <= ECU_MODULE_NONE || instance >= ECU_MODULE_IGNITION_MAX, err = E_PARAM);
    BREAK_IF_ACTION(ignition_active == NULL, err = E_PARAM);

    ignition_ctx = &ecu_modules_ignition_ctx[instance];

    err = ignition_is_active(ignition_ctx->ctx, ignition_active);

  } while(0);

  return err;
}

error_t ecu_modules_ignition_register_cb(ecu_module_ignition_t instance, ignition_signal_update_cb_t callback, void *usrdata)
{
  error_t err = E_OK;
  ecu_modules_ignition_ctx_t *ignition_ctx;
  ecu_modules_ignition_cb_t *cb;

  do {
    BREAK_IF_ACTION(instance <= ECU_MODULE_NONE || instance >= ECU_MODULE_IGNITION_MAX || callback == NULL, err = E_PARAM);

    ignition_ctx = &ecu_modules_ignition_ctx[instance];

    err = E_OVERFLOW;

    for(int i = 0; i < ECU_MODULES_IGNITION_CALLBACKS_MAX; i++) {
      cb = &ignition_ctx->signal_update_callbacks[i];
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


ITCM_FUNC static void ecu_modules_ignition_signal_update_cb(void *usrdata, bool ignition_active)
{
  ecu_modules_ignition_ctx_t *ctx = (ecu_modules_ignition_ctx_t *)usrdata;
  ecu_modules_ignition_cb_t *cb;

  for(int i = 0; i < ECU_MODULES_IGNITION_CALLBACKS_MAX; i++) {
    cb = &ctx->signal_update_callbacks[i];
    if(cb->callback != NULL) {
      cb->callback(cb->usrdata, ignition_active);
    } else {
      break;
    }
  }
}
