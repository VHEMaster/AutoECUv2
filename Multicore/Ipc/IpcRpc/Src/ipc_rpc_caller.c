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
    BREAK_IF_ACTION(ctx->pending, err = E_BUSY);

    ATOMIC_RELEASE(ctx->lock);

  } while(0);

  return err;
}

error_t ipc_rpc_caller_call(ipc_rpc_caller_ctx_t *ctx, uint32_t func_id, uint32_t args_bytes, const void *args)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || (args_bytes != 0u && args == NULL), err = E_PARAM);
    BREAK_IF_ACTION(ATOMIC_ISLOCKED(ctx->lock) == false, err = E_INVALACT);
    BREAK_IF_ACTION(ctx->pending, err = E_BUSY);
    BREAK_IF_ACTION(args_bytes > ctx->buffer_size, err = E_OVERFLOW);
    BREAK_IF_ACTION(ATOMIC_LOAD(ctx->header->response_id) != ctx->request_id, err = E_AGAIN);

    if(args_bytes != 0u) {
      memcpy(ctx->buffer, args, args_bytes);
    }

    ctx->request_id++;
    ctx->pending = true;
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

  do {
    BREAK_IF_ACTION(ctx == NULL || p_ret == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ATOMIC_ISLOCKED(ctx->lock) == false || ctx->pending == false, err = E_INVALACT);
    BREAK_IF_ACTION(ATOMIC_LOAD(ctx->header->response_id) != ctx->request_id, err = E_AGAIN);

    ctx->pending = false;
    BREAK_IF_ACTION(ATOMIC_LOAD_RELAXED(ctx->header->status) != E_OK, err = E_IPC);
    *p_ret = ATOMIC_LOAD_RELAXED(ctx->header->ret);

  } while(0);

  return err;
}

error_t ipc_rpc_caller_abort(ipc_rpc_caller_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ATOMIC_ISLOCKED(ctx->lock) == false || ctx->pending == false, err = E_INVALACT);

    ATOMIC_STORE(ctx->header->aborted, true);
    ctx->pending = false;

  } while(0);

  return err;
}
