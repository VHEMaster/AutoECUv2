/*
 * ckp.c
 *
 *  Created on: May 14, 2024
 *      Author: VHEMaster
 */

#include "ckp.h"
#include "compiler.h"
#include <string.h>

error_t ckp_init(ckp_ctx_t *ctx, const ckp_init_ctx_t *init_ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || init_ctx == NULL, err = E_PARAM);

    memset(ctx, 0u, sizeof(ckp_ctx_t));
    memcpy(&ctx->init, init_ctx, sizeof(ckp_init_ctx_t));

    ctx->ready = true;

  } while(0);

  return err;
}

error_t ckp_configure(ckp_ctx_t *ctx, const ckp_config_t *config)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || config == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    if(&ctx->config != config) {
      memcpy(&ctx->config, config, sizeof(*config));
    }

    ctx->configured = true;

  } while(0);

  return err;
}

error_t ckp_reset(ckp_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    ctx->configured = false;

  } while(0);

  return err;
}

void ckp_loop_main(ckp_ctx_t *ctx)
{

}

void ckp_loop_slow(ckp_ctx_t *ctx)
{

}

ITCM_FUNC void ckp_loop_fast(ckp_ctx_t *ctx)
{

}
