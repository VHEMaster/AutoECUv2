/*
 * ipc_writer.c
 *
 *  Created on: 4 нояб. 2026 г.
 *      Author: VHEMaster
 */

#include "ipc_writer.h"
#include "compiler.h"

error_t ipc_writer_init(ipc_writer_ctx_t *ctx, uint32_t itemsize, uint32_t itemcount, void *buffer)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);



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
