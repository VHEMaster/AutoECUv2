/*
 * tunerstudio.c
 *
 *  Created on: Nov 26, 2026
 *      Author: VHEMaster
 */

#include "tunerstudio.h"
#include "tunerstudio_rs422.h"
#include "tunerstudio_protocol.h"

error_t tunerstudio_init(tunerstudio_ctx_t *ctx, const tunerstudio_init_ctx_t *init)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || init == NULL, err = E_PARAM);

    memset(ctx, 0u, sizeof(tunerstudio_ctx_t));
    memcpy(&ctx->init, init, sizeof(tunerstudio_init_ctx_t));
    tunerstudio_arbiter_reset(&ctx->arbiter, ctx->generation);
    tunerstudio_rs422_init(&ctx->rs422, ctx);
    ctx->initialized = true;

  } while(0);

  return err;
}

error_t tunerstudio_configure(tunerstudio_ctx_t *ctx, const tunerstudio_config_t *config)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || config == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->initialized == false, err = E_INVALACT);
    BREAK_IF_ACTION(config->enabled && !config->lwip_enabled && !config->rs422_enabled, err = E_PARAM);
    BREAK_IF_ACTION(config->lwip_enabled && config->port == 0u, err = E_PARAM);
    BREAK_IF_ACTION(config->rs422_enabled && (config->rs422_instance >= ECU_COMM_RS422_MAX ||
        ctx->init.rs422_ctx[config->rs422_instance] == NULL), err = E_PARAM);
    BREAK_IF_ACTION(!ATOMIC_TRY_LOCK(ctx->lock), err = E_AGAIN);

    ctx->configured = false;
    memcpy(&ctx->config, config, sizeof(tunerstudio_config_t));
    ctx->generation++;
    ctx->configured = ctx->config.enabled;
    ATOMIC_RELEASE(ctx->lock);

  } while(0);

  return err;
}

error_t tunerstudio_reset(tunerstudio_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->initialized == false, err = E_INVALACT);
    BREAK_IF_ACTION(!ATOMIC_TRY_LOCK(ctx->lock), err = E_AGAIN);

    /* The transports will reset their stream state on the next poll. */
    ctx->generation++;
    ATOMIC_RELEASE(ctx->lock);

  } while(0);

  return err;
}

void tunerstudio_loop_main(tunerstudio_ctx_t *ctx)
{
  do {
    BREAK_IF(ctx == NULL || ctx->initialized == false);

    tunerstudio_rs422_loop(&ctx->rs422);

    BREAK_IF(!ATOMIC_TRY_LOCK(ctx->lock));
    tunerstudio_arbiter_sync(&ctx->arbiter, ctx->generation);

    if(ctx->configured) {
      tunerstudio_protocol_loop(ctx);
    }
    ATOMIC_RELEASE(ctx->lock);

  } while(0);
}
