/*
 * ipc_reader.c
 *
 *  Created on: 4 нояб. 2026 г.
 *      Author: VHEMaster
 */

#include "ipc_reader.h"
#include "compiler.h"

error_t ipc_reader_init(ipc_reader_ctx_t *ctx, uint32_t itemsize, uint32_t itemcount, ipc_header_ctx_t *header, const void *buffer)
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
    ctx->synchronized = false;

    (void)ipc_reader_synchronize(ctx);

  } while(0);

  return err;
}

error_t ipc_reader_peek(ipc_reader_ctx_t *ctx, void *buffer)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF((err = ipc_reader_synchronize(ctx)) != E_OK);
    BREAK_IF_ACTION(ctx->read == ctx->write, err = E_AGAIN);

    if(buffer != NULL) {
      memcpy(buffer, ctx->buffer + ctx->read * ctx->itemsize, ctx->itemsize);
    }

  } while(0);

  return err;
}

error_t ipc_reader_pull(ipc_reader_ctx_t *ctx, void *buffer)
{
  error_t err = E_OK;
  uint32_t read;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF((err = ipc_reader_synchronize(ctx)) != E_OK);
    BREAK_IF_ACTION(ctx->read == ctx->write, err = E_AGAIN);
    read = ctx->read;

    if(buffer != NULL) {
      memcpy(buffer, ctx->buffer + read * ctx->itemsize, ctx->itemsize);
    }
    if(++read >= ctx->itemcount) {
      read = 0;
    }
    ATOMIC_STORE(ctx->header->read, read);
    ctx->read = read;

  } while(0);

  return err;
}

error_t ipc_reader_is_full(ipc_reader_ctx_t *ctx, bool *p_is_full)
{
  error_t err = E_OK;
  uint32_t free_to_write;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(p_is_full == NULL, err = E_PARAM);
    BREAK_IF((err = ipc_reader_synchronize(ctx)) != E_OK);

    free_to_write = ipc_get_free_to_write(ctx->itemcount, ctx->read, ctx->write);
    *p_is_full = free_to_write == 0;

  } while(0);

  return err;
}

error_t ipc_reader_is_empty(ipc_reader_ctx_t *ctx, bool *p_is_empty)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(p_is_empty == NULL, err = E_PARAM);
    BREAK_IF((err = ipc_reader_synchronize(ctx)) != E_OK);

    *p_is_empty = ctx->read == ctx->write;

  } while(0);

  return err;
}

error_t ipc_reader_get_used(ipc_reader_ctx_t *ctx, uint32_t *p_used_elem)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(p_used_elem == NULL, err = E_PARAM);
    BREAK_IF((err = ipc_reader_synchronize(ctx)) != E_OK);

    *p_used_elem = ipc_get_used_to_read(ctx->itemcount, ctx->read, ctx->write);

  } while(0);

  return err;
}

error_t ipc_reader_get_free(ipc_reader_ctx_t *ctx, uint32_t *p_free_elem)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(p_free_elem == NULL, err = E_PARAM);
    BREAK_IF((err = ipc_reader_synchronize(ctx)) != E_OK);

    *p_free_elem = ipc_get_free_to_write(ctx->itemcount, ctx->read, ctx->write);

  } while(0);

  return err;
}

error_t ipc_reader_is_sync(ipc_reader_ctx_t *ctx, bool *p_is_sync)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(p_is_sync == NULL, err = E_PARAM);

    (void)ipc_reader_synchronize(ctx);

    *p_is_sync = ctx->synchronized;

  } while(0);

  return err;
}

error_t ipc_reader_synchronize(ipc_reader_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);

    if(ctx->synchronized == true) {
      ctx->unique_val = ctx->header->unique_val;
      ctx->read = ctx->header->read;
      ctx->write = ATOMIC_LOAD(ctx->header->write);
    } else {
      ctx->unique_val = ATOMIC_LOAD(ctx->header->unique_val);
      ctx->read = ATOMIC_LOAD(ctx->header->read);
      ctx->write = ATOMIC_LOAD(ctx->header->write);
    }

    do {
      BREAK_IF_ACTION(ctx->unique_val != IPC_UNIQUE_VAL, err = E_AGAIN);
      BREAK_IF_ACTION(ctx->read >= ctx->itemcount, err = E_FAULT);
      BREAK_IF_ACTION(ctx->write >= ctx->itemcount, err = E_FAULT);

      ctx->synchronized = true;

    } while(0);

    if(err != E_OK) {
      ctx->synchronized = false;
    }
  } while(0);

  return err;
}
