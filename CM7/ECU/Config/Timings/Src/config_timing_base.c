/*
 * config_timing_base.c
 *
 *  Created on: Nov 13, 2025
 *      Author: VHEMaster
 */

#include "common.h"
#include "config_timing_base.h"
#include "config_extern.h"
#include "config_ckp.h"
#include "config_cmp.h"
#include "compiler.h"

static void ecu_timings_base_ckp_signal_update_cb(void *usrdata, const ckp_data_t *data, const ckp_diag_t *diag);
static void ecu_timings_base_cmp_signal_update_cb(void *usrdata, const cmp_data_t *data, const cmp_diag_t *diag);
static void ecu_timings_base_signal_update_cb(void *usrdata, const timing_base_data_t *data, const timing_base_diag_t *diag);

typedef struct ecu_timings_base_ctx_tag ecu_timings_base_ctx_t;

typedef struct {
    timing_base_signal_update_cb_t callback;
    void *usrdata;
}ecu_timings_base_cb_t;

typedef struct {
    ecu_sensor_ckp_t ckp_instance;
    ecu_timings_base_ctx_t *timing_ctx;
}ecu_timings_base_ckp_cb_ctx_t;

typedef struct {
    ecu_sensor_cmp_t cmp_instance;
    ecu_timings_base_ctx_t *timing_ctx;
}ecu_timings_base_cmp_cb_ctx_t;

typedef struct ecu_timings_base_ctx_tag {
    timing_base_config_t config_default;
    timing_base_init_ctx_t init;
    timing_base_ctx_t *ctx;
    ecu_timings_base_ckp_cb_ctx_t ckp_cb_ctx;
    ecu_timings_base_cmp_cb_ctx_t cmp_cb_ctx[ECU_SENSOR_CMP_MAX];
    ecu_timings_base_cb_t signal_update_callbacks[ECU_TIMINGS_BASE_CALLBACKS_MAX];
}ecu_timings_base_ctx_t;

static const timing_base_config_t ecu_timings_base_config_default = {
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
    .phased_syncmode = TIMING_BASE_CONFIG_PHASEDSYNCMODE_CAMSHAFT,
    .phased_only = true,
};

static const bool ecu_timings_base_enabled_default[ECU_TIMING_BASE_MAX] = {
    true
};

static RAM_SECTION ecu_timings_base_ctx_t ecu_timings_base_ctx[ECU_TIMING_BASE_MAX] = {
    {
      .init = {
          .ckp_instance = ECU_SENSOR_CKP_1,
          .signal_update_cb = ecu_timings_base_signal_update_cb,
          .signal_update_usrdata = &ecu_timings_base_ctx[0],
      },
      .config_default = ecu_timings_base_config_default,
    },
};

