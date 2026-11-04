/*
 * ipc_buffer_reader.c
 *
 *  Created on: 4 нояб. 2026 г.
 *      Author: VHEMaster
 */

#include "ipc_buffer_reader.h"
#include "ipc_fifo_reader.h"

#include "ipc_buffer_private.h"
#include "ipc_private.h"
#include "compiler.h"

error_t ipc_buffer_reader_init(ipc_buffer_reader_ctx_t *ctx, uint32_t itemsize,
    uint32_t itemscount, uint32_t descriptorscount, ipc_buffer_header_ctx_t *header,
    const ipc_buffer_descriptor_t *descriptors, const void *memorybuffer)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(header == NULL, err = E_PARAM);
    BREAK_IF_ACTION(descriptors == NULL, err = E_PARAM);
    BREAK_IF_ACTION(memorybuffer == NULL, err = E_PARAM);

    BREAK_IF_ACTION(itemsize == 0, err = E_PARAM);
    BREAK_IF_ACTION(itemscount < 2, err = E_PARAM);
    BREAK_IF_ACTION(descriptorscount < 2, err = E_PARAM);

    memset(ctx, 0u, sizeof(ipc_buffer_reader_ctx_t));
    ctx->header = header;
    ctx->itemsize = itemsize;
    ctx->itemscount = itemscount;
    ctx->memorybuffer = memorybuffer;
    ctx->descriptors = descriptors;

    err = ipc_fifo_reader_init(&ctx->fifo_ctx, sizeof(ipc_buffer_descriptor_t),
        descriptorscount, &header->fifo_header, descriptors);
    BREAK_IF(err != E_OK);

    err = ipc_fifo_reader_lock(&ctx->fifo_ctx);
    BREAK_IF(err != E_OK);

    ATOMIC_STORE_RELAXED(ctx->lock, true);
    (void)ipc_buffer_reader_synchronize(ctx);
    ATOMIC_RELEASE(ctx->lock);

  } while(0);

  return err;
}

error_t ipc_buffer_reader_lock(ipc_buffer_reader_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(!ATOMIC_TRY_LOCK(ctx->lock), err = E_AGAIN);
  } while(0);

  return err;
}

error_t ipc_buffer_reader_unlock(ipc_buffer_reader_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ATOMIC_ISLOCKED(ctx->lock) == false, err = E_INVALACT);
    BREAK_IF_ACTION(ctx->started, err = E_BUSY);
    ATOMIC_RELEASE(ctx->lock);
  } while(0);

  return err;
}

error_t ipc_buffer_reader_is_sync(ipc_buffer_reader_ctx_t *ctx, bool *p_is_sync)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || p_is_sync == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ATOMIC_ISLOCKED(ctx->lock) == false, err = E_INVALACT);

    *p_is_sync = ipc_buffer_reader_synchronize(ctx) == E_OK;

  } while(0);

  return err;
}

error_t ipc_buffer_reader_begin(ipc_buffer_reader_ctx_t *ctx, uint32_t *p_itemscount)
{
  error_t err = E_OK;
  ipc_buffer_descriptor_t descriptor;
  uint32_t read, write, used;

  do {
    BREAK_IF_ACTION(ctx == NULL || p_itemscount == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ATOMIC_ISLOCKED(ctx->lock) == false, err = E_INVALACT);
    BREAK_IF_ACTION(ctx->started, err = E_BUSY);

    err = ipc_buffer_reader_synchronize(ctx);
    BREAK_IF(err != E_OK);

    err = ipc_fifo_reader_peek(&ctx->fifo_ctx, &descriptor);
    BREAK_IF(err != E_OK);

    read = ATOMIC_LOAD_RELAXED(ctx->header->read);
    write = ATOMIC_LOAD(ctx->header->write);

    BREAK_IF_ACTION(read >= ctx->itemscount || write >= ctx->itemscount, err = E_FAULT);

    used = ipc_get_used_to_read(ctx->itemscount, read, write);

    BREAK_IF_ACTION(descriptor.offset != read || descriptor.size == 0 ||
        descriptor.size >= ctx->itemscount || descriptor.size > used, err = E_FAULT);

    ctx->read = descriptor.offset;
    ctx->write = write;
    ctx->size = descriptor.size;
    ctx->left = descriptor.size;
    ctx->started = true;
    *p_itemscount = descriptor.size;
  } while(0);

  return err;
}

