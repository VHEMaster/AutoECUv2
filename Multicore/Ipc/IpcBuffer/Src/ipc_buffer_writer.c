/*
 * ipc_buffer_writer.c
 *
 *  Created on: 4 нояб. 2026 г.
 *      Author: VHEMaster
 */

#include "ipc_buffer_writer.h"
#include "ipc_fifo_writer.h"

#include "ipc_buffer_private.h"
#include "ipc_private.h"
#include "compiler.h"

static error_t ipc_buffer_writer_synchronize(ipc_buffer_writer_ctx_t *ctx);
static error_t ipc_buffer_writer_copy(ipc_buffer_writer_ctx_t *ctx, const void *buffer, uint32_t itemscount);

error_t ipc_buffer_writer_init(ipc_buffer_writer_ctx_t *ctx, uint32_t itemsize,
    uint32_t itemscount, uint32_t descriptorscount, ipc_buffer_header_ctx_t *header,
    ipc_buffer_descriptor_t *descriptors, void *memorybuffer)
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
    BREAK_IF_ACTION(itemsize > UINT32_MAX / itemscount, err = E_PARAM);
    BREAK_IF_ACTION(descriptorscount > UINT32_MAX / sizeof(ipc_buffer_descriptor_t), err = E_PARAM);

    memset(ctx, 0u, sizeof(ipc_buffer_writer_ctx_t));
    ctx->header = header;
    ctx->itemsize = itemsize;
    ctx->itemscount = itemscount;
    ctx->memorybuffer = memorybuffer;
    ctx->descriptors = descriptors;

    ATOMIC_STORE(ctx->header->unique_val, 0u);
    ATOMIC_STORE_RELAXED(ctx->header->read, 0u);
    ATOMIC_STORE_RELAXED(ctx->header->write, 0u);

    err = ipc_fifo_writer_init(&ctx->fifo_ctx, sizeof(ipc_buffer_descriptor_t),
        descriptorscount, &header->fifo_header, descriptors);
    BREAK_IF(err != E_OK);

    err = ipc_fifo_writer_lock(&ctx->fifo_ctx);
    BREAK_IF(err != E_OK);

    ATOMIC_STORE(ctx->header->unique_val, IPC_BUFFER_UNIQUE_VAL);

  } while(0);

  return err;
}

error_t ipc_buffer_writer_set_memcpy(ipc_buffer_writer_ctx_t *ctx,
    ipc_buffer_memcpy_cb_t memcpy_cb, void *memcpy_usrdata)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->started != false, err = E_INVALACT);

    ctx->memcpy_cb = memcpy_cb;
    ctx->memcpy_usrdata = memcpy_usrdata;

  } while(0);

  return err;
}

error_t ipc_buffer_writer_lock(ipc_buffer_writer_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(!ATOMIC_TRY_LOCK(ctx->lock), err = E_AGAIN);
  } while(0);

  return err;
}

error_t ipc_buffer_writer_unlock(ipc_buffer_writer_ctx_t *ctx)
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

error_t ipc_buffer_writer_begin(ipc_buffer_writer_ctx_t *ctx, uint32_t itemscount)
{
  error_t err = E_OK;
  uint32_t free_items, free_descriptors;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ATOMIC_ISLOCKED(ctx->lock) == false, err = E_INVALACT);
    BREAK_IF_ACTION(ctx->started, err = E_BUSY);
    BREAK_IF_ACTION(itemscount == 0, err = E_PARAM);

    err = ipc_buffer_writer_synchronize(ctx);
    BREAK_IF(err != E_OK);

    free_items = ipc_get_free_to_write(ctx->itemscount, ctx->read, ctx->write);
    BREAK_IF_ACTION(itemscount > free_items, err = E_OVERFLOW);

    err = ipc_fifo_writer_get_free(&ctx->fifo_ctx, &free_descriptors);
    BREAK_IF(err != E_OK);

    BREAK_IF_ACTION(free_descriptors == 0, err = E_OVERFLOW);

    ctx->size = itemscount;
    ctx->left = itemscount;
    ctx->operation = IPC_BUFFER_OPERATION_MANUAL;
    ctx->started = true;

  } while(0);

  return err;
}

error_t ipc_buffer_writer_write(ipc_buffer_writer_ctx_t *ctx, const void *buffer, uint32_t itemscount)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ATOMIC_ISLOCKED(ctx->lock) == false, err = E_INVALACT);
    BREAK_IF_ACTION(ctx->started == false, err = E_INVALACT);
    BREAK_IF_ACTION(ctx->operation != IPC_BUFFER_OPERATION_MANUAL, err = E_BUSY);

    err = ipc_buffer_writer_copy(ctx, buffer, itemscount);

  } while(0);

  return err;
}

