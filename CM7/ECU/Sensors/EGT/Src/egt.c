/*
 * egt.c
 *
 *  Created on: May 14, 2024
 *      Author: VHEMaster
 */

#include "egt.h"
#include "compiler.h"
#include <string.h>

error_t egt_init(egt_ctx_t *ctx, const egt_init_ctx_t *init_ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || init_ctx == NULL, err = E_PARAM);

    memset(ctx, 0u, sizeof(egt_ctx_t));
    memcpy(&ctx->init, init_ctx, sizeof(egt_init_ctx_t));

    ctx->ready = true;

  } while(0);

  return err;
}

error_t egt_configure(egt_ctx_t *ctx, const egt_config_t *config)
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

error_t egt_reset(egt_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    ctx->configured = false;

  } while(0);

  return err;
}

void egt_loop_main(egt_ctx_t *ctx)
{

}

void egt_loop_slow(egt_ctx_t *ctx)
{

}

ITCM_FUNC void egt_loop_fast(egt_ctx_t *ctx)
{

}
