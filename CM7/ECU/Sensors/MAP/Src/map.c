/*
 * map.c
 *
 *  Created on: May 14, 2024
 *      Author: VHEMaster
 */

#include "map.h"
#include "compiler.h"
#include <string.h>

error_t map_init(map_ctx_t *ctx, const map_init_ctx_t *init_ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || init_ctx == NULL, err = E_PARAM);

    memset(ctx, 0u, sizeof(map_ctx_t));
    memcpy(&ctx->init, init_ctx, sizeof(map_init_ctx_t));

    ctx->ready = true;

  } while(0);

  return err;
}

error_t map_configure(map_ctx_t *ctx, const map_config_t *config)
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

error_t map_reset(map_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    ctx->configured = false;

  } while(0);

  return err;
}

void map_loop_main(map_ctx_t *ctx)
{

}

void map_loop_slow(map_ctx_t *ctx)
{

}

ITCM_FUNC void map_loop_fast(map_ctx_t *ctx)
{

}