error_t ecu_timings_base_init(ecu_timing_base_t instance, timing_base_ctx_t *ctx)
{
  error_t err = E_OK;
  ecu_timings_base_ctx_t *timing_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_TIMING_BASE_MAX || ctx == NULL, err = E_PARAM);

    timing_ctx = &ecu_timings_base_ctx[instance];
    timing_ctx->ctx = ctx;

    timing_ctx->config_default.enabled = ecu_timings_base_enabled_default[instance];
    timing_ctx->ckp_cb_ctx.ckp_instance = timing_ctx->init.ckp_instance;
    timing_ctx->ckp_cb_ctx.timing_ctx = timing_ctx;

    err = ecu_sensors_ckp_register_cb(timing_ctx->init.ckp_instance, ecu_timings_base_ckp_signal_update_cb, &timing_ctx->ckp_cb_ctx);
    BREAK_IF(err != E_OK);

    for(int i = 0; i < ECU_SENSOR_CMP_MAX; i++) {
      timing_ctx->init.cmp_instances[i] = ECU_SENSOR_CMP_1 + i;
      timing_ctx->config_default.camshafts[i].cmp_instance = ECU_SENSOR_CMP_1 + i;
      timing_ctx->cmp_cb_ctx[i].cmp_instance = ECU_SENSOR_CMP_1 + i;
      timing_ctx->cmp_cb_ctx[i].timing_ctx = timing_ctx;

      err = ecu_sensors_cmp_register_cb(i, ecu_timings_base_cmp_signal_update_cb, &timing_ctx->cmp_cb_ctx[i]);
      BREAK_IF(err != E_OK);
    }
    BREAK_IF(err != E_OK);

    err = ecu_config_global_get_engine_calibration_config(&timing_ctx->init.calibration_config);
    BREAK_IF_ACTION(err != E_OK, err = E_FAULT);
    BREAK_IF_ACTION(timing_ctx->init.calibration_config == NULL, err = E_FAULT);

    err = timing_base_init(timing_ctx->ctx, &timing_ctx->init);
    BREAK_IF(err != E_OK);

    memcpy(&timing_ctx->ctx->config, &timing_ctx->config_default, sizeof(timing_base_config_t));

    err = ecu_timings_base_register_cb(instance, timing_base_signal_update_cb, timing_ctx->ctx);
    BREAK_IF(err != E_OK);

    err = ecu_timings_set_timing_enabled(ECU_TIMING_TYPE_BASE, instance, false);
    BREAK_IF(err != E_OK);

  } while(0);

  return err;
}

error_t ecu_timings_base_get_default_config(ecu_timing_base_t instance, timing_base_config_t *config)
{
  error_t err = E_OK;
  ecu_timings_base_ctx_t *timing_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_TIMING_BASE_MAX || config == NULL, err = E_PARAM);

    timing_ctx = &ecu_timings_base_ctx[instance];

    memcpy(config, &timing_ctx->config_default, sizeof(timing_base_config_t));

  } while(0);

  return err;
}

error_t ecu_timings_base_configure(ecu_timing_base_t instance, const timing_base_config_t *config)
{
  error_t err = E_OK;
  ecu_timings_base_ctx_t *timing_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_TIMING_BASE_MAX || config == NULL, err = E_PARAM);

    timing_ctx = &ecu_timings_base_ctx[instance];

    err = timing_base_configure(timing_ctx->ctx, config);
    BREAK_IF(err != E_OK);

    err = ecu_timings_set_timing_enabled(ECU_TIMING_TYPE_BASE, instance, timing_ctx->ctx->config.enabled);
    BREAK_IF(err != E_OK);

  } while(0);

  return err;
}

error_t ecu_timings_base_reset(ecu_timing_base_t instance)
{
  error_t err = E_OK;
  ecu_timings_base_ctx_t *timing_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_TIMING_BASE_MAX, err = E_PARAM);

    timing_ctx = &ecu_timings_base_ctx[instance];

    err = timing_base_reset(timing_ctx->ctx);

  } while(0);

  return err;
}

ITCM_FUNC error_t ecu_timings_base_get_crankshaft_data(ecu_timing_base_t instance, timing_base_data_crankshaft_t *data)
{
  error_t err = E_OK;
  ecu_timings_base_ctx_t *timing_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_TIMING_BASE_MAX, err = E_PARAM);
    BREAK_IF_ACTION(data == NULL, err = E_PARAM);

    timing_ctx = &ecu_timings_base_ctx[instance];

    err = timing_base_get_crankshaft_data(timing_ctx->ctx, data);

  } while(0);

  return err;
}

ITCM_FUNC error_t ecu_timings_base_get_data(ecu_timing_base_t instance, timing_base_data_t *data)
{
  error_t err = E_OK;
  ecu_timings_base_ctx_t *timing_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_TIMING_BASE_MAX, err = E_PARAM);
    BREAK_IF_ACTION(data == NULL, err = E_PARAM);

    timing_ctx = &ecu_timings_base_ctx[instance];

    err = timing_base_get_data(timing_ctx->ctx, data);

  } while(0);

  return err;
}

