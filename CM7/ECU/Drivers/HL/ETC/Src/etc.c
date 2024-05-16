/*
 * etc.c
 *
 *  Created on: May 16, 2024
 *      Author: VHEMaster
 */

#include "etc.h"
#include "compiler.h"
#include <string.h>

error_t etc_init(etc_ctx_t *ctx, const etc_init_ctx_t *init_ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || init_ctx == NULL, err = E_PARAM);

    memset(ctx, 0u, sizeof(etc_ctx_t));
    memcpy(&ctx->init, init_ctx, sizeof(etc_init_ctx_t));

    ctx->ready = true;

  } while(0);

  return err;
}

error_t etc_configure(etc_ctx_t *ctx, const etc_config_t *config)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || config == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    ctx->configured = false;

    if(&ctx->config != config) {
      memcpy(&ctx->config, config, sizeof(*config));
    }

    if(ctx->config.enabled == true) {

      ctx->configured = true;
    }

  } while(0);

  return err;
}

error_t etc_reset(etc_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    ctx->configured = false;

  } while(0);

  return err;
}

void etc_loop_main(etc_ctx_t *ctx)
{

}

void etc_loop_slow(etc_ctx_t *ctx)
{

}

ITCM_FUNC void etc_loop_fast(etc_ctx_t *ctx)
{

}