error_t ipc_buffer_reader_read(ipc_buffer_reader_ctx_t *ctx, void *buffer, uint32_t *p_itemscount)
{
  error_t err = E_OK;
  uint32_t amount;

  do {
    BREAK_IF_ACTION(ctx == NULL || p_itemscount == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ATOMIC_ISLOCKED(ctx->lock) == false, err = E_INVALACT);
    BREAK_IF_ACTION(ctx->started == false, err = E_INVALACT);

    amount = *p_itemscount;

    BREAK_IF_ACTION(buffer == NULL && amount != 0, err = E_PARAM);

    *p_itemscount = 0;

    BREAK_IF(amount == 0);
    BREAK_IF_ACTION(ctx->left == 0, err = E_AGAIN);
    amount = MIN(amount, ctx->left);

    ipc_buffer_copy_out(buffer, ctx->memorybuffer, ctx->itemsize,
        ctx->itemscount, ctx->read, amount);

    ctx->read = ipc_buffer_advance(ctx->itemscount, ctx->read, amount);
    ctx->left -= amount;

    *p_itemscount = amount;

  } while(0);

  return err;
}

error_t ipc_buffer_reader_cancel(ipc_buffer_reader_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ATOMIC_ISLOCKED(ctx->lock) == false, err = E_INVALACT);
    BREAK_IF_ACTION(ctx->started == false, err = E_INVALACT);

    ctx->read = ATOMIC_LOAD_RELAXED(ctx->header->read);

    ctx->started = false;
    ctx->size = 0;
    ctx->left = 0;

  } while(0);

  return err;
}

error_t ipc_buffer_reader_release(ipc_buffer_reader_ctx_t *ctx)
{
  error_t err = E_OK;
  uint32_t next;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ATOMIC_ISLOCKED(ctx->lock) == false, err = E_INVALACT);
    BREAK_IF_ACTION(ctx->started == false, err = E_INVALACT);

    next = ipc_buffer_advance(ctx->itemscount, ctx->read, ctx->left);

    err = ipc_fifo_reader_pull(&ctx->fifo_ctx, NULL);
    BREAK_IF(err != E_OK);

    ATOMIC_STORE(ctx->header->read, next);

    ctx->read = next;
    ctx->started = false;
    ctx->size = 0;
    ctx->left = 0;

  } while(0);

  return err;
}

static error_t ipc_buffer_reader_copy(ipc_buffer_reader_ctx_t *ctx, void *buffer, uint32_t *p_itemscount, bool consume)
{
  error_t err = E_OK;
  uint32_t capacity, size;

  do {
    BREAK_IF_ACTION(ctx == NULL || p_itemscount == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ATOMIC_ISLOCKED(ctx->lock) == false, err = E_INVALACT);

    BREAK_IF_ACTION(buffer == NULL, err = E_PARAM);
    capacity = *p_itemscount;

    err = ipc_buffer_reader_begin(ctx, &size);
    BREAK_IF(err != E_OK);
    if(capacity < size) {
      *p_itemscount = size;
      (void)ipc_buffer_reader_cancel(ctx);
      err = E_OVERFLOW;
      break;
    }
    *p_itemscount = size;
    err = ipc_buffer_reader_read(ctx, buffer, p_itemscount);
    if(err == E_OK && consume) {
      err = ipc_buffer_reader_release(ctx);
    } else {
      (void)ipc_buffer_reader_cancel(ctx);
    }

  } while(0);

  return err;
}

