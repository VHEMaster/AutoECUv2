/*
 * config_timing.c
 *
 *  Created on: Jun 6, 2024
 *      Author: VHEMaster
 */

#include <string.h>
#include "config_timing.h"
#include "config_extern.h"
#include "config_ckp.h"
#include "config_cmp.h"
#include "compiler.h"

static void ecu_modules_timing_ckp_signal_update_cb(void *usrdata, const ckp_data_t *data, const ckp_diag_t *diag);
static void ecu_modules_timing_cmp_signal_update_cb(void *usrdata, const cmp_data_t *data, const cmp_diag_t *diag);
static void ecu_modules_timing_signal_update_cb(void *usrdata, const timing_data_t *data, const timing_diag_t *diag);

typedef struct ecu_modules_timing_ctx_tag ecu_modules_timing_ctx_t;

typedef struct {
    timing_signal_update_cb_t callback;
    void *usrdata;
}ecu_modules_timing_cb_t;

typedef struct {
    ecu_sensor_ckp_t ckp_instance;
    ecu_modules_timing_ctx_t *module_ctx;
}ecu_modules_timing_ckp_cb_ctx_t;

typedef struct {
    ecu_sensor_cmp_t cmp_instance;
    ecu_modules_timing_ctx_t *module_ctx;
}ecu_modules_timing_cmp_cb_ctx_t;

typedef struct ecu_modules_timing_ctx_tag {
    timing_config_t config_default;
    timing_init_ctx_t init;
    timing_ctx_t *ctx;
    ecu_modules_timing_ckp_cb_ctx_t ckp_cb_ctx;
    ecu_modules_timing_cmp_cb_ctx_t cmp_cb_ctx[ECU_SENSOR_CMP_MAX];
    ecu_modules_timing_cb_t signal_update_callbacks[ECU_MODULES_TIMING_CALLBACKS_MAX];
}ecu_modules_timing_ctx_t;

static const timing_config_t ecu_modules_timing_config_default = {
    .crankshaft = {
        .offset = 0.0f,
    },
    .camshafts = {
        {
          .enabled = true,
          .use_for_phased_sync = true,
          .pos_relative = -105.8f,
          .pos_min = -3.0f,
          .pos_max = 3.0f,
        }, //ECU_SENSOR_CMP_1
    },
    .phased_syncmode = TIMING_CONFIG_PHASEDSYNCMODE_CAMSHAFT,
    .phased_only = true,
};

static ecu_modules_timing_ctx_t ecu_modules_timing_ctx[ECU_MODULE_TIMING_MAX] = {
    {
      .init = {
          .ckp_instance = ECU_SENSOR_CKP_1,
          .signal_update_cb = ecu_modules_timing_signal_update_cb,
          .signal_update_usrdata = &ecu_modules_timing_ctx[0],
      },
      .config_default = ecu_modules_timing_config_default,
    },
};



error_t ecu_modules_timing_init(ecu_module_timing_t instance, timing_ctx_t *ctx)
{
  error_t err = E_OK;
  ecu_modules_timing_ctx_t *timing_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_MODULE_TIMING_MAX || ctx == NULL, err = E_PARAM);

    timing_ctx = &ecu_modules_timing_ctx[instance];
    timing_ctx->ctx = ctx;

    timing_ctx->ckp_cb_ctx.ckp_instance = timing_ctx->init.ckp_instance;
    timing_ctx->ckp_cb_ctx.module_ctx = timing_ctx;

    err = ecu_sensors_ckp_register_cb(timing_ctx->init.ckp_instance, ecu_modules_timing_ckp_signal_update_cb, &timing_ctx->ckp_cb_ctx);
    BREAK_IF(err != E_OK);

    for(int i = 0; i < ECU_SENSOR_CMP_MAX; i++) {
      timing_ctx->init.cmp_instances[i] = ECU_SENSOR_CMP_1 + i;
      timing_ctx->config_default.camshafts[i].cmp_instance = ECU_SENSOR_CMP_1 + i;
      timing_ctx->cmp_cb_ctx[i].cmp_instance = ECU_SENSOR_CMP_1 + i;
      timing_ctx->cmp_cb_ctx[i].module_ctx = timing_ctx;

      err = ecu_sensors_cmp_register_cb(i, ecu_modules_timing_cmp_signal_update_cb, &timing_ctx->cmp_cb_ctx[i]);
      BREAK_IF(err != E_OK);
    }
    BREAK_IF(err != E_OK);

    err = timing_init(timing_ctx->ctx, &timing_ctx->init);
    BREAK_IF(err != E_OK);

    memcpy(&timing_ctx->ctx->config, &timing_ctx->config_default, sizeof(timing_config_t));

  } while(0);

  return err;
}

error_t ecu_modules_timing_get_default_config(ecu_module_timing_t instance, timing_config_t *config)
{
  error_t err = E_OK;
  ecu_modules_timing_ctx_t *timing_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_MODULE_TIMING_MAX || config == NULL, err = E_PARAM);

    timing_ctx = &ecu_modules_timing_ctx[instance];

    memcpy(config, &timing_ctx->config_default, sizeof(timing_config_t));

  } while(0);

  return err;
}

