/*
 * config_knockwindow.c
 *
 *  Created on: 5 окт. 2026 г.
 *      Author: VHEMaster
 */

#include "common.h"
#include "config_timings.h"
#include "versioned_timings.h"

typedef struct ecu_timings_knockwindow_ctx_tag ecu_timings_knockwindow_ctx_t;

typedef struct ecu_timings_knockwindow_ctx_tag {
    knockwindow_config_t config_default;
    knockwindow_init_ctx_t init;
    knockwindow_ctx_t *ctx;
}ecu_timings_knockwindow_ctx_t;

static const knockwindow_config_t ecu_timings_knockwindow_config_default = {

};

static const bool ecu_timings_knockwindow_enabled_default[ECU_TIMING_KNOCKWINDOW_MAX] = {
    true
};

static RAM_SECTION ecu_timings_knockwindow_ctx_t ecu_timings_knockwindow_ctx[ECU_TIMING_KNOCKWINDOW_MAX] = {
    {
      .init = {

      },
      .config_default = ecu_timings_knockwindow_config_default,
    },
};

error_t ecu_timings_knockwindow_init(ecu_timing_knockwindow_t instance, knockwindow_ctx_t *ctx)
{
  error_t err = E_OK;
  ecu_timings_knockwindow_ctx_t *timing_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_TIMING_KNOCKWINDOW_MAX || ctx == NULL, err = E_PARAM);

    timing_ctx = &ecu_timings_knockwindow_ctx[instance];
    timing_ctx->ctx = ctx;

    timing_ctx->config_default.enabled = ecu_timings_knockwindow_enabled_default[instance];

    err = ecu_config_global_get_engine_calibration_config(&timing_ctx->init.calibration_config);
    BREAK_IF_ACTION(err != E_OK, err = E_FAULT);
    BREAK_IF_ACTION(timing_ctx->init.calibration_config == NULL, err = E_FAULT);

    err = knockwindow_init(timing_ctx->ctx, &timing_ctx->init);
    BREAK_IF(err != E_OK);

    memcpy(&timing_ctx->ctx->config, &timing_ctx->config_default, sizeof(knockwindow_config_t));

    err = ecu_timings_set_timing_enabled(ECU_TIMING_TYPE_KNOCKWINDOW, instance, false);
    BREAK_IF(err != E_OK);

  } while(0);

  return err;
}

error_t ecu_timings_knockwindow_get_default_config(ecu_timing_knockwindow_t instance, knockwindow_config_t *config)
{
  error_t err = E_OK;
  ecu_timings_knockwindow_ctx_t *timing_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_TIMING_KNOCKWINDOW_MAX || config == NULL, err = E_PARAM);

    timing_ctx = &ecu_timings_knockwindow_ctx[instance];

    memcpy(config, &timing_ctx->config_default, sizeof(knockwindow_config_t));

  } while(0);

  return err;
}

error_t ecu_timings_knockwindow_configure(ecu_timing_knockwindow_t instance, const knockwindow_config_t *config)
{
  error_t err = E_OK;
  ecu_timings_knockwindow_ctx_t *timing_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_TIMING_KNOCKWINDOW_MAX || config == NULL, err = E_PARAM);

    timing_ctx = &ecu_timings_knockwindow_ctx[instance];

    err = knockwindow_configure(timing_ctx->ctx, config);
    BREAK_IF(err != E_OK);

    err = ecu_timings_set_timing_enabled(ECU_TIMING_TYPE_KNOCKWINDOW, instance, timing_ctx->ctx->config.enabled);
    BREAK_IF(err != E_OK);

  } while(0);

  return err;
}

error_t ecu_timings_knockwindow_reset(ecu_timing_knockwindow_t instance)
{
  error_t err = E_OK;
  ecu_timings_knockwindow_ctx_t *timing_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_TIMING_KNOCKWINDOW_MAX, err = E_PARAM);

    timing_ctx = &ecu_timings_knockwindow_ctx[instance];

    err = knockwindow_reset(timing_ctx->ctx);

  } while(0);

  return err;
}

error_t ecu_timings_knockwindow_get_runtime_data_ptr(ecu_timing_knockwindow_t instance, knockwindow_runtime_ctx_t **data)
{
  error_t err = E_OK;
  ecu_timings_knockwindow_ctx_t *timing_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_TIMING_KNOCKWINDOW_MAX, err = E_PARAM);
    BREAK_IF_ACTION(data == NULL, err = E_PARAM);

    timing_ctx = &ecu_timings_knockwindow_ctx[instance];

    err = knockwindow_get_runtime_data_ptr(timing_ctx->ctx, data);
    BREAK_IF(err != E_OK);

  } while(0);

  return err;
}
