/*
 * kwp.h
 *
 *  Created on: Oct 23, 2025
 *      Author: vhemaster
 */

#ifndef COMMUNICATION_KWP_INC_KWP_H_
#define COMMUNICATION_KWP_INC_KWP_H_

#include "kwp_types.h"

error_t kwp_init(kwp_ctx_t *ctx, const kwp_init_ctx_t *init);
error_t kwp_configure(kwp_ctx_t *ctx, const kwp_config_t *config);
void kwp_loop_main(kwp_ctx_t *ctx);
void kwp_loop_slow(kwp_ctx_t *ctx);
void kwp_loop_comm(kwp_ctx_t *ctx);

error_t kwp_reset(kwp_ctx_t *ctx);

#endif /* COMMUNICATION_KWP_INC_KWP_H_ */
