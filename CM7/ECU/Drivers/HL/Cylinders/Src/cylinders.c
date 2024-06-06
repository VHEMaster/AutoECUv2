/*
 * cylinders.c
 *
 *  Created on: Jun 6, 2024
 *      Author: VHEMaster
 */

#include "cylinders.h"
#include "cylinders_internal.h"
#include "compiler.h"
#include <string.h>

error_t cylinders_init(cylinders_ctx_t *ctx, const cylinders_init_ctx_t *init_ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || init_ctx == NULL, err = E_PARAM);

    memset(ctx, 0u, sizeof(cylinders_ctx_t));
    memcpy(&ctx->init, init_ctx, sizeof(cylinders_init_ctx_t));


    ctx->ready = true;

  } while(0);

  return err;
}

error_t cylinders_configure(cylinders_ctx_t *ctx, const cylinders_config_t *config)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || config == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    ctx->configured = false;

    if(&ctx->config != config) {
      memcpy(&ctx->config, config, sizeof(cylinders_config_t));
    }

    ctx->configured = true;

  } while(0);

  return err;
}

error_t cylinders_reset(cylinders_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    ctx->configured = false;

  } while(0);

  return err;
}

void cylinders_loop_main(cylinders_ctx_t *ctx)
{

}

void cylinders_loop_slow(cylinders_ctx_t *ctx)
{

}

ITCM_FUNC void cylinders_loop_fast(cylinders_ctx_t *ctx)
{

}

error_t cylinders_get_data(cylinders_ctx_t *ctx, cylinders_data_t *data)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(data == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    *data = ctx->data;

  } while(0);

  return err;
}

error_t cylinders_get_diag(cylinders_ctx_t *ctx, cylinders_diag_t *diag)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(diag == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    *diag = ctx->diag;

  } while(0);

  return err;
}
