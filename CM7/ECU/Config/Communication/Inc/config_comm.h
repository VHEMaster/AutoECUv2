/*
 * config_comm.h
 *
 *  Created on: Oct 23, 2025
 *      Author: vhemaster
 */

#ifndef CONFIG_COMMUNICATION_INC_CONFIG_COMM_H_
#define CONFIG_COMMUNICATION_INC_CONFIG_COMM_H_

#include "errors.h"
#include "can.h"
#include "kwp.h"
#include "isotp.h"
#include "uds.h"
#include "obd2.h"
#include "router.h"

#include "config_comm_types.h"

typedef void (*ecu_comm_loop_func_t)(void *ctx);

error_t ecu_comm_init(void);
void ecu_comm_loop_main(void);
void ecu_comm_loop_slow(void);
void ecu_comm_loop_comm(void);

error_t ecu_comm_get_comm_ctx(ecu_comm_type_t type, ecu_comm_instance_t instance, void **ctx);
error_t ecu_comm_set_comm_initialized(ecu_comm_type_t type, ecu_comm_instance_t instance, bool initialized);

error_t ecu_comm_get_can_ctx(ecu_comm_can_t instance, can_ctx_t **ctx);
error_t ecu_comm_get_kwp_ctx(ecu_comm_kwp_t instance, kwp_ctx_t **ctx);
error_t ecu_comm_get_isotp_ctx(ecu_comm_isotp_t instance, isotp_ctx_t **ctx);
error_t ecu_comm_get_uds_ctx(ecu_comm_uds_t instance, uds_ctx_t **ctx);
error_t ecu_comm_get_obd2_ctx(ecu_comm_obd2_t instance, obd2_ctx_t **ctx);
error_t ecu_comm_get_router_ctx(ecu_comm_router_t instance, router_ctx_t **ctx);

#endif /* CONFIG_COMMUNICATION_INC_CONFIG_COMM_H_ */
