/*
 * iat.c
 *
 *  Created on: May 14, 2024
 *      Author: VHEMaster
 */

#include "iat.h"
#include "compiler.h"
#include <string.h>

error_t iat_init(iat_ctx_t *ctx, const iat_init_ctx_t *init_ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || init_ctx == NULL, err = E_PARAM);

    memset(ctx, 0u, sizeof(iat_ctx_t));
    memcpy(&ctx->init, init_ctx, sizeof(iat_init_ctx_t));

    ctx->ready = true;

  } while(0);

  return err;
}

error_t iat_configure(iat_ctx_t *ctx, const iat_config_t *config)
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

error_t iat_reset(iat_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    ctx->configured = false;

  } while(0);

  return err;
}

void iat_loop_main(iat_ctx_t *ctx)
{

}

void iat_loop_slow(iat_ctx_t *ctx)
{

}

ITCM_FUNC void iat_loop_fast(iat_ctx_t *ctx)
{

}
