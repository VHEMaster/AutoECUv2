/*
 * maf.c
 *
 *  Created on: May 14, 2024
 *      Author: VHEMaster
 */

#include "maf.h"
#include "compiler.h"
#include <string.h>

error_t maf_init(maf_ctx_t *ctx, const maf_init_ctx_t *init_ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || init_ctx == NULL, err = E_PARAM);

    memset(ctx, 0u, sizeof(maf_ctx_t));
    memcpy(&ctx->init, init_ctx, sizeof(maf_init_ctx_t));

    ctx->ready = true;

  } while(0);

  return err;
}

error_t maf_configure(maf_ctx_t *ctx, const maf_config_t *config)
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

error_t maf_reset(maf_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    ctx->configured = false;

  } while(0);

  return err;
}

void maf_loop_main(maf_ctx_t *ctx)
{

}

void maf_loop_slow(maf_ctx_t *ctx)
{

}

ITCM_FUNC void maf_loop_fast(maf_ctx_t *ctx)
{

}