error_t ipc_buffer_writer_commit(ipc_buffer_writer_ctx_t *ctx)
{
  error_t err = E_OK;
  ipc_buffer_descriptor_t descriptor;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ATOMIC_ISLOCKED(ctx->lock) == false, err = E_INVALACT);
    BREAK_IF_ACTION(ctx->started == false, err = E_INVALACT);
    BREAK_IF_ACTION(ctx->copy_ctx.state != IPC_BUFFER_COPY_IDLE, err = E_BUSY);
    BREAK_IF_ACTION(ctx->left > 0, err = E_INVALACT);

    err = ipc_buffer_writer_synchronize(ctx);
    BREAK_IF(err != E_OK);

    descriptor.offset = ATOMIC_LOAD_RELAXED(ctx->header->write);
    descriptor.size = ctx->size;

    ATOMIC_STORE_RELAXED(ctx->header->write, ctx->write);

    err = ipc_fifo_writer_push(&ctx->fifo_ctx, &descriptor);
    if(err != E_OK) {
      ATOMIC_STORE_RELAXED(ctx->header->write, descriptor.offset);
      break;
    }

    ctx->started = false;
    ctx->size = 0;
    ctx->left = 0;

  } while(0);

  return err;
}

error_t ipc_buffer_writer_cancel(ipc_buffer_writer_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ATOMIC_ISLOCKED(ctx->lock) == false, err = E_INVALACT);
    BREAK_IF_ACTION(ctx->started == false, err = E_INVALACT);
    BREAK_IF_ACTION(ctx->copy_ctx.state != IPC_BUFFER_COPY_IDLE, err = E_BUSY);

    ctx->write = ATOMIC_LOAD_RELAXED(ctx->header->write);

    ctx->started = false;
    ctx->size = 0;
    ctx->left = 0;

  } while(0);

  return err;
}

error_t ipc_buffer_writer_push(ipc_buffer_writer_ctx_t *ctx, const void *buffer, uint32_t itemscount)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ATOMIC_ISLOCKED(ctx->lock) == false, err = E_INVALACT);
    BREAK_IF_ACTION(buffer == NULL, err = E_PARAM);

    if(ctx->started == false) {
      err = ipc_buffer_writer_begin(ctx, itemscount);
      BREAK_IF(err != E_OK);

      ctx->operation = IPC_BUFFER_OPERATION_PUSH;
    } else {
      BREAK_IF_ACTION(ctx->operation != IPC_BUFFER_OPERATION_PUSH, err = E_BUSY);
      BREAK_IF_ACTION(buffer != ctx->copy_ctx.buffer ||
          itemscount != ctx->copy_ctx.amount, err = E_PARAM);
    }

    err = ipc_buffer_writer_copy(ctx, buffer, itemscount);
    BREAK_IF(err == E_AGAIN);

    if(err == E_OK) {
      err = ipc_buffer_writer_commit(ctx);
    }
    if(err != E_OK && ctx->started) {
      (void)ipc_buffer_writer_cancel(ctx);
    }

  } while(0);

  return err;
}

error_t ipc_buffer_writer_get_memory_used(ipc_buffer_writer_ctx_t *ctx, uint32_t *p_used_items)
{
  error_t err = E_OK;
  uint32_t used;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(p_used_items == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ATOMIC_ISLOCKED(ctx->lock) == false, err = E_INVALACT);
    BREAK_IF_ACTION(ctx->started == true, err = E_INVALACT);

    err = ipc_buffer_writer_synchronize(ctx);
    BREAK_IF(err != E_OK);

    used = ipc_get_used_to_read(ctx->itemscount, ctx->read, ctx->write);
    BREAK_IF_ACTION(used >= ctx->itemscount, err = E_FAULT);

    *p_used_items = used;

  } while(0);

  return err;
}

error_t ipc_buffer_writer_get_memory_free(ipc_buffer_writer_ctx_t *ctx, uint32_t *p_free_items)
{
  error_t err = E_OK;
  uint32_t used;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(p_free_items == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ATOMIC_ISLOCKED(ctx->lock) == false, err = E_INVALACT);
    BREAK_IF_ACTION(ctx->started == true, err = E_INVALACT);

    err = ipc_buffer_writer_synchronize(ctx);
    BREAK_IF(err != E_OK);

    used = ipc_get_used_to_read(ctx->itemscount, ctx->read, ctx->write);
    BREAK_IF_ACTION(used >= ctx->itemscount, err = E_FAULT);

    *p_free_items = ctx->itemscount - used - 1u;

  } while(0);

  return err;
}

