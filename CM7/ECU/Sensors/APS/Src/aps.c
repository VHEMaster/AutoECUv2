/*
 * aps.c
 *
 *  Created on: May 14, 2024
 *      Author: VHEMaster
 */

#include "aps.h"
#include "compiler.h"
#include <string.h>

error_t aps_init(aps_ctx_t *ctx, const aps_init_ctx_t *init_ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || init_ctx == NULL, err = E_PARAM);

    memset(ctx, 0u, sizeof(aps_ctx_t));
    memcpy(&ctx->init, init_ctx, sizeof(aps_init_ctx_t));

    ctx->ready = true;

  } while(0);

  return err;
}

error_t aps_configure(aps_ctx_t *ctx, const aps_config_t *config)
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

error_t aps_reset(aps_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    ctx->configured = false;

  } while(0);

  return err;
}

void aps_loop_main(aps_ctx_t *ctx)
{

}

void aps_loop_slow(aps_ctx_t *ctx)
{

}

ITCM_FUNC void aps_loop_fast(aps_ctx_t *ctx)
{

}
