/*
 * config_coolingfan.c
 *
 *  Created on: Jun 9, 2024
 *      Author: VHEMaster
 */

#include <string.h>
#include "config_coolingfan.h"
#include "config_ignpower.h"
#include "config_ckp.h"
#include "config_extern.h"
#include "compiler.h"

typedef struct {
    coolingfan_config_t config_default;
    coolingfan_init_ctx_t init;
    coolingfan_ctx_t *ctx;
}ecu_modules_coolingfan_ctx_t;

static void ecu_modules_coolingfan_ckp_signal_update_cb(void *usrdata, const ckp_data_t *data, const ckp_diag_t *diag);
static void ecu_modules_coolingfan_ignpower_signal_update_cb(void *usrdata, bool ignpower_active);

static const coolingfan_config_t ecu_modules_coolingfan_config_default = {
    .sensor_ckp = ECU_SENSOR_CKP_1,
    .module_ignpower = ECU_MODULE_IGNPOWER_1,
    .ckp_trigger = COOLINGFAN_CONFIG_CKP_TRIGGER_DETECTED,

    .allow_force_when_stopped = false,
    .crankshaft_rpm_min = 600.0f,
    .control_voltage_threshold_off = 5.0f,
    .control_voltage_threshold_on = 6.0f,

    .control_time_delay_off = 6000 * TIME_US_IN_MS,
    .control_time_delay_on = 500 * TIME_US_IN_MS,
    .crankshaft_timeout = 5000 * TIME_US_IN_MS,
};

static ecu_modules_coolingfan_ctx_t ecu_modules_coolingfan_ctx[ECU_MODULE_COOLINGFAN_MAX] = {
    {
      .init = {

      },
      .config_default = ecu_modules_coolingfan_config_default,
    },
    {
      .init = {

      },
      .config_default = ecu_modules_coolingfan_config_default,
    },
};

static const bool ecu_sensors_coolingfan_enabled_default[ECU_MODULE_COOLINGFAN_MAX] = {
    true,
    true,
};

static const ecu_gpio_output_pin_t ecu_sensors_coolingfan_output_drive_pin_default[ECU_MODULE_COOLINGFAN_MAX] = {
    ECU_OUT_PORT1_PIN13,
    ECU_OUT_PORT1_PIN14,
};

static const ecu_gpio_input_pin_t ecu_sensors_coolingfan_input_control_pin_default[ECU_MODULE_COOLINGFAN_MAX] = {
    ECU_IN_NONE,
    ECU_IN_NONE,
};

static const ecu_gpio_input_pin_t ecu_sensors_coolingfan_input_force_pin_default[ECU_MODULE_COOLINGFAN_MAX] = {
    ECU_IN_NONE,
    ECU_IN_PORT2_PIN13,
};

error_t ecu_modules_coolingfan_init(ecu_module_coolingfan_t instance, coolingfan_ctx_t *ctx)
{
  error_t err = E_OK;
  ecu_modules_coolingfan_ctx_t *coolingfan_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_MODULE_COOLINGFAN_MAX || ctx == NULL, err = E_PARAM);

    coolingfan_ctx = &ecu_modules_coolingfan_ctx[instance];
    coolingfan_ctx->ctx = ctx;

    coolingfan_ctx->config_default.enabled = ecu_sensors_coolingfan_enabled_default[instance];
    coolingfan_ctx->config_default.output_drive_pin = ecu_sensors_coolingfan_output_drive_pin_default[instance];
    coolingfan_ctx->config_default.input_control_pin = ecu_sensors_coolingfan_input_control_pin_default[instance];
    coolingfan_ctx->config_default.input_force_pin = ecu_sensors_coolingfan_input_force_pin_default[instance];

    err = coolingfan_init(coolingfan_ctx->ctx, &coolingfan_ctx->init);
    BREAK_IF(err != E_OK);

    memcpy(&coolingfan_ctx->ctx->config, &coolingfan_ctx->config_default, sizeof(coolingfan_config_t));

  } while(0);

  return err;
}

error_t ecu_modules_coolingfan_get_default_config(ecu_module_coolingfan_t instance, coolingfan_config_t *config)
{
  error_t err = E_OK;
  ecu_modules_coolingfan_ctx_t *coolingfan_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_MODULE_COOLINGFAN_MAX || config == NULL, err = E_PARAM);

    coolingfan_ctx = &ecu_modules_coolingfan_ctx[instance];

    memcpy(config, &coolingfan_ctx->config_default, sizeof(coolingfan_config_t));

  } while(0);

  return err;
}

error_t ecu_modules_coolingfan_configure(ecu_module_coolingfan_t instance, const coolingfan_config_t *config)
{
  error_t err = E_OK;
  ecu_modules_coolingfan_ctx_t *coolingfan_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_MODULE_COOLINGFAN_MAX || config == NULL, err = E_PARAM);

    coolingfan_ctx = &ecu_modules_coolingfan_ctx[instance];

    err = ecu_sensors_ckp_register_cb(config->sensor_ckp, ecu_modules_coolingfan_ckp_signal_update_cb, coolingfan_ctx);
    BREAK_IF(err != E_OK);

    err = ecu_modules_ignpower_register_cb(config->module_ignpower, ecu_modules_coolingfan_ignpower_signal_update_cb, coolingfan_ctx);
    BREAK_IF(err != E_OK);

    err = coolingfan_configure(coolingfan_ctx->ctx, config);

  } while(0);

  return err;
}

