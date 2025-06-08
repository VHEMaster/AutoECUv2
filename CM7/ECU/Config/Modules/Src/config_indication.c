/*
 * config_indication.c
 *
 *  Created on: Jun 9, 2024
 *      Author: VHEMaster
 */

#include <string.h>
#include "config_indication.h"
#include "config_ignpower.h"
#include "config_ckp.h"
#include "config_extern.h"
#include "compiler.h"

typedef struct {
    indication_config_t config_default;
    indication_init_ctx_t init;
    indication_ctx_t *ctx;
}ecu_modules_indication_ctx_t;

static void ecu_modules_indication_ckp_signal_update_cb(void *usrdata, const ckp_data_t *data, const ckp_diag_t *diag);
static void ecu_modules_indication_ignpower_signal_update_cb(void *usrdata, bool ignpower_active);

static const indication_config_t ecu_modules_indication_config_default = {
    .trigger_source = INDICATION_CONFIG_TRIGGER_CRANKSHAFT,
    .sensor_ckp = ECU_SENSOR_CKP_1,
    .module_ignpower = ECU_MODULE_IGNPOWER_1,
    .ignpower_off_disables = true,

    .ckp_rpm_min = 500.0f,
    .ckp_rpm_max = 10000.0f,

};

static ecu_modules_indication_ctx_t ecu_modules_indication_ctx[ECU_MODULE_INDICATION_MAX] = {
    {
      .init = {

      },
      .config_default = ecu_modules_indication_config_default,
    },
    {
      .init = {

      },
      .config_default = ecu_modules_indication_config_default,
    },
    {
      .init = {

      },
      .config_default = ecu_modules_indication_config_default,
    },
    {
      .init = {

      },
      .config_default = ecu_modules_indication_config_default,
    },
    {
      .init = {

      },
      .config_default = ecu_modules_indication_config_default,
    },
    {
      .init = {

      },
      .config_default = ecu_modules_indication_config_default,
    },
};

static const bool ecu_sensors_indication_enabled_default[ECU_MODULE_INDICATION_MAX] = {
    true,
};

static const ecu_gpio_output_pin_t ecu_sensors_indication_output_pin_default[ECU_MODULE_INDICATION_MAX] = {
    ECU_OUT_PORT1_PIN7,
};

error_t ecu_modules_indication_init(ecu_module_indication_t instance, indication_ctx_t *ctx)
{
  error_t err = E_OK;
  ecu_modules_indication_ctx_t *indication_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_MODULE_INDICATION_MAX || ctx == NULL, err = E_PARAM);

    indication_ctx = &ecu_modules_indication_ctx[instance];
    indication_ctx->ctx = ctx;

    indication_ctx->config_default.enabled = ecu_sensors_indication_enabled_default[instance];
    indication_ctx->config_default.output_pin = ecu_sensors_indication_output_pin_default[instance];

    err = indication_init(indication_ctx->ctx, &indication_ctx->init);
    BREAK_IF(err != E_OK);

    memcpy(&indication_ctx->ctx->config, &indication_ctx->config_default, sizeof(indication_config_t));

  } while(0);

  return err;
}

error_t ecu_modules_indication_get_default_config(ecu_module_indication_t instance, indication_config_t *config)
{
  error_t err = E_OK;
  ecu_modules_indication_ctx_t *indication_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_MODULE_INDICATION_MAX || config == NULL, err = E_PARAM);

    indication_ctx = &ecu_modules_indication_ctx[instance];

    memcpy(config, &indication_ctx->config_default, sizeof(indication_config_t));

  } while(0);

  return err;
}

error_t ecu_modules_indication_configure(ecu_module_indication_t instance, const indication_config_t *config)
{
  error_t err = E_OK;
  ecu_modules_indication_ctx_t *indication_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_MODULE_INDICATION_MAX || config == NULL, err = E_PARAM);

    indication_ctx = &ecu_modules_indication_ctx[instance];

    err = ecu_sensors_ckp_register_cb(config->sensor_ckp, ecu_modules_indication_ckp_signal_update_cb, indication_ctx);
    BREAK_IF(err != E_OK);

    err = ecu_modules_ignpower_register_cb(config->module_ignpower, ecu_modules_indication_ignpower_signal_update_cb, indication_ctx);
    BREAK_IF(err != E_OK);

    err = indication_configure(indication_ctx->ctx, config);

  } while(0);

  return err;
}

