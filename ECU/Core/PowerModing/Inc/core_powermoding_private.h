/*
 * core_powermoding_private.h
 *
 *  Created on: Sep 28, 2025
 *      Author: VHEMaster
 */

#ifndef CORE_POWERMODING_INC_POWERMODING_PRIVATE_H_
#define CORE_POWERMODING_INC_POWERMODING_PRIVATE_H_

#include "common.h"
#include "core.h"
#include "core_powermoding_types.h"

void powermoding_switch_standby(ecu_core_ctx_t *ctx);
void powermoding_switch_sleep(ecu_core_ctx_t *ctx);
void powermoding_get_mode_requested(ecu_core_ctx_t *ctx);
bool powermoding_get_state_flag(ecu_core_ctx_t *ctx, ecu_core_powermoding_state_flag_t mask);
void powermoding_set_state_flag(ecu_core_ctx_t *ctx, ecu_core_powermoding_state_flag_t mask);
void powermoding_clear_state_flag(ecu_core_ctx_t *ctx, ecu_core_powermoding_state_flag_t mask);
bool powermoding_is_runup_allowed(ecu_core_ctx_t *ctx);

#endif /* CORE_POWERMODING_INC_POWERMODING_PRIVATE_H_ */
