/*
 * lwip.c
 *
 *  Created on: 17 нояб. 2026 г.
 *      Author: VHEMaster
 */

#include "lwip.h"

error_t lwip_init(lwip_ctx_t *ctx, const lwip_init_ctx_t *init)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(init == NULL, err = E_PARAM);

    memset(ctx, 0u, sizeof(lwip_ctx_t));
    memcpy(&ctx->init, init, sizeof(lwip_init_ctx_t));

    ctx->initialized = true;

  } while(0);

  return err;
}

error_t lwip_configure(lwip_ctx_t *ctx, const lwip_config_t *config)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->initialized == false, err = E_INVALACT);
    BREAK_IF_ACTION(config == NULL, err = E_PARAM);

    memcpy(&ctx->config, config, sizeof(lwip_config_t));

    ctx->configured = ctx->config.enabled;

  } while(0);

  return err;
}

error_t lwip_reset(lwip_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->configured == false, err = E_INVALACT);

    ctx->reset_trigger = true;

  } while(0);

  return err;
}

void lwip_loop_slow(lwip_ctx_t *ctx)
{
  do {
    BREAK_IF(ctx == NULL);
    BREAK_IF(ctx->configured == false);

  } while(0);
}

void lwip_loop_comm(lwip_ctx_t *ctx)
{
  do {
    BREAK_IF(ctx == NULL);
    BREAK_IF(ctx->configured == false);

  } while(0);
}

void lwip_loop_main(lwip_ctx_t *ctx)
{
  do {
    BREAK_IF(ctx == NULL);
    BREAK_IF(ctx->configured == false);

  } while(0);
}
