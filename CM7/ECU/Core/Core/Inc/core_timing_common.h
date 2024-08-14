/*
 * core_timing_common.h
 *
 *  Created on: Aug 14, 2024
 *      Author: VHEMaster
 */

#ifndef CORE_CORE_INC_CORE_TIMING_COMMON_H_
#define CORE_CORE_INC_CORE_TIMING_COMMON_H_

#include "core.h"

error_t core_timing_pulse_schedule(ecu_core_ctx_t *ctx, ecu_gpio_output_pin_t output_pin, time_us_t pulse_start, time_us_t pulse_end);

#endif /* CORE_CORE_INC_CORE_TIMING_COMMON_H_ */
