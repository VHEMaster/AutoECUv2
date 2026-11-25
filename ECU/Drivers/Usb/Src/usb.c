/*
 * usb.c
 *
 *  Created on: 25 нояб. 2026 г.
 *      Author: VHEMaster
 */

#include "usb.h"
#include "usb_device.h"
#include "usbd_desc.h"

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

error_t usb_configure(usb_ctx_t *ctx, const usb_config_t *config, const uint32_t uid[3])
{
  error_t err = E_OK;
  USBD_StatusTypeDef status;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->initialized == false, err = E_INVALACT);
    BREAK_IF_ACTION(config == NULL || uid == NULL, err = E_PARAM);
    BREAK_IF_ACTION(config->enabled && (config->vid == 0u || config->pid == 0u), err = E_PARAM);

    BREAK_IF(ctx->configured && config->enabled &&
        ctx->config.vid == config->vid && ctx->config.pid == config->pid &&
        memcmp(ctx->uid, uid, sizeof(ctx->uid)) == 0);

    err = usb_reset(ctx);
    BREAK_IF(err != E_OK);

    if(&ctx->config != config) {
      memcpy(&ctx->config, config, sizeof(usb_config_t));
    }
    if(ctx->uid != uid) {
      memcpy(ctx->uid, uid, sizeof(ctx->uid));
    }

    if(ctx->config.enabled) {
      USBD_FS_SetIdentity(ctx->config.vid, ctx->config.pid, ctx->uid);
      status = USB_DEVICE_Start();
      BREAK_IF_ACTION(status != USBD_OK, err = E_FAULT);

      ctx->configured = true;

    }

  } while(0);

  return err;
}

error_t usb_reset(usb_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->initialized == false, err = E_INVALACT);

    if(ctx->configured) {
      ctx->configured = false;
      USB_DEVICE_Stop();
    }

  } while(0);

  return err;
}

void usb_loop_main(usb_ctx_t *ctx)
{
  (void)ctx;
}
