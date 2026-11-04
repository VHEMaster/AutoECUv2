/*
 * ipc_buffer_private.c
 *
 *  Created on: 4 нояб. 2026 г.
 *      Author: VHEMaster
 */

#include "ipc_buffer_private.h"

INLINE uint32_t ipc_buffer_advance(uint32_t count, uint32_t position, uint32_t amount)
{
  uint32_t tail = count - position;

  return amount >= tail ? amount - tail : position + amount;
}

INLINE void ipc_buffer_copy_out(void *destination, const void *memory, uint32_t itemsize, uint32_t count, uint32_t position, uint32_t amount)
{
  uint32_t first = MIN(amount, count - position);
  uint8_t *output = destination;
  const uint8_t *input = memory;

  memcpy(output, input + position * itemsize, first * itemsize);

  if(amount > first) {
    memcpy(output + first * itemsize, input, (amount - first) * itemsize);
  }
}

INLINE void ipc_buffer_copy_in(void *memory, const void *source, uint32_t itemsize, uint32_t count, uint32_t position, uint32_t amount)
{
  uint32_t first = MIN(amount, count - position);
  uint8_t *output = memory;
  const uint8_t *input = source;

  memcpy(output + position * itemsize, input, first * itemsize);

  if(amount > first) {
    memcpy(output, input + first * itemsize, (amount - first) * itemsize);
  }
}

INLINE void ipc_buffer_copy_begin(ipc_buffer_copy_ctx_t *ctx, void *memory,
    const void *buffer, uint32_t itemsize, uint32_t count, uint32_t position,
    uint32_t amount, bool output, ipc_buffer_memcpy_cb_t memcpy_cb, void *memcpy_usrdata)
{
  uint32_t first = MIN(amount, count - position);
  uint8_t *ring = memory;
  const uint8_t *user = buffer;

  ctx->segment = 0;
  ctx->buffer = buffer;
  ctx->amount = amount;
  ctx->memcpy_cb = memcpy_cb;
  ctx->memcpy_usrdata = memcpy_usrdata;
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
  ipc_buffer_memcpy_action_t action;
  uint32_t segment;

  while(ctx->state != IPC_BUFFER_COPY_IDLE) {
    segment = ctx->segment;
    action = ctx->state == IPC_BUFFER_COPY_START ?
        IPC_BUFFER_MEMCPY_START : IPC_BUFFER_MEMCPY_POLL;

    err = ctx->memcpy_cb(ctx->destination[segment], ctx->source[segment],
        ctx->bytes[segment], action, ctx->memcpy_usrdata);

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
