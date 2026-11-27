/*
 * tunerstudio_rs422.c
 *
 *  Created on: Nov 26, 2026
 *      Author: VHEMaster
 */

#include "tunerstudio_rs422.h"
#include "tunerstudio.h"
#include "rs422.h"

void tunerstudio_rs422_init(tunerstudio_rs422_ctx_t *ctx, tunerstudio_ctx_t *tunerstudio_ctx)
{
  do {
    BREAK_IF(ctx == NULL);

    memset(ctx, 0u, sizeof(*ctx));
    ctx->tunerstudio_ctx = tunerstudio_ctx;

  } while(0);
}

void tunerstudio_rs422_select(tunerstudio_rs422_ctx_t *ctx)
{
  tunerstudio_ctx_t *tunerstudio_ctx;

  do {
    BREAK_IF(ctx == NULL || ctx->tunerstudio_ctx == NULL);

    tunerstudio_ctx = ctx->tunerstudio_ctx;
    BREAK_IF(!ATOMIC_TRY_LOCK(tunerstudio_ctx->lock));

    if(ctx->generation != tunerstudio_ctx->generation) {
      ctx->rs422_ctx = NULL;
      ctx->available = false;
      tunerstudio_arbiter_sync(&tunerstudio_ctx->arbiter, tunerstudio_ctx->generation);
      ctx->error_code = E_OK;
      ctx->generation = tunerstudio_ctx->generation;

      if(tunerstudio_ctx->configured && tunerstudio_ctx->config.rs422_enabled) {
        ctx->rs422_ctx = tunerstudio_ctx->init.rs422_ctx[tunerstudio_ctx->config.rs422_instance];
      }
    }
    ATOMIC_RELEASE(tunerstudio_ctx->lock);

  } while(0);
}

static void tunerstudio_rs422_stream_loop(tunerstudio_rs422_ctx_t *ctx)
{
  tunerstudio_arbiter_t *arbiter = &ctx->tunerstudio_ctx->arbiter;
  error_t err;
  const uint8_t *tx;
  uint8_t *rx;
  const void *buffer;
  uint16_t capacity;
  uint16_t length;
  uint32_t bytes;

  do {
    err = tunerstudio_arbiter_tx_peek(arbiter, TUNERSTUDIO_TRANSPORT_RS422, &tx, &length);
    if(err == E_OK) {
      err = rs422_tx(ctx->rs422_ctx, tx, length);
      BREAK_IF(err == E_AGAIN);
      BREAK_IF_ACTION(err != E_OK, ctx->error_code = err);

      err = tunerstudio_arbiter_tx_consume(arbiter, TUNERSTUDIO_TRANSPORT_RS422, length);
      BREAK_IF_ACTION(err != E_OK, ctx->error_code = err);
    }

    err = tunerstudio_arbiter_rx_acquire(arbiter, TUNERSTUDIO_TRANSPORT_RS422, &rx, &capacity);
    BREAK_IF(err != E_OK);

    bytes = capacity;
    err = rs422_rx_nocopy(ctx->rs422_ctx, &buffer, &bytes);
    BREAK_IF_ACTION(err != E_OK, ctx->error_code = err);
    BREAK_IF(bytes == 0u);

    memcpy(rx, buffer, bytes);
    err = rs422_rx_release(ctx->rs422_ctx, bytes);
    BREAK_IF_ACTION(err != E_OK, ctx->error_code = err);

    ctx->error_code = tunerstudio_arbiter_rx_commit(arbiter, TUNERSTUDIO_TRANSPORT_RS422, bytes);

  } while(0);
}

void tunerstudio_rs422_loop(tunerstudio_rs422_ctx_t *ctx)
{
  tunerstudio_ctx_t *tunerstudio_ctx;

  do {
    BREAK_IF(ctx == NULL || ctx->tunerstudio_ctx == NULL || ctx->rs422_ctx == NULL);

    tunerstudio_ctx = ctx->tunerstudio_ctx;
    BREAK_IF(!ATOMIC_TRY_LOCK(tunerstudio_ctx->lock));

    if(ctx->generation == tunerstudio_ctx->generation) {
      if(ctx->available && ctx->rs422_ctx->configured) {
        tunerstudio_rs422_stream_loop(ctx);
      } else {
        tunerstudio_arbiter_channel_reset(&tunerstudio_ctx->arbiter, TUNERSTUDIO_TRANSPORT_RS422);
      }
    }
    ATOMIC_RELEASE(tunerstudio_ctx->lock);

  } while(0);
}
