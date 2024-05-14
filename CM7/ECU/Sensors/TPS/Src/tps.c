/*
 * tps.c
 *
 *  Created on: May 14, 2024
 *      Author: VHEMaster
 */

#include "tps.h"
#include "compiler.h"
#include <string.h>

error_t tps_init(tps_ctx_t *ctx, const tps_init_ctx_t *init_ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || init_ctx == NULL, err = E_PARAM);

    memset(ctx, 0u, sizeof(tps_ctx_t));
    memcpy(&ctx->init, init_ctx, sizeof(tps_init_ctx_t));

    ctx->ready = true;

  } while(0);

  return err;
}

error_t tps_configure(tps_ctx_t *ctx, const tps_config_t *config)
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

error_t tps_reset(tps_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    ctx->configured = false;

  } while(0);

  return err;
}

void tps_loop_main(tps_ctx_t *ctx)
{

}

void tps_loop_slow(tps_ctx_t *ctx)
{

}

ITCM_FUNC void tps_loop_fast(tps_ctx_t *ctx)
{

}