error_t ipc_buffer_writer_is_memory_full(ipc_buffer_writer_ctx_t *ctx, bool *p_is_full)
{
  error_t err = E_OK;
  uint32_t count;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(p_is_full == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ATOMIC_ISLOCKED(ctx->lock) == false, err = E_INVALACT);
    BREAK_IF_ACTION(ctx->started == true, err = E_INVALACT);

    err = ipc_buffer_writer_get_memory_free(ctx, &count);
    BREAK_IF(err != E_OK);

    *p_is_full = count == 0;

  } while(0);

  return err;
}

error_t ipc_buffer_writer_is_memory_empty(ipc_buffer_writer_ctx_t *ctx, bool *p_is_empty)
{
  error_t err = E_OK;
  uint32_t count;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(p_is_empty == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ATOMIC_ISLOCKED(ctx->lock) == false, err = E_INVALACT);
    BREAK_IF_ACTION(ctx->started == true, err = E_INVALACT);

    err = ipc_buffer_writer_get_memory_used(ctx, &count);
    BREAK_IF(err != E_OK);

    *p_is_empty = count == 0;

  } while(0);

  return err;
}

error_t ipc_buffer_writer_get_buffers_used(ipc_buffer_writer_ctx_t *ctx, uint32_t *p_used_buffers)
{
  error_t err = E_OK;
  uint32_t used;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(p_used_buffers == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ATOMIC_ISLOCKED(ctx->lock) == false, err = E_INVALACT);
    BREAK_IF_ACTION(ctx->started == true, err = E_INVALACT);

    err = ipc_buffer_writer_synchronize(ctx);
    BREAK_IF(err != E_OK);

    err = ipc_fifo_writer_get_used(&ctx->fifo_ctx, &used);
    BREAK_IF(err != E_OK);

    BREAK_IF_ACTION(used >= ctx->fifo_ctx.itemscount, err = E_FAULT);

    *p_used_buffers = used;

  } while(0);

  return err;
}

error_t ipc_buffer_writer_get_buffers_free(ipc_buffer_writer_ctx_t *ctx, uint32_t *p_free_buffers)
{
  error_t err = E_OK;
  uint32_t used;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(p_free_buffers == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ATOMIC_ISLOCKED(ctx->lock) == false, err = E_INVALACT);
    BREAK_IF_ACTION(ctx->started == true, err = E_INVALACT);

    err = ipc_buffer_writer_synchronize(ctx);
    BREAK_IF(err != E_OK);

    err = ipc_fifo_writer_get_used(&ctx->fifo_ctx, &used);
    BREAK_IF(err != E_OK);

    BREAK_IF_ACTION(used >= ctx->fifo_ctx.itemscount, err = E_FAULT);

    *p_free_buffers = ctx->fifo_ctx.itemscount - used - 1u;

  } while(0);

  return err;
}

error_t ipc_buffer_writer_is_buffers_full(ipc_buffer_writer_ctx_t *ctx, bool *p_is_full)
{
  error_t err = E_OK;
  uint32_t count;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(p_is_full == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ATOMIC_ISLOCKED(ctx->lock) == false, err = E_INVALACT);
    BREAK_IF_ACTION(ctx->started == true, err = E_INVALACT);

    err = ipc_buffer_writer_get_buffers_free(ctx, &count);
    BREAK_IF(err != E_OK);

    *p_is_full = count == 0;

  } while(0);

  return err;
}

error_t ipc_buffer_writer_is_buffers_empty(ipc_buffer_writer_ctx_t *ctx, bool *p_is_empty)
{
  error_t err = E_OK;
  uint32_t count;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(p_is_empty == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ATOMIC_ISLOCKED(ctx->lock) == false, err = E_INVALACT);
    BREAK_IF_ACTION(ctx->started == true, err = E_INVALACT);

    err = ipc_buffer_writer_get_buffers_used(ctx, &count);
    BREAK_IF(err != E_OK);

    *p_is_empty = count == 0;

  } while(0);

  return err;
}

error_t ipc_buffer_writer_is_full(ipc_buffer_writer_ctx_t *ctx, bool *p_is_full)
{
  error_t err = E_OK;
  bool buffers_full;
  bool memory_full;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(p_is_full == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ATOMIC_ISLOCKED(ctx->lock) == false, err = E_INVALACT);
    BREAK_IF_ACTION(ctx->started == true, err = E_INVALACT);

    err = ipc_buffer_writer_is_buffers_full(ctx, &buffers_full);
    BREAK_IF(err != E_OK);

    err = ipc_buffer_writer_is_memory_full(ctx, &memory_full);
    BREAK_IF(err != E_OK);

    *p_is_full = buffers_full || memory_full;

  } while(0);

  return err;
}

