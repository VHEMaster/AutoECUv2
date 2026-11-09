/*
 * ipc_buffer_private.c
 *
 *  Created on: 4 нояб. 2026 г.
 *      Author: VHEMaster
 */

#include "ipc_buffer_private.h"

INLINE void ipc_buffer_copy_begin(ipc_buffer_copy_ctx_t *ctx, void *memory,
    const void *buffer, uint32_t itemsize, uint32_t count, uint32_t position,
    uint32_t amount, bool output, uint32_t memcpy_threshold,
    ipc_memcpy_cb_t memcpy_cb, void *memcpy_usrdata)
{
  uint32_t first = MIN(amount, count - position);
  uint8_t *ring = memory;
  const uint8_t *user = buffer;

  ctx->segment = 0;
  ctx->buffer = buffer;
  ctx->amount = amount;
  ctx->memcpy_cb = memcpy_cb;
  ctx->memcpy_usrdata = memcpy_usrdata;
  ctx->memcpy_threshold = memcpy_threshold;
  ctx->segments = amount > first ? 2u : 1u;
  ctx->bytes[0] = first * itemsize;
  ctx->bytes[1] = (amount - first) * itemsize;

  if(output == true) {
    ctx->destination[0] = (void *)user;
    ctx->destination[1] = (void *)(user + first * itemsize);
    ctx->source[0] = ring + position * itemsize;
    ctx->source[1] = ring;
  } else {
    ctx->destination[0] = ring + position * itemsize;
    ctx->destination[1] = ring;
    ctx->source[0] = user;
    ctx->source[1] = user + first * itemsize;
  }

  ctx->state = IPC_BUFFER_COPY_START;
}

INLINE error_t ipc_buffer_copy_process(ipc_buffer_copy_ctx_t *ctx)
{
  error_t err = E_OK;
  ipc_memcpy_action_t action;
  uint32_t segment;

  while(ctx->state != IPC_BUFFER_COPY_IDLE) {
    segment = ctx->segment;
    action = ctx->state == IPC_BUFFER_COPY_START ?
        IPC_MEMCPY_START : IPC_MEMCPY_POLL;

    if(ctx->bytes[segment] >= ctx->memcpy_threshold) {
      err = ctx->memcpy_cb(ctx->destination[segment], ctx->source[segment],
          ctx->bytes[segment], action, ctx->memcpy_usrdata);
    } else {
      err = E_PARAM;
    }

    if(err == E_AGAIN) {
      ctx->state = IPC_BUFFER_COPY_WAIT;
      break;
    } else if(err != E_OK) {
      memcpy(ctx->destination[segment], ctx->source[segment], ctx->bytes[segment]);
      err = E_OK;
    }

    if(++ctx->segment < ctx->segments) {
      ctx->state = IPC_BUFFER_COPY_START;
    } else {
      ctx->state = IPC_BUFFER_COPY_IDLE;
    }
  }

  return err;
}
