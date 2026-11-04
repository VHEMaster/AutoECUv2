/*
 * ipc_reader.c
 *
 *  Created on: 4 нояб. 2026 г.
 *      Author: VHEMaster
 */

#include "ipc_reader.h"
#include "compiler.h"

error_t ipc_reader_init(ipc_reader_ctx_t *ctx, uint32_t itemsize, uint32_t itemcount, const void *buffer)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);



  } while(0);

  return err;
}

error_t ipc_reader_peek(ipc_reader_ctx_t *ctx, void *buffer)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);



  } while(0);

  return err;
}

error_t ipc_reader_pull(ipc_reader_ctx_t *ctx, void *buffer)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);



  } while(0);

  return err;
}

error_t ipc_reader_is_full(ipc_reader_ctx_t *ctx, bool *p_is_full)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);



  } while(0);

  return err;
}

error_t ipc_reader_is_empty(ipc_reader_ctx_t *ctx, bool *p_is_empty)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);



  } while(0);

  return err;
}
