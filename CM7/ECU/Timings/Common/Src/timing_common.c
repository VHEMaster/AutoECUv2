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

OPTIMIZE_FAST
ITCM_FUNC error_t timing_pulse_schedule(ecu_gpio_output_pin_t output_pin, time_us_t pulse_start, time_us_t pulse_end)
{
  time_us_t now;
  time_us_t time_mask, time_mask_2;
  output_id_t pin;
  error_t err;
  error_t ret;
  time_delta_us_t delta;

  do {
    ret = E_OK;
    time_mask = time_mask_us();
    time_mask_2 = time_mask >> 1;

    err = ecu_config_gpio_output_get_id(output_pin, &pin);
    BREAK_IF_ACTION(ret == E_OK && err != E_OK, ret = err);

    err = E_OK;
    now = time_now_us();
    delta = time_diff(pulse_start, now);
    if(delta < time_mask_2) {
      err = queuedpulses_enqueue_ex(pin, delta, CORE_OUTPUT_INACTIVE);
    } else {
      err = output_set_value(pin, CORE_OUTPUT_ACTIVE);
    }
    if(ret == E_OK && err != E_OK) {
      (void)output_set_value(pin, CORE_OUTPUT_INACTIVE);
      ret = err;
      break;
    }

    err = E_OK;
    delta = time_diff(pulse_end, pulse_start);
    if(delta < time_mask_2) {
      err = queuedpulses_enqueue_ex(pin, delta, CORE_OUTPUT_ACTIVE);
    } else {
      err = output_set_value(pin, CORE_OUTPUT_INACTIVE);
    }
    if(ret == E_OK && err != E_OK) {
      (void)output_set_value(pin, CORE_OUTPUT_INACTIVE);
      ret = err;
      break;
    }
  } while(0);

  return ret;
}

OPTIMIZE_FAST
ITCM_FUNC INLINE void timing_position_clamp(float input, bool phased, float *output)
{
  float out_value = input;

  do {
    BREAK_IF(output == NULL);

    if(phased) {
      while(out_value >= 360.0f) {
        out_value -= 720.0f;
      }
      while(out_value < -360.0f) {
        out_value += 720.0f;
      }
    } else {
      while(out_value >= 180.0f) {
        out_value -= 360.0f;
      }
      while(out_value < -180.0f) {
        out_value += 360.0f;
      }
    }

    *output = out_value;
  } while(0);
}

OPTIMIZE_FAST
ITCM_FUNC static error_t timing_calculate_position_ex(timing_base_ctx_t *ctx, float offset, bool time_recalc, bool phased, timing_base_req_t *req_ctx, timing_base_data_crankshaft_t *data)
{
  error_t err = E_OK;
  float pos, pos_prev, mult, time_delta, current, previous;
  timing_base_data_crankshaft_t data_cur;
  time_us_t now;
  uint32_t prim;
  float *positions[2];
  float pos_temp;
  bool phased_internal = phased;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(data == NULL, err = E_PARAM);

    prim = EnterCritical();
    data_cur = ctx->data.crankshaft;
    ExitCritical(prim);

    if(time_recalc) {
      now = time_now_us();
    }

    positions[0] = &data_cur.sensor_data.current.position;
    positions[1] = &data_cur.sensor_data.previous.position;

    if(data_cur.mode != TIMING_CRANKSHAFT_MODE_VALID_PHASED) {
      phased_internal = false;
    } else if(req_ctx != NULL && req_ctx->phased == false && ctx->config.phased_only == false) {
      phased_internal = false;
    }

    if(offset != 0.0f) {
      for(int i = 0; i < ITEMSOF(positions); i++) {
        pos_temp = *positions[i];
        pos_temp += offset;
        timing_position_clamp(pos_temp, phased_internal, &pos_temp);

        *positions[i] = pos_temp;
      }
    }

    pos = data_cur.sensor_data.current.position;

    if(data_cur.valid && data_cur.mode >= TIMING_CRANKSHAFT_MODE_VALID) {

      if(req_ctx == NULL || req_ctx->position_valid) {
        if(time_recalc) {
          if(data_cur.sensor_data.current.timestamp != data_cur.sensor_data.previous.timestamp) {
            time_delta = time_diff(data_cur.sensor_data.current.timestamp, data_cur.sensor_data.previous.timestamp);
            now = time_diff(now, data_cur.sensor_data.previous.timestamp);

            current = data_cur.sensor_data.current.position;
            previous = data_cur.sensor_data.previous.position;

            if(current < previous) {
              if(phased_internal) {
                current += 720.0f;
              } else {
                current += 360.0f;
              }
            }

            pos = current - previous;
            mult = pos / time_delta;
            pos = mult * now + previous;
          }

          timing_position_clamp(pos, phased_internal, &pos);
        }

        if(req_ctx != NULL) {
          pos_prev = req_ctx->position_prev;

          if((pos - pos_prev < 0.0f && pos - pos_prev > -90.0f) || pos - pos_prev > 90.0f) {
            pos = pos_prev;
          }

          if(phased && !phased_internal && data_cur.mode == TIMING_CRANKSHAFT_MODE_VALID_PHASED) {
            if(pos > 90.0f) {
              req_ctx->phased = true;
              phased_internal = true;
            }
          } else if(req_ctx->phased && !phased_internal && data_cur.mode < TIMING_CRANKSHAFT_MODE_VALID_PHASED) {
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
        pos = data_cur.sensor_data.current.position;
      }

      //Check for NaNs
      if(pos != pos) {
        ctx->diag.crankshaft.bits.pos_calc_nan = true;
        data_cur.valid = false;
        pos = 0.0f;
      }

      if(req_ctx != NULL) {
        req_ctx->position_prev = pos;
        req_ctx->position_valid = true;
      }

      if(!phased_internal) {
        data_cur.mode = MIN(data_cur.mode, TIMING_CRANKSHAFT_MODE_VALID);
      } else if(data_cur.mode > TIMING_CRANKSHAFT_MODE_VALID) {
        data_cur.mode = TIMING_CRANKSHAFT_MODE_VALID_PHASED;
      }

    } else {
      if(req_ctx != NULL) {
        req_ctx->position_prev = pos;
        req_ctx->position_valid = false;
        req_ctx->phased = false;
      }
    }

    data_cur.sensor_data.current_position = pos;
    data_cur.pos_phased = pos;

    if(data != NULL) {
      *data = data_cur;
    }
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
