/*
 * ipc_rpc_executor.c
 *
 *  Created on: 9 нояб. 2026 г.
 *      Author: VHEMaster
 */

#include "ipc_rpc_executor.h"
#include <string.h>

static void ipc_rpc_executor_publish(ipc_rpc_header_ctx_t *header, uint32_t request_id,
    error_t status, error_t ret);

error_t ipc_rpc_executor_init(ipc_rpc_executor_ctx_t *ctx, ipc_rpc_header_ctx_t *header, uint32_t buffer_size, const void *buffer)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || header == NULL, err = E_PARAM);
    BREAK_IF_ACTION(buffer_size != 0u && buffer == NULL, err = E_PARAM);

    memset(ctx, 0u, sizeof(ipc_rpc_executor_ctx_t));
    ctx->header = header;
    ctx->buffer = buffer;
    ctx->buffer_size = buffer_size;

  } while(0);

  return err;
}

error_t ipc_rpc_executor_lock(ipc_rpc_executor_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->header == NULL, err = E_INVALACT);
    BREAK_IF_ACTION(ATOMIC_TRY_LOCK(ctx->lock) == false, err = E_AGAIN);

  } while(0);

  return err;
}

error_t ipc_rpc_executor_unlock(ipc_rpc_executor_ctx_t *ctx)
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

error_t ipc_rpc_executor_poll(ipc_rpc_executor_ctx_t *ctx, uint32_t *p_func_id, uint32_t *p_args_bytes, const void **args)
{
  error_t err = E_OK;
  uint32_t request_id, args_bytes;

  do {
    BREAK_IF_ACTION(ctx == NULL || p_func_id == NULL || p_args_bytes == NULL || args == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ATOMIC_ISLOCKED(ctx->lock) == false, err = E_INVALACT);
    BREAK_IF_ACTION(ctx->pending, err = E_BUSY);
    BREAK_IF_ACTION(ATOMIC_LOAD(ctx->header->unique_val) != IPC_RPC_UNIQUE_VAL, err = E_AGAIN);

    request_id = ATOMIC_LOAD(ctx->header->request_id);
    BREAK_IF_ACTION(request_id == ATOMIC_LOAD_RELAXED(ctx->header->response_id), err = E_AGAIN);

    if(ATOMIC_LOAD(ctx->header->aborted) != false) {
      ipc_rpc_executor_publish(ctx->header, request_id, E_ABORTED, E_OK);
      err = E_AGAIN;
      break;
    }

    args_bytes = ATOMIC_LOAD_RELAXED(ctx->header->args_bytes);
    if(args_bytes > ctx->buffer_size) {
      ipc_rpc_executor_publish(ctx->header, request_id, E_OVERFLOW, E_OK);
      err = E_OVERFLOW;
      break;
    }

    *p_func_id = ATOMIC_LOAD_RELAXED(ctx->header->func_id);
    *p_args_bytes = args_bytes;
    *args = args_bytes != 0u ? ctx->buffer : NULL;
    ctx->request_id = request_id;
    ctx->pending = true;

  } while(0);

  return err;
}

error_t ipc_rpc_executor_return(ipc_rpc_executor_ctx_t *ctx, error_t ret)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ATOMIC_ISLOCKED(ctx->lock) == false, err = E_INVALACT);
    BREAK_IF_ACTION(ctx->pending == false, err = E_INVALACT);

    ipc_rpc_executor_publish(ctx->header, ctx->request_id, E_OK, ret);
    ctx->pending = false;

  } while(0);

  return err;
}

static void ipc_rpc_executor_publish(ipc_rpc_header_ctx_t *header, uint32_t request_id,
    error_t status, error_t ret)
{
  ATOMIC_STORE_RELAXED(header->status, status);
  ATOMIC_STORE_RELAXED(header->ret, ret);
  ATOMIC_STORE(header->response_id, request_id);
}
