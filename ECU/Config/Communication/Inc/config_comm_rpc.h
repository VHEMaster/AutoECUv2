/*
 * config_comm_rpc.h
 *
 *  Created on: 11 нояб. 2026 г.
 */

#ifndef CONFIG_COMMUNICATION_INC_CONFIG_COMM_RPC_H_
#define CONFIG_COMMUNICATION_INC_CONFIG_COMM_RPC_H_

#include "config_comm_types.h"
#include "config_ipc_types.h"
#include "ipc_rpc_caller_fsm.h"

typedef struct {
    ipc_rpc_caller_ctx_t *caller;
    ipc_rpc_caller_fsm_ctx_t fsm;
    ecu_ipc_rpc_func_t func;
    ecu_comm_instance_t instance;
    const void *argument;
    uint32_t bytes;
    uint32_t alignment;
}ecu_comm_rpc_ctx_t;

error_t ecu_comm_rpc_call(ecu_comm_rpc_ctx_t *ctx, ecu_ipc_rpc_func_t func,
    ecu_comm_instance_t instance, const void *argument, uint32_t bytes, uint32_t alignment);

#endif /* CONFIG_COMMUNICATION_INC_CONFIG_COMM_RPC_H_ */
