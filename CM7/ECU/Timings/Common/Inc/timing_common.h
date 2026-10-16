/*
 * timing_common.h
 *
 *  Created on: Aug 14, 2024
 *      Author: VHEMaster
 */

#ifndef CORE_CORE_INC_CORE_TIMING_COMMON_H_
#define CORE_CORE_INC_CORE_TIMING_COMMON_H_

#include "core.h"
#include "timing_base.h"

typedef enum {
  TIMING_LATE_POLICY_KEEP_WIDTH = 0,
  TIMING_LATE_POLICY_KEEP_END,
  TIMING_LATE_POLICY_MAX
}timing_late_policy_t;

error_t timing_pulse_schedule(ecu_gpio_output_pin_t output_pin, time_us_t pulse_start, time_us_t pulse_end, timing_late_policy_t late_policy);

error_t timing_calculate_current_position(timing_base_ctx_t *ctx, float offset, bool phased, timing_base_req_t *req_ctx, timing_base_data_crankshaft_t *data);
error_t timing_calculate_offset_position(timing_base_ctx_t *ctx, float offset, bool phased, timing_base_req_t *req_ctx, timing_base_data_crankshaft_t *data);

error_t timing_calculate_current_position_from_snapshot(timing_base_ctx_t *ctx, const timing_base_data_crankshaft_t *snapshot, float offset, bool phased, timing_base_req_t *req_ctx, timing_base_data_crankshaft_t *data);
error_t timing_calculate_offset_position_from_snapshot(timing_base_ctx_t *ctx, const timing_base_data_crankshaft_t *snapshot, float offset, bool phased, timing_base_req_t *req_ctx, timing_base_data_crankshaft_t *data);

error_t timing_calculate_cylinders_positions(timing_base_ctx_t *ctx, bool sequential, bool semisequential);

OPTIMIZE_FAST
STATIC_INLINE void timing_position_clamp(float input, bool phased, float *output)
{
  float result = input;
  float turns;

  do {
    BREAK_IF(output == NULL);

    if(phased) {
      turns = floorf((input - 360.0f) * (1.0f / 720.0f)) + 1.0f;
      result = input - turns * 720.0f;

      if(result < -360.0f)
          result += 720.0f;

      if(result >= 360.0f)
          result -= 720.0f;
    } else {
      turns = floorf((input - 180.0f) * (1.0f / 360.0f)) + 1.0f;
      result = input - turns * 360.0f;

      if(result < -180.0f)
          result += 360.0f;

      if(result >= 180.0f)
          result -= 360.0f;
    }

    *output = result;
  } while(0);
}

#endif /* CORE_CORE_INC_CORE_TIMING_COMMON_H_ */
