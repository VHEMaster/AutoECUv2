/*
 * ipc_sharedmemory.c
 *
 *  Created on: 8 нояб. 2026 г.
 *      Author: VHEMaster
 */

#include "ipc_sharedmemory.h"
#include "ipc_private.h"
#include "compiler.h"

error_t ipc_sharedmemory_init(ipc_sharedmemory_ctx_t *ctx, uint32_t bytes, void *buffer)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(buffer == NULL, err = E_PARAM);

    memset(ctx, 0u, sizeof(ipc_sharedmemory_ctx_t));
    ctx->size = bytes;
    ctx->buffer = buffer;

    ATOMIC_RELEASE(ctx->lock);

  } while(0);

  return err;
}

error_t ipc_sharedmemory_lock(ipc_sharedmemory_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ATOMIC_TRY_LOCK(ctx->lock) == false, err = E_AGAIN);

  } while(0);

  return err;
}

error_t ipc_sharedmemory_unlock(ipc_sharedmemory_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ATOMIC_ISLOCKED(ctx->lock) == false, err = E_INVALACT);

    ATOMIC_RELEASE(ctx->lock);

  } while(0);

  return err;
}

error_t ipc_sharedmemory_get_buffer(ipc_sharedmemory_ctx_t *ctx, void **buffer, uint32_t *bytes)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(buffer == NULL, err = E_PARAM);
    BREAK_IF_ACTION(bytes == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ATOMIC_ISLOCKED(ctx->lock) == false, err = E_INVALACT);

    *buffer = ctx->buffer;
    *bytes = ctx->size;

  } while(0);

  return err;
}
