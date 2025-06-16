/*
 * core_timing_rough.c
 *
 *  Created on: Jun 1, 2025
 *      Author: VHEMaster
 */

#include "core_timing_common.h"
#include "config_hw.h"
#include "common.h"
#include "interpolation.h"

OPTIMIZE_FAST
ITCM_FUNC void core_timing_signal_update_rough(ecu_core_ctx_t *ctx)
{
  timing_crankshaft_mode_t crankshaft_mode;
  timing_data_crankshaft_t *crankshaft_data;
  const ecu_config_cylinders_t *cylinders_config;
  ecu_core_runtime_global_rough_ctx_t *runtime;
  ecu_core_runtime_group_cylinder_rough_ctx_t *runtime_cy;
  ecu_core_runtime_cylinder_sequentialed_type_t sequentialed_mode;
  bool needtoclear = false;

  float start_pos, end_pos, mid_pos;

  do {
    crankshaft_mode = ctx->timing_data.crankshaft.mode;
    sequentialed_mode = ECU_CORE_RUNTIME_CYLINDER_SEQUENTIALED_NONE;
    cylinders_config = &ctx->calibration->cylinders;
    runtime = &ctx->runtime.global.rough;

    end_pos = fabsf(cylinders_config->rough_measure_range);
    start_pos = -end_pos;
    mid_pos = 0.0f;

    if(crankshaft_mode >= TIMING_CRANKSHAFT_MODE_VALID) {
      if(crankshaft_mode == TIMING_CRANKSHAFT_MODE_VALID_PHASED) {
        sequentialed_mode = ECU_CORE_RUNTIME_CYLINDER_SEQUENTIAL;
      } else {
        sequentialed_mode = ECU_CORE_RUNTIME_CYLINDER_SEMISEQUENTIAL_DISTRIBUTOR;
      }
    } else {
      needtoclear = true;
    }

    if(runtime->sequentialed_mode != sequentialed_mode) {
      runtime->sequentialed_mode = sequentialed_mode;
      needtoclear = true;
    }

    if(!needtoclear) {
      for(ecu_cylinder_t cy = 0; cy < ctx->runtime.global.cylinders_count; cy++) {
        crankshaft_data = &ctx->runtime.cylinders[cy].sequentialed[sequentialed_mode].crankshaft_data;
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
