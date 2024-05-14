/*
 * cmp.c
 *
 *  Created on: May 14, 2024
 *      Author: VHEMaster
 */

#include "cmp.h"
#include "compiler.h"
#include <string.h>

error_t cmp_init(cmp_ctx_t *ctx, const cmp_init_ctx_t *init_ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || init_ctx == NULL, err = E_PARAM);

    memset(ctx, 0u, sizeof(cmp_ctx_t));
    memcpy(&ctx->init, init_ctx, sizeof(cmp_init_ctx_t));

    ctx->ready = true;

  } while(0);

  return err;
}

error_t cmp_configure(cmp_ctx_t *ctx, const cmp_config_t *config)
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

error_t cmp_reset(cmp_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    ctx->configured = false;

  } while(0);

  return err;
}

void cmp_loop_main(cmp_ctx_t *ctx)
{

}

void cmp_loop_slow(cmp_ctx_t *ctx)
{

}

ITCM_FUNC void cmp_loop_fast(cmp_ctx_t *ctx)
{

}
