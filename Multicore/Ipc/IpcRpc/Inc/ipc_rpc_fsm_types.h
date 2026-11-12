/*
 * ipc_rpc_fsm_types.h
 *
 *  Created on: 11 нояб. 2026 г.
 *      Author: VHEMaster
 */

#ifndef IPC_IPCRPC_INC_IPC_RPC_FSM_TYPES_H_
#define IPC_IPCRPC_INC_IPC_RPC_FSM_TYPES_H_

#include "ipc_rpc_types.h"

typedef error_t (*ipc_rpc_caller_fsm_start_cb_t)(uint32_t *func_id, uint32_t *args_count, void *usrdata);
typedef error_t (*ipc_rpc_caller_fsm_pack_cb_t)(uint32_t arg_index, void *buffer, uint32_t capacity, uint32_t *p_pos, void *usrdata);
/* E_AGAIN retries the same argument and position. Copy data needed after unlock. */
typedef error_t (*ipc_rpc_caller_fsm_unpack_cb_t)(uint32_t arg_index, error_t result,
    const void *resp, uint32_t resp_bytes, uint32_t *p_pos, void *usrdata);
typedef void (*ipc_rpc_caller_fsm_finish_cb_t)(error_t result, void *usrdata);

typedef error_t (*ipc_rpc_executor_fsm_start_cb_t)(uint32_t func_id, uint32_t *args_count, void *usrdata);
typedef error_t (*ipc_rpc_executor_fsm_unpack_cb_t)(uint32_t arg_index, const void *buffer, uint32_t capacity, uint32_t *p_pos, void *usrdata);
typedef error_t (*ipc_rpc_executor_fsm_execute_cb_t)(uint32_t *p_resp_count, void *usrdata);
/* E_AGAIN retries the same argument and position. A final result must leave no copy in flight. */
typedef error_t (*ipc_rpc_executor_fsm_pack_cb_t)(uint32_t arg_index, void *resp, uint32_t resp_capacity, uint32_t *p_pos, void *usrdata);
typedef void (*ipc_rpc_executor_fsm_finish_cb_t)(void *usrdata);

typedef enum {
  IPC_RPC_CALLER_FSM_IDLE = 0,
  IPC_RPC_CALLER_FSM_START,
  IPC_RPC_CALLER_FSM_LOCK,
  IPC_RPC_CALLER_FSM_BUFFER,
  IPC_RPC_CALLER_FSM_PACK,
  IPC_RPC_CALLER_FSM_CALL,
  IPC_RPC_CALLER_FSM_POLL,
  IPC_RPC_CALLER_FSM_UNPACK_RESPONSE,
  IPC_RPC_CALLER_FSM_UNLOCK,
  IPC_RPC_CALLER_FSM_FINISH,
}ipc_rpc_caller_fsm_state_t;

typedef enum {
  IPC_RPC_EXECUTOR_FSM_IDLE = 0,
  IPC_RPC_EXECUTOR_FSM_LOCK,
  IPC_RPC_EXECUTOR_FSM_POLL,
  IPC_RPC_EXECUTOR_FSM_START,
  IPC_RPC_EXECUTOR_FSM_UNPACK,
  IPC_RPC_EXECUTOR_FSM_EXECUTE,
  IPC_RPC_EXECUTOR_FSM_PACK_RESPONSE,
  IPC_RPC_EXECUTOR_FSM_FINISH,
  IPC_RPC_EXECUTOR_FSM_REPLY,
  IPC_RPC_EXECUTOR_FSM_UNLOCK,
}ipc_rpc_executor_fsm_state_t;

typedef struct {
    ipc_rpc_caller_fsm_state_t state;
    uint32_t func_id;
    uint32_t args_count;
    uint32_t arg_index;
    void *buffer;
    uint32_t capacity;
    const void *resp;
    uint32_t resp_bytes;
    uint32_t resp_count;
    uint32_t pos;
    error_t result;
}ipc_rpc_caller_fsm_ctx_t;

typedef struct {
    ipc_rpc_executor_fsm_state_t state;
    uint32_t func_id;
    uint32_t args_count;
    uint32_t arg_index;
    const void *buffer;
    uint32_t args_bytes;
    void *resp;
    uint32_t resp_capacity;
    uint32_t resp_count;
    uint32_t resp_index;
    uint32_t resp_bytes;
    uint32_t pos;
    bool callback_pending;
    error_t result;
}ipc_rpc_executor_fsm_ctx_t;

#endif /* IPC_IPCRPC_INC_IPC_RPC_FSM_TYPES_H_ */
