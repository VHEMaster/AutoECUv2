/*
 * usb.c
 *
 *  Created on: 25 нояб. 2026 г.
 *      Author: VHEMaster
 */

#include "usb.h"
#include "usb_device.h"
#include "usbd_vendor.h"
#include "usb_private.h"

error_t usb_init(usb_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);

    memset(ctx, 0u, sizeof(usb_ctx_t));

    usb_set_device_ctx(ctx);

    ctx->initialized = true;

  } while(0);

  return err;
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
  uint32_t prim;

  do {
    BREAK_IF(ctx == NULL || ctx->initialized == false);

    usb_fsm(ctx);

    BREAK_IF(ctx->configured == false);

    prim = EnterCritical();
    usb_arm_rx(ctx, &hUsbDeviceFS);
    usb_start_tx(ctx, &hUsbDeviceFS);
    ExitCritical(prim);

  } while(0);
}

bool usb_is_connected(const usb_ctx_t *ctx)
{
  return ctx != NULL && ctx->configured && ATOMIC_LOAD(ctx->attached);
}

uint32_t usb_get_generation(const usb_ctx_t *ctx)
{
  return ctx != NULL ? ATOMIC_LOAD(ctx->generation) : 0u;
}

error_t usb_tx(usb_ctx_t *ctx, const void *buffer, uint32_t length)
{
  error_t err = E_OK;
  uint32_t prim;
  uint32_t used;

  RETURN_IF(ctx == NULL, E_PARAM);
  RETURN_IF(length > 0u && buffer == NULL, E_PARAM);

  prim = EnterCritical();

  do {
    BREAK_IF_ACTION(usb_is_connected(ctx) == false, err = E_INVALACT);

    used = ctx->tx_write_pointer - ctx->tx_read_pointer;
    BREAK_IF_ACTION(length > USB_TX_BUFFER_SIZE - used, err = E_AGAIN);
    BREAK_IF(length == 0u);

    usb_ring_write(ctx->tx_buffer, USB_TX_BUFFER_SIZE, ctx->tx_write_pointer, buffer, length);
    ctx->tx_write_pointer += length;

  } while(0);

  ExitCritical(prim);

  return err;
}

error_t usb_rx(usb_ctx_t *ctx, void *buffer, uint32_t *p_length)
{
  uint32_t prim;
  uint32_t length;

  RETURN_IF(ctx == NULL || p_length == NULL, E_PARAM);
  RETURN_IF(*p_length > 0u && buffer == NULL, E_PARAM);

  prim = EnterCritical();

  length = MIN(ctx->rx_write_pointer - ctx->rx_read_pointer, *p_length);
  if(length > 0u) {
    usb_ring_read(ctx->rx_buffer, USB_RX_BUFFER_SIZE, ctx->rx_read_pointer, buffer, length);
    ctx->rx_read_pointer += length;
  }
  *p_length = length;

  ExitCritical(prim);

  return length > 0u ? E_OK : E_AGAIN;
}
