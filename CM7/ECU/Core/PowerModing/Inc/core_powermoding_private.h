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

void powermoding_switch_standby(ecu_core_ctx_t *ctx);
void powermoding_switch_sleep(ecu_core_ctx_t *ctx);
void powermoding_get_mode_requested(ecu_core_ctx_t *ctx);

#endif /* CORE_POWERMODING_INC_POWERMODING_PRIVATE_H_ */
