/*
 * config_fuelpump.c
 *
 *  Created on: Jun 9, 2024
 *      Author: VHEMaster
 */

#include <string.h>
#include "config_fuelpump.h"
#include "config_ignpower.h"
#include "config_ckp.h"
#include "config_extern.h"
#include "compiler.h"

typedef struct {
    fuelpump_config_t config_default;
    fuelpump_init_ctx_t init;
    fuelpump_ctx_t *ctx;
}ecu_modules_fuelpump_ctx_t;

static void ecu_modules_fuelpump_ckp_signal_update_cb(void *usrdata, const ckp_data_t *data, const ckp_diag_t *diag);
static void ecu_modules_fuelpump_ignpower_signal_update_cb(void *usrdata, bool ignpower_active);

static const fuelpump_config_t ecu_modules_fuelpump_config_default = {
    .trigger_source = FUELPUMP_CONFIG_TRIGGER_CRANKSHAFT,
    .sensor_ckp = ECU_SENSOR_CKP_1,
    .module_ignpower = ECU_MODULE_IGNPOWER_1,
    .ckp_trigger = FUELPUMP_CONFIG_CKP_TRIGGER_DETECTED,

    .control_voltage_threshold_off = 1.0f,
    .control_voltage_threshold_on = 6.0f,

    .control_time_delay_off = 100 * TIME_US_IN_MS,
    .control_time_delay_on = 100 * TIME_US_IN_MS,
    .prepumping_time = 1000 * TIME_US_IN_MS,
    .crankshaft_timeout = 500 * TIME_US_IN_MS,
};

static RAM_SECTION ecu_modules_fuelpump_ctx_t ecu_modules_fuelpump_ctx[ECU_MODULE_FUELPUMP_MAX] = {
    {
      .init = {

      },
      .config_default = ecu_modules_fuelpump_config_default,
    },
    {
      .init = {

      },
      .config_default = ecu_modules_fuelpump_config_default,
    },
};

static const bool ecu_sensors_fuelpump_enabled_default[ECU_MODULE_FUELPUMP_MAX] = {
    true,
};

static const ecu_gpio_output_pin_t ecu_sensors_fuelpump_output_drive_pin_default[ECU_MODULE_FUELPUMP_MAX] = {
    ECU_OUT_PORT1_PIN8,
};

static const ecu_gpio_input_pin_t ecu_sensors_fuelpump_input_control_pin_default[ECU_MODULE_FUELPUMP_MAX] = {
    ECU_IN_NONE,
};

static const ecu_gpio_input_pin_t ecu_sensors_fuelpump_input_trigger_pin_default[ECU_MODULE_FUELPUMP_MAX] = {
    ECU_IN_NONE,
};

error_t ecu_modules_fuelpump_init(ecu_module_fuelpump_t instance, fuelpump_ctx_t *ctx)
{
  error_t err = E_OK;
  ecu_modules_fuelpump_ctx_t *fuelpump_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_MODULE_FUELPUMP_MAX || ctx == NULL, err = E_PARAM);

    fuelpump_ctx = &ecu_modules_fuelpump_ctx[instance];
    fuelpump_ctx->ctx = ctx;

    fuelpump_ctx->config_default.enabled = ecu_sensors_fuelpump_enabled_default[instance];
    fuelpump_ctx->config_default.output_drive_pin = ecu_sensors_fuelpump_output_drive_pin_default[instance];
    fuelpump_ctx->config_default.input_control_pin = ecu_sensors_fuelpump_input_control_pin_default[instance];
    fuelpump_ctx->config_default.input_trigger_pin = ecu_sensors_fuelpump_input_trigger_pin_default[instance];

    err = fuelpump_init(fuelpump_ctx->ctx, &fuelpump_ctx->init);
    BREAK_IF(err != E_OK);

    memcpy(&fuelpump_ctx->ctx->config, &fuelpump_ctx->config_default, sizeof(fuelpump_config_t));

  } while(0);

  return err;
}

error_t ecu_modules_fuelpump_get_default_config(ecu_module_fuelpump_t instance, fuelpump_config_t *config)
{
  error_t err = E_OK;
  ecu_modules_fuelpump_ctx_t *fuelpump_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_MODULE_FUELPUMP_MAX || config == NULL, err = E_PARAM);

    fuelpump_ctx = &ecu_modules_fuelpump_ctx[instance];

    memcpy(config, &fuelpump_ctx->config_default, sizeof(fuelpump_config_t));

  } while(0);

  return err;
}

