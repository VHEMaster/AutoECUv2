/*
 * config_comm.h
 *
 *  Created on: Oct 23, 2025
 *      Author: vhemaster
 */

#ifndef CONFIG_COMMUNICATION_INC_CONFIG_COMM_H_
#define CONFIG_COMMUNICATION_INC_CONFIG_COMM_H_

#include "errors.h"
#include "isotp.h"

#include "config_comm_types.h"

typedef void (*ecu_comm_loop_func_t)(void *ctx);

error_t ecu_comm_init(void);
void ecu_comm_loop_main(void);
void ecu_comm_loop_slow(void);
void ecu_comm_loop_comm(void);

error_t ecu_comm_get_comm_ctx(ecu_comm_type_t type, ecu_comm_instance_t instance, void **ctx);
error_t ecu_comm_set_comm_initialized(ecu_comm_type_t type, ecu_comm_instance_t instance, bool initialized);

error_t ecu_comm_get_isotp_ctx(ecu_comm_isotp_t instance, isotp_ctx_t **ctx);

#endif /* CONFIG_COMMUNICATION_INC_CONFIG_COMM_H_ */
