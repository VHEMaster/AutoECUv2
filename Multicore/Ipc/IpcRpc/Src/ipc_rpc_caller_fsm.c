/*
 * ipc_rpc_caller_fsm.c
 *
 *  Created on: 11 нояб. 2026 г.
 *      Author: VHEMaster
 */

#include "ipc_rpc_caller_fsm.h"
#include "ipc_rpc_caller.h"

error_t ipc_rpc_caller_fsm(ipc_rpc_caller_ctx_t *caller, ipc_rpc_caller_fsm_ctx_t *fsm,
    ipc_rpc_caller_fsm_start_cb_t start, ipc_rpc_caller_fsm_pack_cb_t pack,
    ipc_rpc_caller_fsm_unpack_cb_t unpack, ipc_rpc_caller_fsm_finish_cb_t finish, void *usrdata)
{
  error_t err;
  uint32_t pos;

  RETURN_IF(caller == NULL || fsm == NULL || start == NULL, E_PARAM);

  while(true) {
    switch(fsm->state) {
      case IPC_RPC_CALLER_FSM_IDLE:
        fsm->arg_index = 0;
        fsm->pos = 0;
        fsm->result = E_IPC;
        fsm->state = IPC_RPC_CALLER_FSM_START;
        continue;
      case IPC_RPC_CALLER_FSM_START:
        err = start(&fsm->func_id, &fsm->args_count, usrdata);
        RETURN_IF(err == E_AGAIN, err);
        if(err == E_OK) {
          fsm->state = IPC_RPC_CALLER_FSM_LOCK;
        } else if(err != E_AGAIN) {
          fsm->state = IPC_RPC_CALLER_FSM_IDLE;
          return err;
        }
        continue;
      case IPC_RPC_CALLER_FSM_LOCK:
        err = ipc_rpc_caller_lock(caller);
        if(err == E_OK) {
          if(fsm->args_count > 0) {
            fsm->state = IPC_RPC_CALLER_FSM_BUFFER;
          } else {
            fsm->state = IPC_RPC_CALLER_FSM_CALL;
          }
        } else if(err != E_AGAIN) {
          fsm->state = IPC_RPC_CALLER_FSM_FINISH;
        } else {
          return err;
        }
        continue;
      case IPC_RPC_CALLER_FSM_BUFFER:
        err = ipc_rpc_caller_get_args_buffer(caller, &fsm->capacity, &fsm->buffer);
        if(err == E_OK) {
          fsm->state = IPC_RPC_CALLER_FSM_PACK;
        } else if(err != E_AGAIN) {
          fsm->state = IPC_RPC_CALLER_FSM_UNLOCK;
        } else {
          return err;
        }
        continue;
      case IPC_RPC_CALLER_FSM_PACK:
        if(fsm->arg_index == fsm->args_count) {
          fsm->state = IPC_RPC_CALLER_FSM_CALL;
        } else if(pack != NULL) {
          pos = fsm->pos;
          err = pack(fsm->arg_index, fsm->buffer, fsm->capacity, &pos, usrdata);

          if(err == E_OK) {
            if(pos < fsm->pos || pos > fsm->capacity) {
              fsm->state = IPC_RPC_CALLER_FSM_UNLOCK;
            } else {
              fsm->pos = pos;
              fsm->arg_index++;
            }
          } else if(err != E_AGAIN) {
            fsm->state = IPC_RPC_CALLER_FSM_UNLOCK;
            fsm->result = err;
          } else {
            return err;
          }
        } else {
          fsm->result = E_PARAM;
          fsm->state = IPC_RPC_CALLER_FSM_UNLOCK;
        }
        continue;
      case IPC_RPC_CALLER_FSM_CALL:
        err = ipc_rpc_caller_call(caller, fsm->func_id, fsm->pos, fsm->buffer);
        if(err == E_OK) {
          fsm->state = IPC_RPC_CALLER_FSM_POLL;
        } else if(err != E_AGAIN) {
          fsm->state = IPC_RPC_CALLER_FSM_UNLOCK;
        } else {
          return err;
        }
        continue;
      case IPC_RPC_CALLER_FSM_POLL:
        err = ipc_rpc_caller_poll(caller, &fsm->result, &fsm->resp_count, &fsm->resp_bytes, &fsm->resp);
        if(err == E_OK) {
          fsm->arg_index = 0u;
          fsm->pos = 0u;
          fsm->state = unpack != NULL && fsm->resp_count != 0u
              ? IPC_RPC_CALLER_FSM_UNPACK_RESPONSE : IPC_RPC_CALLER_FSM_UNLOCK;
        } else if(err != E_AGAIN) {
          fsm->result = E_IPC;
          fsm->state = IPC_RPC_CALLER_FSM_UNLOCK;
        } else {
          return err;
        }
        continue;
      case IPC_RPC_CALLER_FSM_UNPACK_RESPONSE:
        pos = fsm->pos;
        err = unpack(fsm->arg_index, fsm->result, fsm->resp, fsm->resp_bytes, &pos, usrdata);
        RETURN_IF(err == E_AGAIN, err);
        if(err == E_OK && (pos < fsm->pos || pos > fsm->resp_bytes)) {
          err = E_PARAM;
        }
        if(err != E_OK) {
          fsm->result = err;
          fsm->state = IPC_RPC_CALLER_FSM_UNLOCK;
        } else {
          fsm->pos = pos;
          if(++fsm->arg_index == fsm->resp_count) {
            if(fsm->pos != fsm->resp_bytes) {
              fsm->result = E_PARAM;
            }
            fsm->state = IPC_RPC_CALLER_FSM_UNLOCK;
          }
        }
        continue;
      case IPC_RPC_CALLER_FSM_UNLOCK:
        fsm->resp = NULL;
        fsm->resp_bytes = 0u;
        fsm->resp_count = 0u;
        err = ipc_rpc_caller_unlock(caller);
        if(err == E_OK) {
          fsm->state = IPC_RPC_CALLER_FSM_FINISH;
        } else if(err != E_AGAIN && err != E_BUSY) {
          fsm->result = E_IPC;
          fsm->state = IPC_RPC_CALLER_FSM_FINISH;
        } else {
          err = E_AGAIN;
          return err;
        }
        continue;
      case IPC_RPC_CALLER_FSM_FINISH:
        if(finish != NULL) {
          finish(fsm->result, usrdata);
        }
        fsm->state = IPC_RPC_CALLER_FSM_IDLE;
        return fsm->result;
      default:
        return E_FAULT;
    }
  }
}
