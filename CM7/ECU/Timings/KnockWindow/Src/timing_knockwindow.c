/*
 * timing_knockwindow.c
 *
 *  Created on: 5 окт. 2026 г.
 *      Author: VHEMaster
 */



#include "config_global.h"
#include "timing_common.h"
#include "config_hw.h"
#include "common.h"
#include "interpolation.h"

static void knockwindow_pulsedadc_sampling_cplt_cb_t(void *usrdata, const pulsedadc_sampling_cplt_ctx_t *cplt_ctx);
static void knockwindow_pulsedadc_sampling_error_cb_t(void *usrdata);

error_t knockwindow_init(knockwindow_ctx_t *ctx, const knockwindow_init_ctx_t *init_ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || init_ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(init_ctx->calibration_config == NULL, err = E_PARAM);

    memset(ctx, 0u, sizeof(knockwindow_ctx_t));
    memcpy(&ctx->init, init_ctx, sizeof(knockwindow_init_ctx_t));


    ctx->ready = true;

  } while(0);

  return err;
}

error_t knockwindow_configure(knockwindow_ctx_t *ctx, const knockwindow_config_t *config)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || config == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    ctx->configured = false;

    if(&ctx->config != config) {
      memcpy(&ctx->config, config, sizeof(knockwindow_config_t));
    }

    if(ctx->config.enabled) {
      err = ecu_devices_get_pulsedadc_ctx(config->pulsedadc_instance, &ctx->pulsedadc_ctx);
      BREAK_IF(err != E_OK);

      err = ecu_devices_pulsedadc_register_cb(config->pulsedadc_instance,
          knockwindow_pulsedadc_sampling_cplt_cb_t,
          knockwindow_pulsedadc_sampling_error_cb_t,
          ctx);
      BREAK_IF(err != E_OK);

      ctx->configured = true;
    }

  } while(0);

  return err;
}

error_t knockwindow_reset(knockwindow_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    ctx->configured = false;

  } while(0);

  return err;
}

error_t knockwindow_get_runtime_data_ptr(knockwindow_ctx_t *ctx, knockwindow_runtime_ctx_t **runtime_data)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(runtime_data == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    *runtime_data = &ctx->runtime;

  } while(0);

  return err;
}

