/*
 * timing_common.c
 *
 *  Created on: Aug 14, 2024
 *      Author: VHEMaster
 */

#include "timing_common.h"
#include "common.h"
#include "queuedpulses.h"

#define CORE_OUTPUT_ACTIVE      1
#define CORE_OUTPUT_INACTIVE    0

static error_t timing_calculate_position_ex(timing_base_ctx_t *ctx, float offset, bool time_recalc, bool phased, timing_base_req_t *req_ctx, timing_base_data_crankshaft_t *data);
static error_t timing_calculate_position_from_snapshot_ex(timing_base_ctx_t *ctx, const timing_base_data_crankshaft_t *snapshot, float offset, bool time_recalc, bool phased, timing_base_req_t *req_ctx, timing_base_data_crankshaft_t *data);

OPTIMIZE_FAST
ITCM_FUNC error_t timing_pulse_schedule(ecu_gpio_output_pin_t output_pin, time_us_t pulse_start, time_us_t pulse_end, timing_late_policy_t late_policy)
{
  output_id_t pin;
  error_t err;

  do {
    BREAK_IF_ACTION(late_policy >= TIMING_LATE_POLICY_MAX, err = E_PARAM)

    err = ecu_config_gpio_output_get_id(output_pin, &pin);
    BREAK_IF(err != E_OK);

    pulse_start &= time_mask_us();
    pulse_end &= time_mask_us();

    err = queuedpulses_schedule(pin, pulse_start, pulse_end, CORE_OUTPUT_ACTIVE,
        late_policy == TIMING_LATE_POLICY_KEEP_WIDTH);

  } while(0);

  return err;
}

OPTIMIZE_FAST
ITCM_FUNC error_t timing_pulse_clean(ecu_gpio_output_pin_t output_pin)
{
  output_id_t pin;
  error_t err;

  do {

    err = ecu_config_gpio_output_get_id(output_pin, &pin);
    BREAK_IF(err != E_OK);

    err = queuedpulses_reset_output(pin);
    BREAK_IF(err != E_OK);

  } while(0);

  return err;
}

OPTIMIZE_FAST
ITCM_FUNC static error_t timing_calculate_position_ex(timing_base_ctx_t *ctx, float offset, bool time_recalc, bool phased, timing_base_req_t *req_ctx, timing_base_data_crankshaft_t *data)
{
  error_t err = E_OK;
  uint32_t prim;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(data == NULL, err = E_PARAM);

    prim = EnterCritical();
    *data = ctx->data.crankshaft;
    ExitCritical(prim);

    err = timing_calculate_position_from_snapshot_ex(ctx, data, offset, time_recalc, phased, req_ctx, data);

  } while(0);

  return err;
}

OPTIMIZE_FAST
ITCM_FUNC static error_t timing_calculate_position_from_snapshot_ex(timing_base_ctx_t *ctx, const timing_base_data_crankshaft_t *snapshot, float offset, bool time_recalc, bool phased, timing_base_req_t *req_ctx, timing_base_data_crankshaft_t *data)
{
  error_t err = E_OK;
  float pos, pos_prev, current, previous;
  time_us_t now;
  float *positions[2];
  float pos_temp;
  bool phased_internal = phased;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(data == NULL, err = E_PARAM);
    BREAK_IF_ACTION(snapshot == NULL, err = E_PARAM);

    if(data != snapshot) {
      *data = *snapshot;
    }

    positions[0] = &data->sensor_data.current.position;
    positions[1] = &data->sensor_data.previous.position;

    if(data->mode != TIMING_CRANKSHAFT_MODE_VALID_PHASED) {
      phased_internal = false;
    } else if(req_ctx != NULL && req_ctx->phased == false && ctx->config.phased_only == false) {
      phased_internal = false;
    }

    for(int i = 0; i < ITEMSOF(positions); i++) {
      pos_temp = *positions[i];
      pos_temp += offset;
      timing_position_clamp(pos_temp, phased_internal, &pos_temp);

      *positions[i] = pos_temp;
    }

    pos = data->sensor_data.current.position;

    if(data->valid && data->mode >= TIMING_CRANKSHAFT_MODE_VALID) {

      if(req_ctx == NULL || req_ctx->position_valid) {
        if(time_recalc) {
          now = time_now_us();
          current = data->sensor_data.current.position;
          previous = data->sensor_data.previous.position;

          if(current < previous) {
            if(phased_internal) {
              current += 720.0f;
            } else {
              current += 360.0f;
            }
          }

          pos = time_interpolate_value(
              data->sensor_data.previous.timestamp,
              now,
              data->sensor_data.current.timestamp,
              previous,
              current);

          timing_position_clamp(pos, phased_internal, &pos);
        }

        if(req_ctx != NULL) {
          pos_prev = req_ctx->position_prev;

          if((pos - pos_prev < 0.0f && pos - pos_prev > -90.0f) || pos - pos_prev > 90.0f) {
            pos = pos_prev;
          }

          if(phased && !phased_internal && data->mode == TIMING_CRANKSHAFT_MODE_VALID_PHASED) {
            if(pos > 90.0f) {
              req_ctx->phased = true;
              phased_internal = true;
            }
          } else if(req_ctx->phased && !phased_internal && data->mode < TIMING_CRANKSHAFT_MODE_VALID_PHASED) {
            pos_temp = req_ctx->position_prev;
            if(pos_temp > 90.0f || pos_temp <= -180.0f) {
              req_ctx->phased = false;

              timing_position_clamp(pos_temp, false, &pos_temp);
              req_ctx->position_prev = pos_temp;

              timing_position_clamp(pos, false, &pos);

              if(offset != 0.0f) {
                for(int i = 0; i < ITEMSOF(positions); i++) {
                  pos_temp = *positions[i];
                  pos_temp += offset;

                  timing_position_clamp(pos_temp, false, &pos_temp);
                  *positions[i] = pos_temp;
                }
              }
            }
          }
        }
      } else {
        pos = data->sensor_data.current.position;
      }

      //Check for NaNs
      if(pos != pos) {
        ctx->diag.crankshaft.bits.pos_calc_nan = true;
        data->valid = false;
        pos = 0.0f;
      }

      if(req_ctx != NULL) {
        req_ctx->position_prev = pos;
        req_ctx->position_valid = true;
      }

      if(!phased_internal) {
        data->mode = MIN(data->mode, TIMING_CRANKSHAFT_MODE_VALID);
      } else if(data->mode > TIMING_CRANKSHAFT_MODE_VALID) {
        data->mode = TIMING_CRANKSHAFT_MODE_VALID_PHASED;
      }

    } else {
      if(req_ctx != NULL) {
        req_ctx->position_prev = pos;
        req_ctx->position_valid = false;
        req_ctx->phased = false;
      }
    }

    data->sensor_data.current_position = pos;
    data->pos_phased = pos;

  } while(0);

  return err;
}

