/*
 * lwip.h
 *
 *  Created on: 17 нояб. 2026 г.
 *      Author: VHEMaster
 */

#ifndef COMMUNICATION_LWIP_INC_LWIP_H_
#define COMMUNICATION_LWIP_INC_LWIP_H_

#include "lwip_types.h"

error_t lwip_init(lwip_ctx_t *ctx, const lwip_init_ctx_t *init);
error_t lwip_configure(lwip_ctx_t *ctx, const lwip_config_t *config);
void lwip_loop_main(lwip_ctx_t *ctx);
void lwip_loop_slow(lwip_ctx_t *ctx);
void lwip_loop_comm(lwip_ctx_t *ctx);

error_t lwip_reset(lwip_ctx_t *ctx);

#endif /* COMMUNICATION_LWIP_INC_LWIP_H_ */
