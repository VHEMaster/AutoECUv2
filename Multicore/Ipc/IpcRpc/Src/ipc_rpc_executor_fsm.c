/*
 * ipc_rpc_executor_fsm.c
 *
 *  Created on: 11 нояб. 2026 г.
 *      Author: VHEMaster
 */

#include "ipc_rpc_executor_fsm.h"
#include "ipc_rpc_executor.h"

void ipc_rpc_executor_fsm(ipc_rpc_executor_ctx_t *executor, ipc_rpc_executor_fsm_ctx_t *fsm,
    ipc_rpc_executor_fsm_start_cb_t start, ipc_rpc_executor_fsm_unpack_cb_t unpack,
    ipc_rpc_executor_fsm_execute_cb_t execute, ipc_rpc_executor_fsm_finish_cb_t finish,
    void *usrdata)
{
  error_t err;
  uint32_t pos;

  if(executor == NULL || fsm == NULL || start == NULL || execute == NULL) {
    return;
  }

  while(true) {
    switch(fsm->state) {
      case IPC_RPC_EXECUTOR_FSM_IDLE:
        fsm->state = IPC_RPC_EXECUTOR_FSM_LOCK;
        continue;
      case IPC_RPC_EXECUTOR_FSM_LOCK:
        err = ipc_rpc_executor_lock(executor);
        BREAK_IF(err != E_OK);
        fsm->state = IPC_RPC_EXECUTOR_FSM_POLL;
        continue;
      case IPC_RPC_EXECUTOR_FSM_POLL:
        err = ipc_rpc_executor_poll(executor, &fsm->func_id, &fsm->args_bytes, &fsm->buffer);
        BREAK_IF(err != E_OK);
        fsm->args_count = 0u;
        fsm->arg_index = 0u;
        fsm->pos = 0u;
        fsm->callback_pending = false;
        fsm->state = IPC_RPC_EXECUTOR_FSM_START;
        continue;
      case IPC_RPC_EXECUTOR_FSM_START:
      case IPC_RPC_EXECUTOR_FSM_UNPACK:
        if(!fsm->callback_pending && ATOMIC_LOAD(executor->header->aborted)) {
          fsm->result = E_ABORTED;
          fsm->state = IPC_RPC_EXECUTOR_FSM_FINISH;
          continue;
        }

        pos = fsm->pos;
        if(fsm->state == IPC_RPC_EXECUTOR_FSM_START) {
          err = start(fsm->func_id, &fsm->args_count, usrdata);
        } else {
          err = unpack(fsm->arg_index, fsm->buffer, fsm->args_bytes, &pos, usrdata);
        }

        fsm->callback_pending = err == E_AGAIN;
        BREAK_IF(fsm->callback_pending);

        if(ATOMIC_LOAD(executor->header->aborted)) {
          err = E_ABORTED;
        }

        if(err != E_OK) {
          fsm->result = err;
          fsm->state = IPC_RPC_EXECUTOR_FSM_FINISH;
        } else if(fsm->state == IPC_RPC_EXECUTOR_FSM_START) {
          if(fsm->args_count == 0u) {
            fsm->state = IPC_RPC_EXECUTOR_FSM_EXECUTE;
          } else if(unpack == NULL) {
            fsm->result = E_PARAM;
            fsm->state = IPC_RPC_EXECUTOR_FSM_FINISH;
          } else {
            fsm->state = IPC_RPC_EXECUTOR_FSM_UNPACK;
          }
        } else if(pos < fsm->pos || pos > fsm->args_bytes) {
          fsm->result = E_PARAM;
          fsm->state = IPC_RPC_EXECUTOR_FSM_FINISH;
        } else {
          fsm->pos = pos;
          if(++fsm->arg_index == fsm->args_count) {
            fsm->state = IPC_RPC_EXECUTOR_FSM_EXECUTE;
          }
        }
        continue;
      case IPC_RPC_EXECUTOR_FSM_EXECUTE:
        if(ATOMIC_LOAD(executor->header->aborted)) {
          err = E_ABORTED;
        } else if(fsm->pos != fsm->args_bytes) {
          err = E_PARAM;
        } else {
          err = execute(usrdata);
        }
        if(err != E_AGAIN) {
          fsm->result = err;
          fsm->state = IPC_RPC_EXECUTOR_FSM_FINISH;
        } else {
          break;
        }
        continue;
      case IPC_RPC_EXECUTOR_FSM_FINISH:
        if(finish != NULL) {
          finish(usrdata);
        }
        fsm->state = IPC_RPC_EXECUTOR_FSM_REPLY;
        continue;
      case IPC_RPC_EXECUTOR_FSM_REPLY:
        err = ipc_rpc_executor_return(executor, fsm->result);
        BREAK_IF(err != E_OK);
        fsm->buffer = NULL;
        fsm->state = IPC_RPC_EXECUTOR_FSM_POLL;
        break;
      case IPC_RPC_EXECUTOR_FSM_UNLOCK:
        err = ipc_rpc_executor_unlock(executor);
        BREAK_IF(err != E_OK);
        fsm->state = IPC_RPC_EXECUTOR_FSM_IDLE;
        break;
      default:
        break;
    }
    break;
  }
}
