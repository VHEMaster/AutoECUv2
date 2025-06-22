/*
 * config_ckp.c
 *
 *  Created on: Apr 17, 2024
 *      Author: VHEMaster
 */

#include <string.h>
#include "config_ckp.h"
#include "config_extern.h"
#include "compiler.h"

static void ecu_sensors_ckp_signal_update_cb(void *usrdata, const ckp_data_t *data, const ckp_diag_t *diag);

typedef struct {
    ckp_signal_update_cb_t callback;
    void *usrdata;
}ecu_sensors_ckp_cb_t;

typedef struct {
    ckp_config_t config_default;
    ckp_init_ctx_t init;
    ckp_ctx_t *ctx;
    ecu_sensors_ckp_cb_t signal_update_callbacks[ECU_SENSORS_CKP_CALLBACKS_MAX];
}ecu_sensors_ckp_ctx_t;

static const ckp_config_t ecu_sensors_ckp_config_default = {
    .desync_on_error = true,
    .signal_ref_type = CKP_CONFIG_SIGNAL_REF_TYPE_REGULAR_60_2,
    .boot_time = 100 * TIME_US_IN_MS,
};

static const bool ecu_sensors_ckp_enabled_default[ECU_SENSOR_CKP_MAX] = {
    true,
};

static const ecu_gpio_input_pin_t ecu_sensors_ckp_input_pin_default[ECU_SENSOR_CKP_MAX] = {
    ECU_IN_PORT1_VRS,
};


static RAM_SECTION ecu_sensors_ckp_ctx_t ecu_sensors_ckp_ctx[ECU_SENSOR_CKP_MAX] = {
    {
      .init = {
          .instance_index = CKP_INSTANCE_1,
          .signal_update_cb = ecu_sensors_ckp_signal_update_cb,
          .signal_update_usrdata = &ecu_sensors_ckp_ctx[0],
      },
      .config_default = ecu_sensors_ckp_config_default,
    },
};

error_t ecu_sensors_ckp_init(ecu_sensor_ckp_t instance, ckp_ctx_t *ctx)
{
  error_t err = E_OK;
  ecu_sensors_ckp_ctx_t *ckp_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_SENSOR_CKP_MAX || ctx == NULL, err = E_PARAM);

    ckp_ctx = &ecu_sensors_ckp_ctx[instance];
    ckp_ctx->ctx = ctx;

    ckp_ctx->config_default.enabled = ecu_sensors_ckp_enabled_default[instance];
    ckp_ctx->config_default.input_pin = ecu_sensors_ckp_input_pin_default[instance];

    err = ckp_init(ckp_ctx->ctx, &ckp_ctx->init);
    BREAK_IF(err != E_OK);

    memcpy(&ckp_ctx->ctx->config, &ckp_ctx->config_default, sizeof(ckp_config_t));

  } while(0);

  return err;
}

error_t ecu_sensors_ckp_get_default_config(ecu_sensor_ckp_t instance, ckp_config_t *config)
{
  error_t err = E_OK;
  ecu_sensors_ckp_ctx_t *ckp_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_SENSOR_CKP_MAX || config == NULL, err = E_PARAM);

    ckp_ctx = &ecu_sensors_ckp_ctx[instance];

    memcpy(config, &ckp_ctx->config_default, sizeof(ckp_config_t));

  } while(0);

  return err;
}

error_t ecu_sensors_ckp_configure(ecu_sensor_ckp_t instance, const ckp_config_t *config)
{
  error_t err = E_OK;
  ecu_sensors_ckp_ctx_t *ckp_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_SENSOR_CKP_MAX || config == NULL, err = E_PARAM);

    ckp_ctx = &ecu_sensors_ckp_ctx[instance];

    err = ckp_configure(ckp_ctx->ctx, config);

  } while(0);

  return err;
}

error_t ecu_sensors_ckp_reset(ecu_sensor_ckp_t instance)
{
  error_t err = E_OK;
  ecu_sensors_ckp_ctx_t *ckp_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_SENSOR_CKP_MAX, err = E_PARAM);

    ckp_ctx = &ecu_sensors_ckp_ctx[instance];

    err = ckp_reset(ckp_ctx->ctx);

  } while(0);

  return err;
}

error_t ecu_sensors_ckp_get_value(ecu_sensor_ckp_t instance, ckp_data_t *data)
{
  error_t err = E_OK;
  ecu_sensors_ckp_ctx_t *ckp_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_SENSOR_CKP_MAX, err = E_PARAM);
    BREAK_IF_ACTION(data == NULL, err = E_PARAM);

    ckp_ctx = &ecu_sensors_ckp_ctx[instance];

    err = ckp_get_value(ckp_ctx->ctx, data);

  } while(0);

  return err;
}

error_t ecu_sensors_ckp_get_diag(ecu_sensor_ckp_t instance, ckp_diag_t *diag)
{
  error_t err = E_OK;
  ecu_sensors_ckp_ctx_t *ckp_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_SENSOR_CKP_MAX, err = E_PARAM);
    BREAK_IF_ACTION(diag == NULL, err = E_PARAM);

    ckp_ctx = &ecu_sensors_ckp_ctx[instance];

    err = ckp_get_diag(ckp_ctx->ctx, diag);

  } while(0);

  return err;
}

ITCM_FUNC error_t ecu_sensors_ckp_calculate_current_position(ecu_sensor_ckp_t instance, ckp_req_t *req, ckp_data_t *data)
{
  error_t err = E_OK;
  ecu_sensors_ckp_ctx_t *ckp_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_SENSOR_CKP_MAX, err = E_PARAM);
    BREAK_IF_ACTION(data == NULL, err = E_PARAM);

    ckp_ctx = &ecu_sensors_ckp_ctx[instance];

    err = ckp_calculate_current_position(ckp_ctx->ctx, req, data);

  } while(0);

  return err;
}

error_t ecu_sensors_ckp_register_cb(ecu_sensor_ckp_t instance, ckp_signal_update_cb_t callback, void *usrdata)
{
  error_t err = E_OK;
  ecu_sensors_ckp_ctx_t *ckp_ctx;
  ecu_sensors_ckp_cb_t *cb;

  do {
    BREAK_IF_ACTION(instance >= ECU_SENSOR_CKP_MAX || callback == NULL, err = E_PARAM);

    ckp_ctx = &ecu_sensors_ckp_ctx[instance];

    err = E_OVERFLOW;

    for(int i = 0; i < ECU_SENSORS_CKP_CALLBACKS_MAX; i++) {
      cb = &ckp_ctx->signal_update_callbacks[i];
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

ITCM_FUNC static void ecu_sensors_ckp_signal_update_cb(void *usrdata, const ckp_data_t *data, const ckp_diag_t *diag)
{
  ecu_sensors_ckp_ctx_t *ctx = (ecu_sensors_ckp_ctx_t *)usrdata;
  ecu_sensors_ckp_cb_t *cb;

  for(int i = 0; i < ECU_SENSORS_CKP_CALLBACKS_MAX; i++) {
    cb = &ctx->signal_update_callbacks[i];
    if(cb->callback != NULL) {
      cb->callback(cb->usrdata, data, diag);
    } else {
      break;
    }
  }
}
