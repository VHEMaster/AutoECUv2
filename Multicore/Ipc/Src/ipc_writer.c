/*
 * ipc_writer.c
 *
 *  Created on: 4 нояб. 2026 г.
 *      Author: VHEMaster
 */

#include "ipc_writer.h"
#include "compiler.h"

error_t ipc_writer_init(ipc_writer_ctx_t *ctx, uint32_t itemsize, uint32_t itemcount, ipc_header_ctx_t *header, void *buffer)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(buffer == NULL, err = E_PARAM);
    BREAK_IF_ACTION(header == NULL, err = E_PARAM);
    BREAK_IF_ACTION(itemsize == 0, err = E_PARAM);
    BREAK_IF_ACTION(itemcount < 2, err = E_PARAM);

    ctx->header = header;
    ctx->itemsize = itemsize;
    ctx->itemcount = itemcount;
    ctx->buffer = buffer;
    ctx->read = 0;
    ctx->write = 0;
    ctx->unique_val = IPC_UNIQUE_VAL;

    ATOMIC_STORE_RELAXED(ctx->header->read, ctx->read);
    ATOMIC_STORE_RELAXED(ctx->header->write, ctx->write);
    ATOMIC_STORE(ctx->header->unique_val, ctx->unique_val);

  } while(0);

  return err;
}

error_t ipc_writer_push(ipc_writer_ctx_t *ctx, const void *buffer)
{
  error_t err = E_OK;
  uint32_t unique_val, read, write, next, cur;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(buffer == NULL, err = E_PARAM);

    unique_val = ATOMIC_LOAD_RELAXED(ctx->header->unique_val);
    write = ATOMIC_LOAD_RELAXED(ctx->header->write);
    read = ATOMIC_LOAD(ctx->header->read);

    do {
      BREAK_IF_ACTION(unique_val != IPC_UNIQUE_VAL, err = E_FAULT);
      BREAK_IF_ACTION(read >= ctx->itemcount, err = E_FAULT);
      BREAK_IF_ACTION(write >= ctx->itemcount, err = E_FAULT);

      BREAK_IF_ACTION(buffer == NULL, err = E_PARAM);
      cur = write;
      next = write;

      if(++next >= ctx->itemcount) {
        next = 0;
      }
      BREAK_IF_ACTION(next == read, err = E_OVERFLOW);

      if(buffer != NULL) {
        memcpy(ctx->buffer + cur * ctx->itemsize, buffer, ctx->itemsize);
      }
      write = next;
      ATOMIC_STORE(ctx->header->write, write);
      ATOMIC_STORE_RELAXED(ctx->header->unique_val, IPC_UNIQUE_VAL);
    } while(0);

    ctx->read = read;
    ctx->write = write;
    ctx->unique_val = unique_val;

  } while(0);

  return err;
}

error_t ipc_writer_get_used(ipc_writer_ctx_t *ctx, uint32_t *p_used_elem)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(p_used_elem == NULL, err = E_PARAM);
    ctx->read = ATOMIC_LOAD(ctx->header->read);

    *p_used_elem = ipc_get_used_to_read(ctx->itemcount, ctx->read, ctx->write);

  } while(0);

  return err;
}

error_t ipc_writer_get_free(ipc_writer_ctx_t *ctx, uint32_t *p_free_elem)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(p_free_elem == NULL, err = E_PARAM);
    ctx->read = ATOMIC_LOAD(ctx->header->read);

    *p_free_elem = ipc_get_free_to_write(ctx->itemcount, ctx->read, ctx->write);

  } while(0);

  return err;
}

error_t ipc_writer_is_full(ipc_writer_ctx_t *ctx, bool *p_is_full)
{
  error_t err = E_OK;
  uint32_t free_to_write;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(p_is_full == NULL, err = E_PARAM);
    ctx->read = ATOMIC_LOAD(ctx->header->read);

    free_to_write = ipc_get_free_to_write(ctx->itemcount, ctx->read, ctx->write);
    *p_is_full = free_to_write == 0;

  } while(0);

  return err;
}

error_t ipc_writer_is_empty(ipc_writer_ctx_t *ctx, bool *p_is_empty)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(p_is_empty == NULL, err = E_PARAM);
    ctx->read = ATOMIC_LOAD(ctx->header->read);

    *p_is_empty = ctx->read == ctx->write;

  } while(0);

  return err;
}
