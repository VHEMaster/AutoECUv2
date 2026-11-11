/*
 * ipc_rpc_caller_fsm.h
 *
 *  Created on: 11 нояб. 2026 г.
 *      Author: VHEMaster
 */

#ifndef IPC_IPCRPC_INC_IPC_RPC_CALLER_FSM_H_
#define IPC_IPCRPC_INC_IPC_RPC_CALLER_FSM_H_

#include "ipc_rpc_fsm_types.h"

error_t ipc_rpc_caller_fsm(ipc_rpc_caller_ctx_t *caller, ipc_rpc_caller_fsm_ctx_t *fsm,
    ipc_rpc_caller_fsm_start_cb_t start, ipc_rpc_caller_fsm_pack_cb_t pack,
    ipc_rpc_caller_fsm_finish_cb_t finish, void *usrdata);

#endif /* IPC_IPCRPC_INC_IPC_RPC_CALLER_FSM_H_ */
