/*
 * ect.c
 *
 *  Created on: May 14, 2024
 *      Author: VHEMaster
 */

#include "ect.h"
#include "compiler.h"
#include <string.h>

error_t ect_init(ect_ctx_t *ctx, const ect_init_ctx_t *init_ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || init_ctx == NULL, err = E_PARAM);

    memset(ctx, 0u, sizeof(ect_ctx_t));
    memcpy(&ctx->init, init_ctx, sizeof(ect_init_ctx_t));

    ctx->ready = true;

  } while(0);

  return err;
}

error_t ect_configure(ect_ctx_t *ctx, const ect_config_t *config)
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

error_t ect_reset(ect_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    ctx->configured = false;

  } while(0);

  return err;
}

void ect_loop_main(ect_ctx_t *ctx)
{

}

void ect_loop_slow(ect_ctx_t *ctx)
{

}

ITCM_FUNC void ect_loop_fast(ect_ctx_t *ctx)
{

}
