/*
 * timing_rough.c
 *
 *  Created on: Jun 1, 2025
 *      Author: VHEMaster
 */

#include "config_global.h"
#include "timing_common.h"
#include "config_hw.h"
#include "common.h"
#include "interpolation.h"

error_t rough_init(rough_ctx_t *ctx, const rough_init_ctx_t *init_ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || init_ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(init_ctx->calibration_config == NULL, err = E_PARAM);

    memset(ctx, 0u, sizeof(rough_ctx_t));
    memcpy(&ctx->init, init_ctx, sizeof(rough_init_ctx_t));


    ctx->ready = true;

  } while(0);

  return err;
}

error_t rough_configure(rough_ctx_t *ctx, const rough_config_t *config)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || config == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    ctx->configured = false;

    if(&ctx->config != config) {
      memcpy(&ctx->config, config, sizeof(rough_config_t));
    }

    ctx->configured = true;

  } while(0);

  return err;
}

error_t rough_reset(rough_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    ctx->configured = false;

  } while(0);

  return err;
}

error_t rough_get_runtime_data_ptr(rough_ctx_t *ctx, rough_runtime_ctx_t **runtime_data)
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
ITCM_FUNC void rough_signal_update_callback(rough_ctx_t *ctx)
{
  error_t err;
  timing_base_crankshaft_mode_t crankshaft_mode;
  const ecu_config_cylinders_t *cylinders_config;
  const ecu_config_engine_calibration_t *calibration_config;
  const timing_base_data_crankshaft_t *crankshaft_data;
  const timing_base_data_t *timing_base_data;
  rough_runtime_ctx_t *runtime;
  rough_runtime_cylinder_ctx_t *runtime_cy;
  timing_base_runtime_cylinder_sequentialed_type_t sequentialed_mode;
  bool needtoclear = false;
  uint32_t cylinders_count;

  float start_pos, end_pos, mid_pos;

  do {
    // TODO: assign proper instance
    err = ecu_timings_base_get_data_ptr(ECU_TIMING_BASE_1, &timing_base_data);
    BREAK_IF_ACTION(err != E_OK, err = E_FAULT);
    BREAK_IF_ACTION(timing_base_data == NULL, err = E_FAULT);

    calibration_config = ctx->init.calibration_config;
    crankshaft_mode = timing_base_data->crankshaft.mode;
    sequentialed_mode = TIMING_RUNTIME_CYLINDER_SEQUENTIALED_NONE;
    cylinders_config = &calibration_config->cylinders;
    runtime = &ctx->runtime;

    cylinders_count = calibration_config->cylinders.cylinders_count;

    end_pos = fabsf(cylinders_config->rough_measure_range);
    start_pos = -end_pos;
    mid_pos = 0.0f;

    if(crankshaft_mode >= TIMING_CRANKSHAFT_MODE_VALID) {
      if(crankshaft_mode == TIMING_CRANKSHAFT_MODE_VALID_PHASED) {
        sequentialed_mode = TIMING_RUNTIME_CYLINDER_SEQUENTIAL;
      } else {
        sequentialed_mode = TIMING_RUNTIME_CYLINDER_SEMISEQUENTIAL_DISTRIBUTOR;
      }
    } else {
      needtoclear = true;
    }

    if(runtime->sequentialed_mode != sequentialed_mode) {
      runtime->sequentialed_mode = sequentialed_mode;
      needtoclear = true;
    }

    if(!needtoclear) {
      for(ecu_cylinder_t cy = 0; cy < cylinders_count; cy++) {
        crankshaft_data = &timing_base_data->sequentialed[sequentialed_mode].cylinders[cy].crankshaft_data;
        runtime_cy = &runtime->cylinders[cy];
        needtoclear = false;

        if(runtime_cy->ready) {
          if(runtime_cy->measuring_start) {
            if(crankshaft_data->sensor_data.current.position >= mid_pos && crankshaft_data->sensor_data.current.position <= end_pos) {
              runtime_cy->time_tdc = math_interpolate(
                  crankshaft_data->sensor_data.previous.position, mid_pos, crankshaft_data->sensor_data.current.position,
                  crankshaft_data->sensor_data.previous.timestamp, crankshaft_data->sensor_data.current.timestamp);
              runtime_cy->measuring_end = true;
              runtime_cy->measuring_start = false;
            } else if(crankshaft_data->sensor_data.current.position > end_pos) {
              needtoclear = true;
            }
          } else if(runtime_cy->measuring_end) {
            if(crankshaft_data->sensor_data.current.position >= end_pos && crankshaft_data->sensor_data.current.position >= start_pos) {
              runtime_cy->time_end = math_interpolate(
                  crankshaft_data->sensor_data.previous.position, end_pos, crankshaft_data->sensor_data.current.position,
                  crankshaft_data->sensor_data.previous.timestamp, crankshaft_data->sensor_data.current.timestamp);
              runtime_cy->measuring_end = false;
              runtime_cy->measuring_start = false;

              runtime_cy->rough_value = math_interpolate(
                  runtime_cy->time_start, runtime_cy->time_tdc, runtime_cy->time_end, -100.0f, 100.0f);

            } else if(crankshaft_data->sensor_data.current.position < start_pos) {
              needtoclear = true;
            }
          } else {
            if(crankshaft_data->sensor_data.current.position >= start_pos && crankshaft_data->sensor_data.current.position <= mid_pos) {
              runtime_cy->time_start = math_interpolate(
                  crankshaft_data->sensor_data.previous.position, start_pos, crankshaft_data->sensor_data.current.position,
                  crankshaft_data->sensor_data.previous.timestamp, crankshaft_data->sensor_data.current.timestamp);
              runtime_cy->measuring_start = true;
              runtime_cy->measuring_end = false;
            }
          }
        } else {
          if(crankshaft_data->sensor_data.current.position < start_pos || crankshaft_data->sensor_data.current.position > end_pos) {
            runtime_cy->ready = true;
            runtime_cy->measuring_start = false;
            runtime_cy->measuring_end = false;
          }
        }

        if(needtoclear) {
          memset(&runtime->cylinders[cy], 0, sizeof(runtime->cylinders[cy]));
          needtoclear = false;
        }
      }
    } else {
      memset(runtime->cylinders, 0, sizeof(runtime->cylinders));
    }

  } while(0);
}