error_t ipc_buffer_reader_peek(ipc_buffer_reader_ctx_t *ctx, void *buffer, uint32_t *p_itemscount)
{
  return ipc_buffer_reader_copy(ctx, buffer, p_itemscount, false);
}

error_t ipc_buffer_reader_pull(ipc_buffer_reader_ctx_t *ctx, void *buffer, uint32_t *p_itemscount)
{
  return ipc_buffer_reader_copy(ctx, buffer, p_itemscount, true);
}

error_t ipc_buffer_reader_get_memory_used(ipc_buffer_reader_ctx_t *ctx, uint32_t *p_used_items)
{
  error_t err = E_OK;
  uint32_t used;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(p_used_items == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ATOMIC_ISLOCKED(ctx->lock) == false, err = E_INVALACT);
    BREAK_IF_ACTION(ctx->started == true, err = E_INVALACT);
    BREAK_IF((err = ipc_buffer_reader_synchronize(ctx)) != E_OK);

    used = ipc_get_used_to_read(ctx->itemscount, ctx->read, ctx->write);

    *p_used_items = used;

  } while(0);

  return err;
}

error_t ipc_buffer_reader_get_memory_free(ipc_buffer_reader_ctx_t *ctx, uint32_t *p_free_items)
{
  error_t err = E_OK;
  uint32_t used;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(p_free_items == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ATOMIC_ISLOCKED(ctx->lock) == false, err = E_INVALACT);
    BREAK_IF_ACTION(ctx->started == true, err = E_INVALACT);
    BREAK_IF((err = ipc_buffer_reader_synchronize(ctx)) != E_OK);

    used = ipc_get_used_to_read(ctx->itemscount, ctx->read, ctx->write);

    *p_free_items = ctx->itemscount - used - 1u;

  } while(0);

  return err;
}

error_t ipc_buffer_reader_is_memory_full(ipc_buffer_reader_ctx_t *ctx, bool *p_is_full)
{
  error_t err = E_OK;
  uint32_t count;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(p_is_full == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ATOMIC_ISLOCKED(ctx->lock) == false, err = E_INVALACT);
    BREAK_IF_ACTION(ctx->started == true, err = E_INVALACT);
    BREAK_IF((err = ipc_buffer_reader_get_memory_free(ctx, &count)) != E_OK);

    *p_is_full = count == 0;

  } while(0);

  return err;
}

error_t ipc_buffer_reader_is_memory_empty(ipc_buffer_reader_ctx_t *ctx, bool *p_is_empty)
{
  error_t err = E_OK;
  uint32_t count;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(p_is_empty == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ATOMIC_ISLOCKED(ctx->lock) == false, err = E_INVALACT);
    BREAK_IF_ACTION(ctx->started == true, err = E_INVALACT);
    BREAK_IF((err = ipc_buffer_reader_get_memory_used(ctx, &count)) != E_OK);

    *p_is_empty = count == 0;

  } while(0);

  return err;
}

error_t ipc_buffer_reader_get_buffers_used(ipc_buffer_reader_ctx_t *ctx, uint32_t *p_used_buffers)
{
  error_t err = E_OK;
  uint32_t used;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(p_used_buffers == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ATOMIC_ISLOCKED(ctx->lock) == false, err = E_INVALACT);
    BREAK_IF_ACTION(ctx->started == true, err = E_INVALACT);

    BREAK_IF((err = ipc_buffer_reader_synchronize(ctx)) != E_OK);
    BREAK_IF((err = ipc_fifo_reader_get_used(&ctx->fifo_ctx, &used)) != E_OK);

    *p_used_buffers = used;

  } while(0);

  return err;
}

error_t ipc_buffer_reader_get_buffers_free(ipc_buffer_reader_ctx_t *ctx, uint32_t *p_free_buffers)
{
  error_t err = E_OK;
  uint32_t used;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(p_free_buffers == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ATOMIC_ISLOCKED(ctx->lock) == false, err = E_INVALACT);

    BREAK_IF((err = ipc_buffer_reader_synchronize(ctx)) != E_OK);
    BREAK_IF((err = ipc_fifo_reader_get_used(&ctx->fifo_ctx, &used)) != E_OK);

    *p_free_buffers = ctx->fifo_ctx.itemscount - used - 1u;

  } while(0);

  return err;
}

