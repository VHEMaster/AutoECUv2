/*
 * ipc_rpc_caller.c
 *
 *  Created on: 9 нояб. 2026 г.
 *      Author: VHEMaster
 */

#include "ipc_rpc_caller.h"
#include <string.h>

error_t ipc_rpc_caller_init(ipc_rpc_caller_ctx_t *ctx, ipc_rpc_header_ctx_t *header, uint32_t buffer_size, void *buffer)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || header == NULL, err = E_PARAM);
    BREAK_IF_ACTION(buffer_size != 0u && buffer == NULL, err = E_PARAM);

    memset(ctx, 0u, sizeof(ipc_rpc_caller_ctx_t));
    ctx->header = header;
    ctx->buffer = buffer;
    ctx->buffer_size = buffer_size;

    memset(header, 0u, sizeof(ipc_rpc_header_ctx_t));
    ATOMIC_STORE(header->unique_val, IPC_RPC_UNIQUE_VAL);

  } while(0);

  return err;
}

error_t ipc_rpc_caller_set_memcpy(ipc_rpc_caller_ctx_t *ctx, uint32_t threshold_bytes,
    ipc_memcpy_cb_t memcpy_cb, void *memcpy_usrdata)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->state != IPC_RPC_CALLER_STATE_IDLE &&
        ctx->state != IPC_RPC_CALLER_STATE_WAIT_ABORT_ACK, err = E_INVALACT);

    ctx->memcpy_cb = memcpy_cb;
    ctx->memcpy_usrdata = memcpy_usrdata;
    ctx->memcpy_threshold = threshold_bytes;

  } while(0);

  return err;
}

error_t ipc_rpc_caller_lock(ipc_rpc_caller_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->header == NULL, err = E_INVALACT);
    BREAK_IF_ACTION(ATOMIC_TRY_LOCK(ctx->lock) == false, err = E_AGAIN);

  } while(0);

  return err;
}

error_t ipc_rpc_caller_unlock(ipc_rpc_caller_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ATOMIC_ISLOCKED(ctx->lock) == false, err = E_INVALACT);
    BREAK_IF_ACTION(ctx->state != IPC_RPC_CALLER_STATE_IDLE &&
        ctx->state != IPC_RPC_CALLER_STATE_WAIT_ABORT_ACK, err = E_BUSY);

    ATOMIC_RELEASE(ctx->lock);

  } while(0);

  return err;
}

error_t ipc_rpc_caller_get_args_buffer(ipc_rpc_caller_ctx_t *ctx, uint32_t *p_args_bytes, void **args)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || p_args_bytes == NULL || args == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ATOMIC_ISLOCKED(ctx->lock) == false, err = E_INVALACT);
    BREAK_IF_ACTION(ctx->state != IPC_RPC_CALLER_STATE_IDLE &&
        ctx->state != IPC_RPC_CALLER_STATE_WAIT_ABORT_ACK, err = E_BUSY);
    BREAK_IF_ACTION(ATOMIC_LOAD(ctx->header->response_id) != ctx->request_id, err = E_AGAIN);

    *p_args_bytes = ctx->buffer_size;
    *args = ctx->buffer;

  } while(0);

  return err;
}

error_t ipc_rpc_caller_call(ipc_rpc_caller_ctx_t *ctx, uint32_t func_id, uint32_t args_bytes, const void *args)
{
  error_t err = E_OK;
  ipc_memcpy_action_t action = IPC_MEMCPY_START;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(args_bytes != 0u && args == NULL, err = E_PARAM);
    BREAK_IF_ACTION(args_bytes > ctx->buffer_size, err = E_OVERFLOW);
    BREAK_IF_ACTION(ATOMIC_ISLOCKED(ctx->lock) == false, err = E_INVALACT);

    switch(ctx->state) {
      case IPC_RPC_CALLER_STATE_IDLE:
      case IPC_RPC_CALLER_STATE_WAIT_ABORT_ACK:
        BREAK_IF_ACTION(ATOMIC_LOAD(ctx->header->response_id) != ctx->request_id, err = E_AGAIN);
        break;
      case IPC_RPC_CALLER_STATE_COPYING:
        action = IPC_MEMCPY_POLL;
        break;
      case IPC_RPC_CALLER_STATE_WAIT_RESPONSE:
        err = E_BUSY;
        break;
      default:
        err = E_INVALACT;
        break;
    }
    BREAK_IF(err != E_OK);

    if(args_bytes != 0u && args != ctx->buffer) {
      if(ctx->memcpy_cb != NULL && args_bytes >= ctx->memcpy_threshold) {
        if(action == IPC_MEMCPY_START) {
          ctx->state = IPC_RPC_CALLER_STATE_COPYING;
        }

        err = ctx->memcpy_cb(ctx->buffer, args, args_bytes, action, ctx->memcpy_usrdata);
        BREAK_IF(err == E_AGAIN);

        if(err != E_OK) {
          memcpy(ctx->buffer, args, args_bytes);
          err = E_OK;
        }
      } else {
        memcpy(ctx->buffer, args, args_bytes);
      }
    }

    ctx->request_id++;
    ctx->state = IPC_RPC_CALLER_STATE_WAIT_RESPONSE;
    ATOMIC_STORE_RELAXED(ctx->header->aborted, false);
    ATOMIC_STORE_RELAXED(ctx->header->func_id, func_id);
    ATOMIC_STORE_RELAXED(ctx->header->args_bytes, args_bytes);
    ATOMIC_STORE(ctx->header->request_id, ctx->request_id);

  } while(0);

  return err;
}

error_t ipc_rpc_caller_poll(ipc_rpc_caller_ctx_t *ctx, error_t *p_ret)
{
  error_t err = E_OK;
  error_t ret;

  do {
    BREAK_IF_ACTION(ctx == NULL || p_ret == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ATOMIC_ISLOCKED(ctx->lock) == false, err = E_INVALACT);
    BREAK_IF_ACTION(ctx->state != IPC_RPC_CALLER_STATE_WAIT_RESPONSE, err = E_INVALACT);
    BREAK_IF_ACTION(ATOMIC_LOAD(ctx->header->response_id) != ctx->request_id, err = E_AGAIN);

    ctx->state = IPC_RPC_CALLER_STATE_IDLE;
    ret = ATOMIC_LOAD_RELAXED(ctx->header->status);
    BREAK_IF_ACTION(ret != E_OK, err = ret);

    ret = ATOMIC_LOAD_RELAXED(ctx->header->ret);
    *p_ret = ret;

  } while(0);

  return err;
}

error_t ipc_rpc_caller_abort(ipc_rpc_caller_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ATOMIC_ISLOCKED(ctx->lock) == false, err = E_INVALACT);
    BREAK_IF_ACTION(ctx->state == IPC_RPC_CALLER_STATE_COPYING, err = E_BUSY);
    BREAK_IF_ACTION(ctx->state != IPC_RPC_CALLER_STATE_WAIT_RESPONSE, err = E_INVALACT);

    ATOMIC_STORE(ctx->header->aborted, true);
    ctx->state = IPC_RPC_CALLER_STATE_WAIT_ABORT_ACK;

  } while(0);

  return err;
}
