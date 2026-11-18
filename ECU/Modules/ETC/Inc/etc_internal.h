/*
 * etc_internal.h
 *
 *  Created on: May 16, 2024
 *      Author: VHEMaster
 */

#ifndef MODULES_ETC_INC_ETC_INTERNAL_H_
#define MODULES_ETC_INC_ETC_INTERNAL_H_

#include "etc.h"

error_t etc_internal_process(etc_ctx_t *ctx);
error_t etc_internal_sample(etc_ctx_t *ctx, time_us_t now);
error_t etc_internal_drive(etc_ctx_t *ctx, bool enabled, float effort);
void etc_internal_reset_control(etc_ctx_t *ctx, time_us_t now);
float etc_internal_control(etc_ctx_t *ctx, const etc_adaptation_config_t *model, float target, float limit, time_us_t now);
bool etc_internal_adaptation_config_valid(const etc_adaptation_config_t *config);
bool etc_internal_number_valid(float value);
void etc_adaptation_loop(etc_ctx_t *ctx);

#endif /* MODULES_ETC_INC_ETC_INTERNAL_H_ */