ITCM_FUNC error_t ecu_timings_base_get_data_ptr(ecu_timing_base_t instance, const timing_base_data_t **data)
{
  error_t err = E_OK;
  ecu_timings_base_ctx_t *timing_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_TIMING_BASE_MAX, err = E_PARAM);
    BREAK_IF_ACTION(data == NULL, err = E_PARAM);

    timing_ctx = &ecu_timings_base_ctx[instance];

    err = timing_base_get_data_ptr(timing_ctx->ctx, data);

  } while(0);

  return err;
}

ITCM_FUNC error_t ecu_timings_base_get_diag(ecu_timing_base_t instance, timing_base_diag_t *diag)
{
  error_t err = E_OK;
  ecu_timings_base_ctx_t *timing_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_TIMING_BASE_MAX, err = E_PARAM);
    BREAK_IF_ACTION(diag == NULL, err = E_PARAM);

    timing_ctx = &ecu_timings_base_ctx[instance];

    err = timing_base_get_diag(timing_ctx->ctx, diag);

  } while(0);

  return err;
}

error_t ecu_timings_base_register_cb(ecu_timing_base_t instance, timing_base_signal_update_cb_t callback, void *usrdata)
{
  error_t err = E_OK;
  ecu_timings_base_ctx_t *ckp_ctx;
  ecu_timings_base_cb_t *cb;

  do {
    BREAK_IF_ACTION(instance >= ECU_TIMING_BASE_MAX || callback == NULL, err = E_PARAM);

    ckp_ctx = &ecu_timings_base_ctx[instance];

    err = E_OVERFLOW;

    for(int i = 0; i < ECU_TIMINGS_BASE_CALLBACKS_MAX; i++) {
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

ITCM_FUNC static void ecu_timings_base_ckp_signal_update_cb(void *usrdata, const ckp_data_t *data, const ckp_diag_t *diag)
{
  ecu_timings_base_ckp_cb_ctx_t *ckp_cb_ctx = (ecu_timings_base_ckp_cb_ctx_t *)usrdata;
  ecu_timings_base_ctx_t *timing_ctx;
  timing_base_ctx_t *ctx;

  do {
    BREAK_IF(ckp_cb_ctx == NULL);
    timing_ctx = ckp_cb_ctx->timing_ctx;
    BREAK_IF(timing_ctx == NULL);
    ctx = timing_ctx->ctx;
    BREAK_IF(ctx == NULL);

    timing_base_ckp_signal_update(ctx, data, diag);

    if(ctx->init.signal_update_cb) {
      ctx->init.signal_update_cb(ctx->init.signal_update_usrdata, &ctx->data, &ctx->diag);
    }

  } while(0);
}

ITCM_FUNC static void ecu_timings_base_cmp_signal_update_cb(void *usrdata, const cmp_data_t *data, const cmp_diag_t *diag)
{
  ecu_timings_base_cmp_cb_ctx_t *cmp_cb_ctx = (ecu_timings_base_cmp_cb_ctx_t *)usrdata;
  ecu_timings_base_ctx_t *timing_ctx;
  timing_base_ctx_t *ctx;

  do {
    BREAK_IF(cmp_cb_ctx == NULL);
    timing_ctx = cmp_cb_ctx->timing_ctx;
    BREAK_IF(timing_ctx == NULL);
    ctx = timing_ctx->ctx;
    BREAK_IF(ctx == NULL);

    timing_base_cmp_signal_update(ctx, cmp_cb_ctx->cmp_instance, data, diag);

  } while(0);
}

ITCM_FUNC static void ecu_timings_base_signal_update_cb(void *usrdata, const timing_base_data_t *data, const timing_base_diag_t *diag)
{
  ecu_timings_base_ctx_t *ctx = (ecu_timings_base_ctx_t *)usrdata;
  ecu_timings_base_cb_t *cb;

  for(int i = 0; i < ECU_TIMINGS_BASE_CALLBACKS_MAX; i++) {
    cb = &ctx->signal_update_callbacks[i];
    if(cb->callback != NULL) {
      cb->callback(cb->usrdata, data, diag);
    } else {
      break;
    }
  }
}
