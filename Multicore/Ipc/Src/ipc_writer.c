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

    ATOMIC_STORE(ctx->header->read, ctx->read);
    ATOMIC_STORE(ctx->header->write, ctx->write);
    ATOMIC_STORE(ctx->header->unique_val, IPC_UNIQUE_VAL);

  } while(0);

  return err;
}

error_t ipc_writer_push(ipc_writer_ctx_t *ctx, const void *buffer)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);



  } while(0);

  return err;
}

error_t ipc_writer_get_used(ipc_writer_ctx_t *ctx, uint32_t *p_used_elem)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(p_used_elem == NULL, err = E_PARAM);

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

    *p_free_elem = ipc_get_free_to_write(ctx->itemcount, ctx->read, ctx->write);

  } while(0);

  return err;
}

error_t ipc_writer_is_full(ipc_writer_ctx_t *ctx, bool *p_is_full)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);



  } while(0);

  return err;
}

error_t ipc_writer_is_empty(ipc_writer_ctx_t *ctx, bool *p_is_empty)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);



  } while(0);

  return err;
}
