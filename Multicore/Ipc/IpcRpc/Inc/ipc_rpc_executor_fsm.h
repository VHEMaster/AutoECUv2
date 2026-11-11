/*
 * ipc_rpc_executor_fsm.h
 *
 *  Created on: 11 нояб. 2026 г.
 *      Author: VHEMaster
 */

#ifndef IPC_IPCRPC_INC_IPC_RPC_EXECUTOR_FSM_H_
#define IPC_IPCRPC_INC_IPC_RPC_EXECUTOR_FSM_H_

#include "ipc_rpc_fsm_types.h"

error_t ipc_rpc_executor_fsm(ipc_rpc_executor_ctx_t *executor, ipc_rpc_executor_fsm_ctx_t *fsm,
    ipc_rpc_executor_fsm_start_cb_t start, ipc_rpc_executor_fsm_unpack_cb_t unpack,
    ipc_rpc_executor_fsm_execute_cb_t execute, ipc_rpc_executor_fsm_finish_cb_t finish,
    void *usrdata);

#endif /* IPC_IPCRPC_INC_IPC_RPC_EXECUTOR_FSM_H_ */
