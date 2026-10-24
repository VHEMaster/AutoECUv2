/*
 * signals.c
 *
 *  Created on: 13 окт. 2026 г.
 *      Author: VHEMaster
 */

#include "signals.h"
#include "signals_private.h"

error_t signals_init(signals_ctx_t *ctx, const signals_init_ctx_t *init)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(init == NULL, err = E_PARAM);

    memset(ctx, 0u, sizeof(signals_ctx_t));
    memcpy(&ctx->init, init, sizeof(signals_init_ctx_t));

    ctx->initialized = true;

  } while(0);

  return err;
}

error_t signals_configure(signals_ctx_t *ctx, const signals_config_t *config)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->initialized == false, err = E_INVALACT);
    BREAK_IF_ACTION(config == NULL, err = E_PARAM);

    ctx->configured = false;
    memcpy(&ctx->config, config, sizeof(signals_config_t));
    memset(&ctx->runtime, 0u, sizeof(signals_runtime_ctx_t));

    if(ctx->config.enabled) {
      err = signals_config_build(ctx);
      BREAK_IF(err != E_OK);
    }

    ctx->configured = ctx->config.enabled;

  } while(0);

  return err;
}

error_t signals_reset(signals_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->configured == false, err = E_INVALACT);

    ctx->reset_trigger = true;

  } while(0);

  return err;
}

void signals_loop_slow(signals_ctx_t *ctx)
{
  do {
    BREAK_IF(ctx == NULL);
    BREAK_IF(ctx->configured == false);

  } while(0);
}

void signals_loop_comm(signals_ctx_t *ctx)
{
  do {
    BREAK_IF(ctx == NULL);
    BREAK_IF(ctx->configured == false);

    if(ctx->reset_trigger) {
      memset(&ctx->runtime, 0u, sizeof(ctx->runtime));
      ctx->configured = false;
      ctx->reset_trigger = false;
      return;
    }

    signals_handle_upstream(ctx);
    signals_handle_downstream(ctx);

  } while(0);
}

void signals_loop_main(signals_ctx_t *ctx)
{
  do {
    BREAK_IF(ctx == NULL);
    BREAK_IF(ctx->configured == false);

  } while(0);
}

error_t signals_data_write_upstream(signals_ctx_t *ctx, const signals_runtime_messages_fifo_item_t *message)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->configured == false, err = E_INVALACT);
    BREAK_IF_ACTION(message == NULL, err = E_PARAM);

    err = signals_frame_fifo_push(&ctx->runtime.fifo_messages_upstream, message);

  } while(0);

  return err;
}

error_t signals_data_read_downstream(signals_ctx_t *ctx, signals_runtime_messages_fifo_item_t *message)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->configured == false, err = E_INVALACT);
    BREAK_IF_ACTION(message == NULL, err = E_PARAM);

    err = signals_frame_fifo_pull(&ctx->runtime.fifo_messages_downstream, message);

  } while(0);

  return err;
}