error_t ecu_modules_fuelpump_configure(ecu_module_fuelpump_t instance, const fuelpump_config_t *config)
{
  error_t err = E_OK;
  ecu_modules_fuelpump_ctx_t *fuelpump_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_MODULE_FUELPUMP_MAX || config == NULL, err = E_PARAM);

    fuelpump_ctx = &ecu_modules_fuelpump_ctx[instance];

    err = ecu_sensors_ckp_register_cb(config->sensor_ckp, ecu_modules_fuelpump_ckp_signal_update_cb, fuelpump_ctx);
    BREAK_IF(err != E_OK);

    err = ecu_modules_ignpower_register_cb(config->module_ignpower, ecu_modules_fuelpump_ignpower_signal_update_cb, fuelpump_ctx);
    BREAK_IF(err != E_OK);

    err = fuelpump_configure(fuelpump_ctx->ctx, config);

  } while(0);

  return err;
}

error_t ecu_modules_fuelpump_reset(ecu_module_fuelpump_t instance)
{
  error_t err = E_OK;
  ecu_modules_fuelpump_ctx_t *fuelpump_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_MODULE_FUELPUMP_MAX, err = E_PARAM);

    fuelpump_ctx = &ecu_modules_fuelpump_ctx[instance];

    err = fuelpump_reset(fuelpump_ctx->ctx);

  } while(0);

  return err;
}

error_t ecu_modules_fuelpump_manual_set(ecu_module_fuelpump_t instance, bool manual_enabled)
{
  error_t err = E_OK;
  ecu_modules_fuelpump_ctx_t *fuelpump_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_MODULE_FUELPUMP_MAX, err = E_PARAM);

    fuelpump_ctx = &ecu_modules_fuelpump_ctx[instance];

    err = fuelpump_manual_set(fuelpump_ctx->ctx, manual_enabled);

  } while(0);

  return err;
}

error_t ecu_modules_fuelpump_force_reset(ecu_module_fuelpump_t instance)
{
  error_t err = E_OK;
  ecu_modules_fuelpump_ctx_t *fuelpump_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_MODULE_FUELPUMP_MAX, err = E_PARAM);

    fuelpump_ctx = &ecu_modules_fuelpump_ctx[instance];

    err = fuelpump_force_reset(fuelpump_ctx->ctx);

  } while(0);

  return err;
}

error_t ecu_modules_fuelpump_force_set(ecu_module_fuelpump_t instance, bool force_enabled)
{
  error_t err = E_OK;
  ecu_modules_fuelpump_ctx_t *fuelpump_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_MODULE_FUELPUMP_MAX, err = E_PARAM);

    fuelpump_ctx = &ecu_modules_fuelpump_ctx[instance];

    err = fuelpump_force_set(fuelpump_ctx->ctx, force_enabled);

  } while(0);

  return err;
}

error_t ecu_modules_fuelpump_get_data(ecu_module_fuelpump_t instance, fuelpump_data_t *data)
{
  error_t err = E_OK;
  ecu_modules_fuelpump_ctx_t *fuelpump_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_MODULE_FUELPUMP_MAX, err = E_PARAM);
    BREAK_IF_ACTION(data == NULL, err = E_PARAM);

    fuelpump_ctx = &ecu_modules_fuelpump_ctx[instance];

    err = fuelpump_get_data(fuelpump_ctx->ctx, data);

  } while(0);

  return err;
}

error_t ecu_modules_fuelpump_get_diag(ecu_module_fuelpump_t instance, fuelpump_diag_t *diag)
{
  error_t err = E_OK;
  ecu_modules_fuelpump_ctx_t *fuelpump_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_MODULE_FUELPUMP_MAX, err = E_PARAM);
    BREAK_IF_ACTION(diag == NULL, err = E_PARAM);

    fuelpump_ctx = &ecu_modules_fuelpump_ctx[instance];

    err = fuelpump_get_diag(fuelpump_ctx->ctx, diag);

  } while(0);

  return err;
}

ITCM_FUNC static void ecu_modules_fuelpump_ckp_signal_update_cb(void *usrdata, const ckp_data_t *data, const ckp_diag_t *diag)
{
  ecu_modules_fuelpump_ctx_t *module_ctx = (ecu_modules_fuelpump_ctx_t *)usrdata;
  fuelpump_ctx_t *ctx;

  do {
    BREAK_IF(module_ctx == NULL);
    ctx = module_ctx->ctx;
    BREAK_IF(ctx == NULL);

    fuelpump_ckp_signal_update(ctx, data, diag);

  } while(0);
}

ITCM_FUNC static void ecu_modules_fuelpump_ignpower_signal_update_cb(void *usrdata, bool ignpower_active)
{
  ecu_modules_fuelpump_ctx_t *module_ctx = (ecu_modules_fuelpump_ctx_t *)usrdata;
  fuelpump_ctx_t *ctx;

  do {
    BREAK_IF(module_ctx == NULL);
    ctx = module_ctx->ctx;
    BREAK_IF(ctx == NULL);

    fuelpump_ignpower_update(ctx, ignpower_active);

  } while(0);
}
