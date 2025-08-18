/*
 * core_powermoding.h
 *
 *  Created on: Aug 16, 2025
 *      Author: VHEMaster
 */

#ifndef CORE_POWERMODING_INC_POWERMODING_H_
#define CORE_POWERMODING_INC_POWERMODING_H_

#include "core.h"
#include "core_powermoding_types.h"

error_t core_powermoding_init(ecu_core_ctx_t *ctx);
void core_powermoding_loop_slow(ecu_core_ctx_t *ctx);

error_t core_powermoding_user_mode_request(ecu_core_ctx_t *ctx, ecu_core_powermoding_user_t user, ecu_core_powermoding_mode_request_t mode_request);
error_t core_powermoding_user_mode_reset(ecu_core_ctx_t *ctx, ecu_core_powermoding_user_t user);
error_t core_powermoding_mode_get(ecu_core_ctx_t *ctx, ecu_core_powermoding_mode_t *mode);

#endif /* CORE_POWERMODING_INC_POWERMODING_H_ */