error_t ipc_buffer_reader_is_buffers_full(ipc_buffer_reader_ctx_t *ctx, bool *p_is_full)
{
  error_t err = E_OK;
  uint32_t count;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(p_is_full == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ATOMIC_ISLOCKED(ctx->lock) == false, err = E_INVALACT);
    BREAK_IF((err = ipc_buffer_reader_get_buffers_free(ctx, &count)) != E_OK);

    *p_is_full = count == 0;

  } while(0);

  return err;
}

error_t ipc_buffer_reader_is_buffers_empty(ipc_buffer_reader_ctx_t *ctx, bool *p_is_empty)
{
  error_t err = E_OK;
  uint32_t count;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(p_is_empty == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ATOMIC_ISLOCKED(ctx->lock) == false, err = E_INVALACT);
    BREAK_IF((err = ipc_buffer_reader_get_buffers_used(ctx, &count)) != E_OK);

    *p_is_empty = count == 0;

  } while(0);

  return err;
}

error_t ipc_buffer_reader_is_full(ipc_buffer_reader_ctx_t *ctx, bool *p_is_full)
{
  error_t err = E_OK;
  bool buffers_full;
  bool memory_full;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(p_is_full == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ATOMIC_ISLOCKED(ctx->lock) == false, err = E_INVALACT);

    err = ipc_buffer_reader_is_buffers_full(ctx, &buffers_full);
    BREAK_IF(err != E_OK);

    err = ipc_buffer_reader_is_memory_full(ctx, &memory_full);
    BREAK_IF(err != E_OK);

    *p_is_full = buffers_full || memory_full;

  } while(0);

  return err;
}

error_t ipc_buffer_reader_is_empty(ipc_buffer_reader_ctx_t *ctx, bool *p_is_empty)
{
  error_t err = E_OK;
  bool buffers_empty;
  bool memory_empty;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(p_is_empty == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ATOMIC_ISLOCKED(ctx->lock) == false, err = E_INVALACT);

    err = ipc_buffer_reader_is_buffers_empty(ctx, &buffers_empty);
    BREAK_IF(err != E_OK);

    err = ipc_buffer_reader_is_memory_empty(ctx, &memory_empty);
    BREAK_IF(err != E_OK);

    *p_is_empty = buffers_empty && memory_empty;

  } while(0);

  return err;
}

error_t ipc_buffer_reader_synchronize(ipc_buffer_reader_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ATOMIC_ISLOCKED(ctx->lock) == false, err = E_INVALACT);

    err = ipc_fifo_reader_synchronize(&ctx->fifo_ctx);
    BREAK_IF(err != E_OK);

    if(ctx->synchronized == true) {
      ctx->unique_val = ATOMIC_LOAD_RELAXED(ctx->header->unique_val);
      if(ctx->started == false) {
        ctx->read = ATOMIC_LOAD_RELAXED(ctx->header->read);
      }
      ctx->write = ATOMIC_LOAD(ctx->header->write);
    } else {
      ctx->unique_val = ATOMIC_LOAD(ctx->header->unique_val);
      if(ctx->started == false) {
        ctx->read = ATOMIC_LOAD(ctx->header->read);
      }
      ctx->write = ATOMIC_LOAD(ctx->header->write);
    }

    do {
      BREAK_IF_ACTION(ctx->unique_val != IPC_BUFFER_UNIQUE_VAL, err = E_AGAIN);
      BREAK_IF_ACTION(ctx->read >= ctx->itemscount, err = E_FAULT);
      BREAK_IF_ACTION(ctx->write >= ctx->itemscount, err = E_FAULT);

      ctx->synchronized = true;

    } while(0);

    if(err != E_OK) {
      ctx->synchronized = false;
    }
  } while(0);

  return err;
}
