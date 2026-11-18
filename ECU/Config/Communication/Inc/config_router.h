/*
 * config_router.h
 *
 *  Created on: Oct 23, 2025
 *      Author: vhemaster
 */

#ifndef CONFIG_COMMUNICATION_INC_CONFIG_ROUTER_H_
#define CONFIG_COMMUNICATION_INC_CONFIG_ROUTER_H_

#include "config_comm.h"
#include "versioned_router.h"
#ifdef CORE_CM4
#include "router.h"
#endif /* CORE_CM4 */

#ifdef CORE_CM4
error_t ecu_comm_router_init(ecu_comm_router_t instance, router_ctx_t *ctx);
#endif /* CORE_CM4 */
error_t ecu_comm_router_get_default_config(ecu_comm_router_t instance, router_config_t *config);
error_t ecu_comm_router_configure(ecu_comm_router_t instance, const router_config_t *config);
error_t ecu_comm_router_reset(ecu_comm_router_t instance);

#ifdef CORE_CM7
error_t ecu_comm_router_translate_v1_to_v2(const void *src, void *dest, uint32_t dest_bytes);
#endif /* CORE_CM7 */

#endif /* CONFIG_COMMUNICATION_INC_CONFIG_ROUTER_H_ */