OPTIMIZE_FAST
INLINE ITCM_FUNC error_t timing_calculate_offset_position(timing_base_ctx_t *ctx, float offset, bool phased, timing_base_req_t *req_ctx, timing_base_data_crankshaft_t *data)
{
  return timing_calculate_position_ex(ctx, offset, false, phased, req_ctx, data);
}

OPTIMIZE_FAST
INLINE ITCM_FUNC error_t timing_calculate_current_position(timing_base_ctx_t *ctx, float offset, bool phased, timing_base_req_t *req_ctx, timing_base_data_crankshaft_t *data)
{
  return timing_calculate_position_ex(ctx, offset, true, phased, req_ctx, data);
}

OPTIMIZE_FAST
ITCM_FUNC error_t timing_calculate_cylinders_positions(timing_base_ctx_t *ctx, bool sequential, bool semisequential)
{
  error_t err = E_OK;
  timing_base_data_crankshaft_t snapshot;
  const ecu_config_engine_calibration_t *calibration_config = NULL;
  timing_base_cylinder_sequentialed_cylinder_ctx_t *sequentialed;
  float pos_offset;
  uint32_t prim;

  calibration_config = ctx->init.calibration_config;

  prim = EnterCritical();
  snapshot = ctx->data.crankshaft;
  ExitCritical(prim);

  if(sequential) {
    for(ecu_cylinder_t cy = 0; cy < calibration_config->cylinders.cylinders_count; cy++) {
      pos_offset = calibration_config->cylinders.cylinders[cy].pos_offset;

      sequentialed = &ctx->data.sequentialed[TIMING_RUNTIME_CYLINDER_SEQUENTIAL].cylinders[cy];
      err = timing_calculate_offset_position_from_snapshot(ctx, &snapshot,
          pos_offset, true,
          &sequentialed->timing_req,
          &sequentialed->crankshaft_data);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));
    }
  }

  if(semisequential) {
    for(ecu_cylinder_t cy = 0; cy < calibration_config->cylinders.cylinders_count; cy++) {
      pos_offset = calibration_config->cylinders.cylinders[cy].pos_offset;

      sequentialed = &ctx->data.sequentialed[TIMING_RUNTIME_CYLINDER_SEMISEQUENTIAL_DISTRIBUTOR].cylinders[cy];
      err = timing_calculate_offset_position_from_snapshot(ctx, &snapshot,
          pos_offset, false,
          &sequentialed->timing_req,
          &sequentialed->crankshaft_data);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));
    }
  }

  return err;
}

OPTIMIZE_FAST
INLINE ITCM_FUNC error_t timing_calculate_offset_position_from_snapshot(timing_base_ctx_t *ctx, const timing_base_data_crankshaft_t *snapshot, float offset, bool phased, timing_base_req_t *req_ctx, timing_base_data_crankshaft_t *data)
{
  return timing_calculate_position_from_snapshot_ex(ctx, snapshot, offset, false, phased, req_ctx, data);
}

OPTIMIZE_FAST
INLINE ITCM_FUNC error_t timing_calculate_current_position_from_snapshot(timing_base_ctx_t *ctx, const timing_base_data_crankshaft_t *snapshot, float offset, bool phased, timing_base_req_t *req_ctx, timing_base_data_crankshaft_t *data)
{
  return timing_calculate_position_from_snapshot_ex(ctx, snapshot, offset, true, phased, req_ctx, data);
}
