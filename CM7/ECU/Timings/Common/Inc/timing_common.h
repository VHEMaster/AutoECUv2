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

error_t timing_pulse_schedule(ecu_gpio_output_pin_t output_pin, time_us_t pulse_start, time_us_t pulse_end);
void timing_position_clamp(float input, bool phased, float *output);

error_t timing_calculate_current_position(timing_base_ctx_t *ctx, float offset, bool phased, timing_base_req_t *req_ctx, timing_base_data_crankshaft_t *data);
error_t timing_calculate_offset_position(timing_base_ctx_t *ctx, float offset, bool phased, timing_base_req_t *req_ctx, timing_base_data_crankshaft_t *data);

#endif /* CORE_CORE_INC_CORE_TIMING_COMMON_H_ */
