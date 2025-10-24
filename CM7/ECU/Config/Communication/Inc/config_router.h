/*
 * config_router.h
 *
 *  Created on: Oct 23, 2025
 *      Author: vhemaster
 */

#ifndef CONFIG_COMMUNICATION_INC_CONFIG_ROUTER_H_
#define CONFIG_COMMUNICATION_INC_CONFIG_ROUTER_H_

#include "config_comm.h"
#include "router.h"

error_t ecu_comm_router_init(ecu_comm_router_t instance, router_ctx_t *ctx);
error_t ecu_comm_router_get_default_config(ecu_comm_router_t instance, router_config_t *config);
error_t ecu_comm_router_configure(ecu_comm_router_t instance, const router_config_t *config);
error_t ecu_comm_router_reset(ecu_comm_router_t instance);

#endif /* CONFIG_COMMUNICATION_INC_CONFIG_ROUTER_H_ */
