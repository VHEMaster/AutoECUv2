/*
 * usb.c
 *
 *  Created on: 25 нояб. 2026 г.
 *      Author: VHEMaster
 */

#include "usb.h"

error_t usb_init(usb_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);

    memset(ctx, 0u, sizeof(usb_ctx_t));
    ctx->initialized = true;

  } while(0);

  return err;
}

static bool usb_config_matches(const usb_ctx_t *ctx, const usb_config_t *config, const uint32_t uid[3])
{
  return ctx->config.enabled == config->enabled && ctx->config.vid == config->vid &&
      ctx->config.pid == config->pid && memcmp(ctx->uid, uid, sizeof(ctx->uid)) == 0;
}

error_t usb_configure(usb_ctx_t *ctx, const usb_config_t *config, const uint32_t uid[3])
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->initialized == false, err = E_INVALACT);
    BREAK_IF_ACTION(config == NULL || uid == NULL, err = E_PARAM);
    BREAK_IF_ACTION(config->enabled && (config->vid == 0u || config->pid == 0u), err = E_PARAM);

    if(ATOMIC_LOAD(ctx->request_init)) {
      err = ATOMIC_LOAD(ctx->request_errcode);
      BREAK_IF(err == E_AGAIN);

      ATOMIC_STORE(ctx->request_init, false);
      BREAK_IF(!ctx->request_reset && usb_config_matches(ctx, config, uid));
      err = E_OK;
    }

    BREAK_IF(ctx->configured && usb_config_matches(ctx, config, uid));

    if(&ctx->config != config) {
      memcpy(&ctx->config, config, sizeof(usb_config_t));
    }
    if(ctx->uid != uid) {
      memcpy(ctx->uid, uid, sizeof(ctx->uid));
    }

    ctx->request_reset = false;
    ATOMIC_STORE(ctx->request_errcode, E_AGAIN);
    ATOMIC_STORE(ctx->request_init, true);
    err = E_AGAIN;

  } while(0);

  return err;
}

error_t usb_reset(usb_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->initialized == false, err = E_INVALACT);

    if(ATOMIC_LOAD(ctx->request_init)) {
      err = ATOMIC_LOAD(ctx->request_errcode);
      BREAK_IF(err == E_AGAIN);

      ATOMIC_STORE(ctx->request_init, false);
      BREAK_IF(ctx->request_reset);
      err = E_OK;
    }

    if(ctx->configured) {
      ctx->request_reset = true;
      ATOMIC_STORE(ctx->request_errcode, E_AGAIN);
      ATOMIC_STORE(ctx->request_init, true);
      err = E_AGAIN;
    }

  } while(0);

  return err;
}

void usb_loop_main(usb_ctx_t *ctx)
{
  do {
    BREAK_IF(ctx == NULL || ctx->initialized == false);

    usb_fsm(ctx);

  } while(0);
}
