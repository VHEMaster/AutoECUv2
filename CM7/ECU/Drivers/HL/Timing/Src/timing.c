/*
 * timing.c
 *
 *  Created on: Jun 6, 2024
 *      Author: VHEMaster
 */

#include "timing.h"
#include "timing_internal.h"
#include "compiler.h"
#include <string.h>

error_t timing_init(timing_ctx_t *ctx, const timing_init_ctx_t *init_ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || init_ctx == NULL, err = E_PARAM);

    memset(ctx, 0u, sizeof(timing_ctx_t));
    memcpy(&ctx->init, init_ctx, sizeof(timing_init_ctx_t));


    ctx->ready = true;

  } while(0);

  return err;
}

error_t timing_configure(timing_ctx_t *ctx, const timing_config_t *config)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || config == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    if(ctx->reset_request == false) {
      ctx->configured = false;

      if(&ctx->config != config) {
        memcpy(&ctx->config, config, sizeof(timing_config_t));
      }



    } else if(ctx->reset_errcode != E_AGAIN) {
      err = ctx->reset_errcode;
      ctx->reset_request = false;
      ctx->configured = true;
    } else {
      err = E_AGAIN;
    }

  } while(0);

  return err;
}

error_t timing_reset(timing_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    ctx->configured = false;

  } while(0);

  return err;
}

void timing_loop_main(timing_ctx_t *ctx)
{

}

void timing_loop_slow(timing_ctx_t *ctx)
{

}

ITCM_FUNC void timing_loop_fast(timing_ctx_t *ctx)
{

}

error_t timing_get_data(timing_ctx_t *ctx, timing_data_t *data)
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

error_t timing_get_diag(timing_ctx_t *ctx, timing_diag_t *diag)
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