error_t ecu_modules_indication_reset(ecu_module_indication_t instance)
{
  error_t err = E_OK;
  ecu_modules_indication_ctx_t *indication_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_MODULE_INDICATION_MAX, err = E_PARAM);

    indication_ctx = &ecu_modules_indication_ctx[instance];

    err = indication_reset(indication_ctx->ctx);

  } while(0);

  return err;
}

error_t ecu_modules_indication_manual_reset(ecu_module_indication_t instance)
{
  error_t err = E_OK;
  ecu_modules_indication_ctx_t *indication_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_MODULE_INDICATION_MAX, err = E_PARAM);

    indication_ctx = &ecu_modules_indication_ctx[instance];

    err = indication_manual_reset(indication_ctx->ctx);

  } while(0);

  return err;
}

error_t ecu_modules_indication_manual_set(ecu_module_indication_t instance, bool manual_enabled)
{
  error_t err = E_OK;
  ecu_modules_indication_ctx_t *indication_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_MODULE_INDICATION_MAX, err = E_PARAM);

    indication_ctx = &ecu_modules_indication_ctx[instance];

    err = indication_manual_set(indication_ctx->ctx, manual_enabled);

  } while(0);

  return err;
}

error_t ecu_modules_indication_force_reset(ecu_module_indication_t instance)
{
  error_t err = E_OK;
  ecu_modules_indication_ctx_t *indication_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_MODULE_INDICATION_MAX, err = E_PARAM);

    indication_ctx = &ecu_modules_indication_ctx[instance];

    err = indication_force_reset(indication_ctx->ctx);

  } while(0);

  return err;
}

error_t ecu_modules_indication_force_set(ecu_module_indication_t instance, bool force_enabled)
{
  error_t err = E_OK;
  ecu_modules_indication_ctx_t *indication_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_MODULE_INDICATION_MAX, err = E_PARAM);

    indication_ctx = &ecu_modules_indication_ctx[instance];

    err = indication_force_set(indication_ctx->ctx, force_enabled);

  } while(0);

  return err;
}

error_t ecu_modules_indication_get_data(ecu_module_indication_t instance, indication_data_t *data)
{
  error_t err = E_OK;
  ecu_modules_indication_ctx_t *indication_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_MODULE_INDICATION_MAX, err = E_PARAM);
    BREAK_IF_ACTION(data == NULL, err = E_PARAM);

    indication_ctx = &ecu_modules_indication_ctx[instance];

    err = indication_get_data(indication_ctx->ctx, data);

  } while(0);

  return err;
}

error_t ecu_modules_indication_get_diag(ecu_module_indication_t instance, indication_diag_t *diag)
{
  error_t err = E_OK;
  ecu_modules_indication_ctx_t *indication_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_MODULE_INDICATION_MAX, err = E_PARAM);
    BREAK_IF_ACTION(diag == NULL, err = E_PARAM);

    indication_ctx = &ecu_modules_indication_ctx[instance];

    err = indication_get_diag(indication_ctx->ctx, diag);

  } while(0);

  return err;
}

ITCM_FUNC static void ecu_modules_indication_ckp_signal_update_cb(void *usrdata, const ckp_data_t *data, const ckp_diag_t *diag)
{
  ecu_modules_indication_ctx_t *module_ctx = (ecu_modules_indication_ctx_t *)usrdata;
  indication_ctx_t *ctx;

  do {
    BREAK_IF(module_ctx == NULL);
    ctx = module_ctx->ctx;
    BREAK_IF(ctx == NULL);

    indication_ckp_signal_update(ctx, data, diag);

  } while(0);
}

ITCM_FUNC static void ecu_modules_indication_ignpower_signal_update_cb(void *usrdata, bool ignpower_active)
{
  ecu_modules_indication_ctx_t *module_ctx = (ecu_modules_indication_ctx_t *)usrdata;
  indication_ctx_t *ctx;

  do {
    BREAK_IF(module_ctx == NULL);
    ctx = module_ctx->ctx;
    BREAK_IF(ctx == NULL);

    indication_ignpower_update(ctx, ignpower_active);

  } while(0);
}
