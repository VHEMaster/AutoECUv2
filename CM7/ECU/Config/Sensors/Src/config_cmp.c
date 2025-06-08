/*
 * config_cmp.c
 *
 *  Created on: Apr 17, 2024
 *      Author: VHEMaster
 */

#include <string.h>
#include "config_cmp.h"
#include "config_ckp.h"
#include "config_extern.h"
#include "compiler.h"

static error_t ecu_sensors_cmp_ckp_update_req_cb(void *usrdata, ckp_req_t *req, ckp_data_t *data);
static void ecu_sensors_cmp_signal_update_cb(void *usrdata, const cmp_data_t *data, const cmp_diag_t *diag);

typedef struct {
    cmp_signal_update_cb_t callback;
    void *usrdata;
}ecu_sensors_cmp_cb_t;

typedef struct {
    cmp_config_t config_default;
    cmp_init_ctx_t init;
    cmp_ctx_t *ctx;
    ecu_sensors_cmp_cb_t signal_update_callbacks[ECU_SENSORS_CMP_CALLBACKS_MAX];
}ecu_sensors_cmp_ctx_t;

static const cmp_config_t ecu_sensors_cmp_config_default = {
    .desync_on_error = true,
    .signal_ref_type = CMP_CONFIG_SIGNAL_REF_TYPE_SINGLEPULSE,
    .boot_time = 100 * TIME_US_IN_MS,
    .signal_ref_types_config = {
        .singlepulse = {
            .pulse_edge_pos_min = -140,
            .pulse_edge_pos_max = -60,
            .pulse_width_min = 50,
            .pulse_width_max = 60,
            .polarity = CMP_CONFIG_SIGNAL_POLARITY_ACTIVE_LOW,
        },
    },
};

static const bool ecu_sensors_cmp_enabled_default[ECU_SENSOR_CMP_MAX] = {
    true,
};

static const ecu_gpio_input_pin_t ecu_sensors_cmp_input_pin_default[ECU_SENSOR_CMP_MAX] = {
    ECU_IN_PORT2_SENT2,
};

static ecu_sensors_cmp_ctx_t ecu_sensors_cmp_ctx[ECU_SENSOR_CMP_MAX] = {
    {
      .init = {
          .ckp_instance_index = CKP_INSTANCE_1,
          .instance_index = CMP_INSTANCE_1,
          .signal_update_cb = ecu_sensors_cmp_signal_update_cb,
          .signal_update_usrdata = &ecu_sensors_cmp_ctx[0],
          .ckp_update_req_cb = ecu_sensors_cmp_ckp_update_req_cb,
          .ckp_update_usrdata = &ecu_sensors_cmp_ctx[ECU_SENSOR_CMP_1],
      },
      .config_default = ecu_sensors_cmp_config_default,
    },
};

error_t ecu_sensors_cmp_init(ecu_sensor_cmp_t instance, cmp_ctx_t *ctx)
{
  error_t err = E_OK;
  ecu_sensors_cmp_ctx_t *cmp_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_SENSOR_CMP_MAX || ctx == NULL, err = E_PARAM);

    cmp_ctx = &ecu_sensors_cmp_ctx[instance];
    cmp_ctx->ctx = ctx;

    cmp_ctx->config_default.enabled = ecu_sensors_cmp_enabled_default[instance];
    cmp_ctx->config_default.input_pin = ecu_sensors_cmp_input_pin_default[instance];

    err = ecu_sensors_ckp_register_cb(ctx->init.ckp_instance_index, cmp_ckp_signal_update, cmp_ctx->ctx);
    BREAK_IF(err != E_OK);

    err = cmp_init(cmp_ctx->ctx, &cmp_ctx->init);
    BREAK_IF(err != E_OK);


    memcpy(&cmp_ctx->ctx->config, &cmp_ctx->config_default, sizeof(cmp_config_t));

  } while(0);

  return err;
}

error_t ecu_sensors_cmp_get_default_config(ecu_sensor_cmp_t instance, cmp_config_t *config)
{
  error_t err = E_OK;
  ecu_sensors_cmp_ctx_t *cmp_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_SENSOR_CMP_MAX || config == NULL, err = E_PARAM);

    cmp_ctx = &ecu_sensors_cmp_ctx[instance];

    memcpy(config, &cmp_ctx->config_default, sizeof(cmp_config_t));

  } while(0);

  return err;
}