error_t ecu_modules_timing_configure(ecu_module_timing_t instance, const timing_config_t *config)
{
  error_t err = E_OK;
  ecu_modules_timing_ctx_t *timing_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_MODULE_TIMING_MAX || config == NULL, err = E_PARAM);

    timing_ctx = &ecu_modules_timing_ctx[instance];

    err = timing_configure(timing_ctx->ctx, config);

  } while(0);

  return err;
}

error_t ecu_modules_timing_reset(ecu_module_timing_t instance)
{
  error_t err = E_OK;
  ecu_modules_timing_ctx_t *timing_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_MODULE_TIMING_MAX, err = E_PARAM);

    timing_ctx = &ecu_modules_timing_ctx[instance];

    err = timing_reset(timing_ctx->ctx);

  } while(0);

  return err;
}

error_t ecu_modules_timing_get_data(ecu_module_timing_t instance, timing_data_t *data)
{
  error_t err = E_OK;
  ecu_modules_timing_ctx_t *timing_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_MODULE_TIMING_MAX, err = E_PARAM);
    BREAK_IF_ACTION(data == NULL, err = E_PARAM);

    timing_ctx = &ecu_modules_timing_ctx[instance];

    err = timing_get_data(timing_ctx->ctx, data);

  } while(0);

  return err;
}

error_t ecu_modules_timing_get_diag(ecu_module_timing_t instance, timing_diag_t *diag)
{
  error_t err = E_OK;
  ecu_modules_timing_ctx_t *timing_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_MODULE_TIMING_MAX, err = E_PARAM);
    BREAK_IF_ACTION(diag == NULL, err = E_PARAM);

    timing_ctx = &ecu_modules_timing_ctx[instance];

    err = timing_get_diag(timing_ctx->ctx, diag);

  } while(0);

  return err;
}

error_t ecu_modules_timing_register_cb(ecu_module_timing_t instance, timing_signal_update_cb_t callback, void *usrdata)
{
  error_t err = E_OK;
  ecu_modules_timing_ctx_t *ckp_ctx;
  ecu_modules_timing_cb_t *cb;

  do {
    BREAK_IF_ACTION(instance >= ECU_MODULE_TIMING_MAX || callback == NULL, err = E_PARAM);

    ckp_ctx = &ecu_modules_timing_ctx[instance];

    err = E_OVERFLOW;

    for(int i = 0; i < ECU_MODULES_TIMING_CALLBACKS_MAX; i++) {
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

ITCM_FUNC static void ecu_modules_timing_ckp_signal_update_cb(void *usrdata, const ckp_data_t *data, const ckp_diag_t *diag)
{
  ecu_modules_timing_ckp_cb_ctx_t *ckp_cb_ctx = (ecu_modules_timing_ckp_cb_ctx_t *)usrdata;
  ecu_modules_timing_ctx_t *module_ctx;
  timing_ctx_t *ctx;

  do {
    BREAK_IF(ckp_cb_ctx == NULL);
    module_ctx = ckp_cb_ctx->module_ctx;
    BREAK_IF(module_ctx == NULL);
    ctx = module_ctx->ctx;
    BREAK_IF(ctx == NULL);

    timing_ckp_signal_update(ctx, data, diag);

    if(ctx->init.signal_update_cb) {
      ctx->init.signal_update_cb(ctx->init.signal_update_usrdata, &ctx->data, &ctx->diag);
    }

  } while(0);
}

ITCM_FUNC static void ecu_modules_timing_cmp_signal_update_cb(void *usrdata, const cmp_data_t *data, const cmp_diag_t *diag)
{
  ecu_modules_timing_cmp_cb_ctx_t *cmp_cb_ctx = (ecu_modules_timing_cmp_cb_ctx_t *)usrdata;
  ecu_modules_timing_ctx_t *module_ctx;
  timing_ctx_t *ctx;

  do {
    BREAK_IF(cmp_cb_ctx == NULL);
    module_ctx = cmp_cb_ctx->module_ctx;
    BREAK_IF(module_ctx == NULL);
    ctx = module_ctx->ctx;
    BREAK_IF(ctx == NULL);

    timing_cmp_signal_update(ctx, cmp_cb_ctx->cmp_instance, data, diag);

  } while(0);
}

ITCM_FUNC static void ecu_modules_timing_signal_update_cb(void *usrdata, const timing_data_t *data, const timing_diag_t *diag)
{
  ecu_modules_timing_ctx_t *ctx = (ecu_modules_timing_ctx_t *)usrdata;
  ecu_modules_timing_cb_t *cb;

  for(int i = 0; i < ECU_MODULES_TIMING_CALLBACKS_MAX; i++) {
    cb = &ctx->signal_update_callbacks[i];
    if(cb->callback != NULL) {
      cb->callback(cb->usrdata, data, diag);
    } else {
      break;
    }
  }
}