error_t ecu_modules_coolingfan_reset(ecu_module_coolingfan_t instance)
{
  error_t err = E_OK;
  ecu_modules_coolingfan_ctx_t *coolingfan_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_MODULE_COOLINGFAN_MAX, err = E_PARAM);

    coolingfan_ctx = &ecu_modules_coolingfan_ctx[instance];

    err = coolingfan_reset(coolingfan_ctx->ctx);

  } while(0);

  return err;
}

error_t ecu_modules_coolingfan_activate_trigger(ecu_module_coolingfan_t instance, bool activate_trigger)
{
  error_t err = E_OK;
  ecu_modules_coolingfan_ctx_t *coolingfan_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_MODULE_COOLINGFAN_MAX, err = E_PARAM);

    coolingfan_ctx = &ecu_modules_coolingfan_ctx[instance];

    err = coolingfan_activate_trigger(coolingfan_ctx->ctx, activate_trigger);

  } while(0);

  return err;
}

error_t ecu_modules_coolingfan_emergency_trigger(ecu_module_coolingfan_t instance, bool emergency_trigger)
{
  error_t err = E_OK;
  ecu_modules_coolingfan_ctx_t *coolingfan_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_MODULE_COOLINGFAN_MAX, err = E_PARAM);

    coolingfan_ctx = &ecu_modules_coolingfan_ctx[instance];

    err = coolingfan_emergency_trigger(coolingfan_ctx->ctx, emergency_trigger);

  } while(0);

  return err;
}

error_t ecu_modules_coolingfan_force_reset(ecu_module_coolingfan_t instance)
{
  error_t err = E_OK;
  ecu_modules_coolingfan_ctx_t *coolingfan_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_MODULE_COOLINGFAN_MAX, err = E_PARAM);

    coolingfan_ctx = &ecu_modules_coolingfan_ctx[instance];

    err = coolingfan_force_reset(coolingfan_ctx->ctx);

  } while(0);

  return err;
}

error_t ecu_modules_coolingfan_force_set(ecu_module_coolingfan_t instance, bool force_enabled)
{
  error_t err = E_OK;
  ecu_modules_coolingfan_ctx_t *coolingfan_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_MODULE_COOLINGFAN_MAX, err = E_PARAM);

    coolingfan_ctx = &ecu_modules_coolingfan_ctx[instance];

    err = coolingfan_force_set(coolingfan_ctx->ctx, force_enabled);

  } while(0);

  return err;
}

error_t ecu_modules_coolingfan_get_data(ecu_module_coolingfan_t instance, coolingfan_data_t *data)
{
  error_t err = E_OK;
  ecu_modules_coolingfan_ctx_t *coolingfan_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_MODULE_COOLINGFAN_MAX, err = E_PARAM);
    BREAK_IF_ACTION(data == NULL, err = E_PARAM);

    coolingfan_ctx = &ecu_modules_coolingfan_ctx[instance];

    err = coolingfan_get_data(coolingfan_ctx->ctx, data);

  } while(0);

  return err;
}

error_t ecu_modules_coolingfan_get_diag(ecu_module_coolingfan_t instance, coolingfan_diag_t *diag)
{
  error_t err = E_OK;
  ecu_modules_coolingfan_ctx_t *coolingfan_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_MODULE_COOLINGFAN_MAX, err = E_PARAM);
    BREAK_IF_ACTION(diag == NULL, err = E_PARAM);

    coolingfan_ctx = &ecu_modules_coolingfan_ctx[instance];

    err = coolingfan_get_diag(coolingfan_ctx->ctx, diag);

  } while(0);

  return err;
}

ITCM_FUNC static void ecu_modules_coolingfan_ckp_signal_update_cb(void *usrdata, const ckp_data_t *data, const ckp_diag_t *diag)
{
  ecu_modules_coolingfan_ctx_t *module_ctx = (ecu_modules_coolingfan_ctx_t *)usrdata;
  coolingfan_ctx_t *ctx;

  do {
    BREAK_IF(module_ctx == NULL);
    ctx = module_ctx->ctx;
    BREAK_IF(ctx == NULL);

    coolingfan_ckp_signal_update(ctx, data, diag);

  } while(0);
}

ITCM_FUNC static void ecu_modules_coolingfan_ignpower_signal_update_cb(void *usrdata, bool ignpower_active)
{
  ecu_modules_coolingfan_ctx_t *module_ctx = (ecu_modules_coolingfan_ctx_t *)usrdata;
  coolingfan_ctx_t *ctx;

  do {
    BREAK_IF(module_ctx == NULL);
    ctx = module_ctx->ctx;
    BREAK_IF(ctx == NULL);

    coolingfan_ignpower_update(ctx, ignpower_active);

  } while(0);
}