error_t ecu_sensors_cmp_configure(ecu_sensor_cmp_t instance, const cmp_config_t *config)
{
  error_t err = E_OK;
  ecu_sensors_cmp_ctx_t *cmp_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_SENSOR_CMP_MAX || config == NULL, err = E_PARAM);

    cmp_ctx = &ecu_sensors_cmp_ctx[instance];

    err = cmp_configure(cmp_ctx->ctx, config);

  } while(0);

  return err;
}

error_t ecu_sensors_cmp_reset(ecu_sensor_cmp_t instance)
{
  error_t err = E_OK;
  ecu_sensors_cmp_ctx_t *cmp_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_SENSOR_CMP_MAX, err = E_PARAM);

    cmp_ctx = &ecu_sensors_cmp_ctx[instance];

    err = cmp_reset(cmp_ctx->ctx);

  } while(0);

  return err;
}

error_t ecu_sensors_cmp_get_value(ecu_sensor_cmp_t instance, cmp_data_t *data)
{
  error_t err = E_OK;
  ecu_sensors_cmp_ctx_t *cmp_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_SENSOR_CMP_MAX, err = E_PARAM);
    BREAK_IF_ACTION(data == NULL, err = E_PARAM);

    cmp_ctx = &ecu_sensors_cmp_ctx[instance];

    err = cmp_get_value(cmp_ctx->ctx, data);

  } while(0);

  return err;
}

error_t ecu_sensors_cmp_get_diag(ecu_sensor_cmp_t instance, cmp_diag_t *diag)
{
  error_t err = E_OK;
  ecu_sensors_cmp_ctx_t *cmp_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_SENSOR_CMP_MAX, err = E_PARAM);
    BREAK_IF_ACTION(diag == NULL, err = E_PARAM);

    cmp_ctx = &ecu_sensors_cmp_ctx[instance];

    err = cmp_get_diag(cmp_ctx->ctx, diag);

  } while(0);

  return err;
}

error_t ecu_sensors_cmp_register_cb(ecu_sensor_cmp_t instance, cmp_signal_update_cb_t callback, void *usrdata)
{
  error_t err = E_OK;
  ecu_sensors_cmp_ctx_t *cmp_ctx;
  ecu_sensors_cmp_cb_t *cb;

  do {
    BREAK_IF_ACTION(instance >= ECU_SENSOR_CMP_MAX || callback == NULL, err = E_PARAM);

    cmp_ctx = &ecu_sensors_cmp_ctx[instance];

    err = E_OVERFLOW;

    for(int i = 0; i < ECU_SENSORS_CMP_CALLBACKS_MAX; i++) {
      cb = &cmp_ctx->signal_update_callbacks[i];
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

ITCM_FUNC static error_t ecu_sensors_cmp_ckp_update_req_cb(void *usrdata, ckp_req_t *req, ckp_data_t *data)
{
  error_t err = E_OK;
  ecu_sensors_cmp_ctx_t *cmp_ctx = (ecu_sensors_cmp_ctx_t *)usrdata;

  do {
    BREAK_IF_ACTION(cmp_ctx == NULL, err = E_PARAM);

    err = ecu_sensors_ckp_calculate_current_position(cmp_ctx->init.ckp_instance_index, req, data);

  } while(0);

  return err;
}

ITCM_FUNC static void ecu_sensors_cmp_signal_update_cb(void *usrdata, const cmp_data_t *data, const cmp_diag_t *diag)
{
  ecu_sensors_cmp_ctx_t *ctx = (ecu_sensors_cmp_ctx_t *)usrdata;
  ecu_sensors_cmp_cb_t *cb;

  for(int i = 0; i < ECU_SENSORS_CMP_CALLBACKS_MAX; i++) {
    cb = &ctx->signal_update_callbacks[i];
    if(cb->callback != NULL) {
      cb->callback(cb->usrdata, data, diag);
    } else {
      break;
    }
  }
}
