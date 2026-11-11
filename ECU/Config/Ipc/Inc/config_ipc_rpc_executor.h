/*
 * config_ipc_rpc_executor.h
 *
 *  Created on: 10 нояб. 2026 г.
 *      Author: VHEMaster
 */

#ifndef CONFIG_IPC_INC_CONFIG_IPC_RPC_EXECUTOR_H_
#define CONFIG_IPC_INC_CONFIG_IPC_RPC_EXECUTOR_H_

#include "config_ipc.h"
#include "ipc_rpc_executor.h"
#include "ipc_rpc_executor_fsm.h"

error_t ecu_ipc_rpc_executor_init(ecu_ipc_rpc_executor_t instance, ipc_rpc_executor_ctx_t *ctx);
error_t ecu_ipc_rpc_executor_register_func(ecu_ipc_rpc_executor_t instance, uint32_t func_id, uint32_t args_count,
    ipc_rpc_executor_fsm_unpack_cb_t unpack, ipc_rpc_executor_fsm_execute_cb_t execute, void *usrdata);

void ecu_ipc_rpc_executor_loop(ecu_ipc_rpc_executor_t instance);

#endif /* CONFIG_IPC_INC_CONFIG_IPC_RPC_EXECUTOR_H_ */
