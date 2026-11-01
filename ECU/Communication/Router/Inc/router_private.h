/*
 * router_private.h
 *
 *  Created on: Oct 29, 2025
 *      Author: VHEMaster
 */

#ifndef COMMUNICATION_ROUTER_INC_ROUTER_PRIVATE_H_
#define COMMUNICATION_ROUTER_INC_ROUTER_PRIVATE_H_

#include "router_types.h"

error_t router_configure_diag(router_ctx_t *ctx);
error_t router_configure_signals(router_ctx_t *ctx);

void router_handle_diag(router_ctx_t *ctx);
void router_handle_signals(router_ctx_t *ctx);

#endif /* COMMUNICATION_ROUTER_INC_ROUTER_PRIVATE_H_ */