OPTIMIZE_FAST
ITCM_FUNC void knockwindow_signal_update_callback(knockwindow_ctx_t *ctx)
{
  error_t err;
  timing_base_crankshaft_mode_t crankshaft_mode;
  const timing_base_data_crankshaft_t *crankshaft_data;
  const timing_base_data_t *timing_base_data;
  const knockwindow_config_t *config;
  knockwindow_runtime_ctx_t *runtime;
  uint16_t samples_requested;
  time_float_delta_us_t window_delta;

  float window_start, window_end, window_prepare;

  do {
    config = &ctx->config;

    BREAK_IF(ctx->configured == false);
    BREAK_IF(config->enabled == false);
    BREAK_IF(ctx->pulsedadc_ctx == NULL);

    // TODO: assign proper instance
    err = ecu_timings_base_get_data_ptr(ECU_TIMING_BASE_1, &timing_base_data);
    BREAK_IF_ACTION(err != E_OK, err = E_FAULT);
    BREAK_IF_ACTION(timing_base_data == NULL, err = E_FAULT);

    ctx->timing_base_data = timing_base_data;
    crankshaft_mode = timing_base_data->crankshaft.mode;
    BREAK_IF(crankshaft_mode < TIMING_CRANKSHAFT_MODE_VALID);

    crankshaft_data = &timing_base_data->sequentialed[TIMING_RUNTIME_CYLINDER_SEQUENTIAL].cylinders[ECU_CYLINDER_1].crankshaft_data;

    runtime = &ctx->runtime;
    BREAK_IF(runtime->inputs.allowed.valid != true);
    BREAK_IF(runtime->inputs.allowed.value != ECU_RUNTIME_PARAMETER_TRUE);

    BREAK_IF(runtime->inputs.knock_window_start.valid != true);
    BREAK_IF(runtime->inputs.knock_window_end.valid != true);

    window_start = runtime->inputs.knock_window_start.value;
    window_end = runtime->inputs.knock_window_end.value;
    window_prepare = window_start - config->window_prepare_advance;

    if(runtime->state == KNOCKWINDOW_STATE_SYNC) {
      if(crankshaft_data->sensor_data.current.position > window_end || crankshaft_data->sensor_data.current.position < window_prepare) {
        runtime->state = KNOCKWINDOW_STATE_WAITING;
      }
    } else if(runtime->state == KNOCKWINDOW_STATE_WAITING) {
      if(crankshaft_data->sensor_data.current.position >= window_prepare && crankshaft_data->sensor_data.current.position < window_end) {
        if(crankshaft_data->sensor_data.current.position < window_start) {
          window_delta = (window_end - window_start) * crankshaft_data->sensor_data.us_per_degree_pulsed;
          samples_requested = window_delta * ctx->pulsedadc_ctx->sampling_frequency * TIME_S_IN_US;
          runtime->position_start = window_start;
          runtime->position_cplt = window_end;
          runtime->samples_requested = samples_requested;
          runtime->cplt_irq = false;
          runtime->error_irq = false;
          err = pulsedadc_prepare(ctx->pulsedadc_ctx, samples_requested);
          if(err == E_OVERFLOW) {
            runtime->state = KNOCKWINDOW_STATE_SYNC;
          } else  if(err != E_OK) {
            runtime->state = KNOCKWINDOW_STATE_SYNC;
            // TODO: error handling
            break;
          }
          runtime->state = KNOCKWINDOW_STATE_PREPARED;
        } else {
          runtime->state = KNOCKWINDOW_STATE_SYNC;
          // TODO: OVERSHOOT
          break;
        }
      }
    } else if(runtime->state == KNOCKWINDOW_STATE_PREPARED) {
      if(crankshaft_data->sensor_data.current.position >= window_start) {
        if(crankshaft_data->sensor_data.current.position < window_end) {
          err = pulsedadc_start(ctx->pulsedadc_ctx);
          if(err != E_OK) {
            runtime->state = KNOCKWINDOW_STATE_SYNC;
            // TODO: error handling
            break;
          }
          runtime->state = KNOCKWINDOW_STATE_SAMPLING;
        } else {
          runtime->state = KNOCKWINDOW_STATE_SYNC;
          // TODO: OVERSHOOT
          break;
        }
      }
    } else if(runtime->state == KNOCKWINDOW_STATE_SAMPLING) {
      // TODO: timeout handling

      if(runtime->cplt_irq) {
        runtime->cplt_irq = false;
        runtime->state = KNOCKWINDOW_STATE_SYNC;

        ctx->sampling_cplt_ctx.position_start = runtime->position_start;
        ctx->sampling_cplt_ctx.position_cplt = runtime->position_cplt;

        if(ctx->init.callback != NULL) {
          ctx->init.callback(ctx->init.callback_usrdata, &ctx->sampling_cplt_ctx);
        }
      } else if(runtime->error_irq) {
        runtime->error_irq = false;
        runtime->state = KNOCKWINDOW_STATE_SYNC;
      } else if(crankshaft_data->sensor_data.current.position >= window_end) {
        runtime->state = KNOCKWINDOW_STATE_SYNC;
      }
    }
  } while(0);
}

ITCM_FUNC static void knockwindow_pulsedadc_sampling_cplt_cb_t(void *usrdata, const pulsedadc_sampling_cplt_ctx_t *cplt_ctx)
{
  knockwindow_ctx_t *ctx = (knockwindow_ctx_t *)usrdata;

  ctx->sampling_cplt_ctx.samples_buffer = cplt_ctx->samples_buffer;
  ctx->sampling_cplt_ctx.samples_count = cplt_ctx->samples_count;
  ctx->sampling_cplt_ctx.time_start = cplt_ctx->time_start;
  ctx->sampling_cplt_ctx.time_cplt = cplt_ctx->time_cplt;

  ctx->runtime.cplt_irq = true;
}

ITCM_FUNC static void knockwindow_pulsedadc_sampling_error_cb_t(void *usrdata)
{
  knockwindow_ctx_t *ctx = (knockwindow_ctx_t *)usrdata;

  ctx->runtime.error_irq = true;
}