error_t ipc_buffer_writer_is_empty(ipc_buffer_writer_ctx_t *ctx, bool *p_is_empty)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(p_is_empty == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ATOMIC_ISLOCKED(ctx->lock) == false, err = E_INVALACT);
    BREAK_IF_ACTION(ctx->started == true, err = E_INVALACT);

    err = ipc_buffer_writer_synchronize(ctx);
    BREAK_IF(err != E_OK);

    err = ipc_fifo_writer_is_empty(&ctx->fifo_ctx, p_is_empty);

  } while(0);

  return err;
}

static error_t ipc_buffer_writer_copy(ipc_buffer_writer_ctx_t *ctx, const void *buffer, uint32_t itemscount)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(buffer == NULL, err = E_PARAM);

    if(ctx->copy_ctx.state == IPC_BUFFER_COPY_IDLE) {
      BREAK_IF_ACTION(itemscount > ctx->left, err = E_OVERFLOW);

      err = ipc_buffer_writer_synchronize(ctx);
      BREAK_IF(err != E_OK);
      BREAK_IF(itemscount == 0);

      if(ctx->memcpy_cb == NULL) {
        ipc_buffer_copy_in(ctx->memorybuffer, buffer, ctx->itemsize,
            ctx->itemscount, ctx->write, itemscount);
      } else {
        ipc_buffer_copy_begin(&ctx->copy_ctx, ctx->memorybuffer, buffer,
            ctx->itemsize, ctx->itemscount, ctx->write, itemscount, false,
            ctx->memcpy_cb, ctx->memcpy_usrdata);

        err = ipc_buffer_copy_process(&ctx->copy_ctx);
        BREAK_IF(err != E_OK);
      }
    } else {
      BREAK_IF_ACTION(buffer != ctx->copy_ctx.buffer ||
          itemscount != ctx->copy_ctx.amount, err = E_PARAM);

      err = ipc_buffer_copy_process(&ctx->copy_ctx);
      BREAK_IF(err != E_OK);
    }

    err = ipc_buffer_writer_synchronize(ctx);
    BREAK_IF(err != E_OK);

    ctx->write = ipc_buffer_advance(ctx->itemscount, ctx->write, itemscount);
    ctx->left -= itemscount;

  } while(0);

  return err;
}

static error_t ipc_buffer_writer_synchronize(ipc_buffer_writer_ctx_t *ctx)
{
  error_t err = E_OK;
  uint32_t read, write, fifo_read, fifo_write, progress, start;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ATOMIC_ISLOCKED(ctx->lock) == false, err = E_INVALACT);
    BREAK_IF_ACTION(ATOMIC_ISLOCKED(ctx->fifo_ctx.lock) == false, err = E_INVALACT);

    do {
      BREAK_IF_ACTION(ctx->header == NULL, err = E_FAULT);
      BREAK_IF_ACTION(ATOMIC_LOAD_RELAXED(ctx->header->unique_val) != IPC_BUFFER_UNIQUE_VAL, err = E_FAULT);
      BREAK_IF_ACTION(ATOMIC_LOAD_RELAXED(ctx->header->fifo_header.unique_val) != IPC_FIFO_UNIQUE_VAL, err = E_FAULT);

      read = ATOMIC_LOAD(ctx->header->read);
      write = ATOMIC_LOAD_RELAXED(ctx->header->write);
      fifo_read = ATOMIC_LOAD_RELAXED(ctx->header->fifo_header.read);
      fifo_write = ATOMIC_LOAD_RELAXED(ctx->header->fifo_header.write);

      BREAK_IF_ACTION(read >= ctx->itemscount || write >= ctx->itemscount, err = E_FAULT);
      BREAK_IF_ACTION(fifo_read >= ctx->fifo_ctx.itemscount || fifo_write >= ctx->fifo_ctx.itemscount, err = E_FAULT);
      BREAK_IF_ACTION(fifo_write != ctx->fifo_ctx.write, err = E_FAULT);

      if(ctx->started) {
        BREAK_IF_ACTION(ctx->size == 0 || ctx->size >= ctx->itemscount ||
            ctx->left > ctx->size || ctx->write >= ctx->itemscount, err = E_FAULT);

        progress = ctx->size - ctx->left;
        start = ctx->write >= progress ? ctx->write - progress :
            ctx->itemscount - (progress - ctx->write);

        BREAK_IF_ACTION(write != start, err = E_FAULT);
      } else {
        ctx->write = write;
      }
      ctx->read = read;

    } while(0);

    if(err != E_OK) {
      ctx->started = false;
      ctx->size = 0;
      ctx->left = 0;
    }

  } while(0);

  return err;
}
