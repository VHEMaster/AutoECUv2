/*
 * router.h
 *
 *  Created on: Oct 23, 2025
 *      Author: vhemaster
 */

#ifndef COMMUNICATION_ROUTER_INC_ROUTER_H_
#define COMMUNICATION_ROUTER_INC_ROUTER_H_

#include "can_types.h"
#include "router_types.h"

error_t router_init(router_ctx_t *ctx, const router_init_ctx_t *init);
error_t router_configure(router_ctx_t *ctx, const router_config_t *config);
void router_loop_main(router_ctx_t *ctx);
void router_loop_slow(router_ctx_t *ctx);
void router_loop_comm(router_ctx_t *ctx);

error_t router_reset(router_ctx_t *ctx);

error_t router_signal_transmit(router_ctx_t *ctx, const can_message_t *message);

#endif /* COMMUNICATION_ROUTER_INC_ROUTER_H_ */
