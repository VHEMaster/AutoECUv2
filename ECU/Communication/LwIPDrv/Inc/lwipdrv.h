/*
 * lwipdrv.h
 *
 *  Created on: 17 нояб. 2026 г.
 *      Author: VHEMaster
 */

#ifndef COMMUNICATION_LWIPDRV_INC_LWIPDRV_H_
#define COMMUNICATION_LWIPDRV_INC_LWIPDRV_H_

#include "lwipdrv_types.h"

error_t lwipdrv_init(lwipdrv_ctx_t *ctx, const lwipdrv_init_ctx_t *init);
error_t lwipdrv_configure(lwipdrv_ctx_t *ctx, const lwipdrv_config_t *config);
void lwipdrv_loop_main(lwipdrv_ctx_t *ctx);
void lwipdrv_loop_slow(lwipdrv_ctx_t *ctx);
void lwipdrv_loop_comm(lwipdrv_ctx_t *ctx);

error_t lwipdrv_reset(lwipdrv_ctx_t *ctx);

#endif /* COMMUNICATION_LWIPDRV_INC_LWIPDRV_H_ */
