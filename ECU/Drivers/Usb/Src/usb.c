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

error_t usb_init(usb_ctx_t *ctx, const usb_init_ctx_t *init_ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION((init_ctx->vid == 0u || init_ctx->pid == 0u), err = E_PARAM);

    memset(ctx, 0u, sizeof(usb_ctx_t));
    memcpy(&ctx->init, init_ctx, sizeof(usb_init_ctx_t));

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
    usb_sync_rings(ctx);

    BREAK_IF(ctx->configured == false);

    usb_arm_rx(ctx, &hUsbDeviceFS);
    usb_start_tx(ctx, &hUsbDeviceFS);

  } while(0);
}

bool usb_is_connected(const usb_ctx_t *ctx)
{
  return ctx != NULL && ctx->configured && ATOMIC_LOAD(ctx->attached) &&
      ctx->ring_generation == ATOMIC_LOAD(ctx->generation);
}

uint32_t usb_get_generation(const usb_ctx_t *ctx)
{
  return ctx != NULL ? ATOMIC_LOAD(ctx->generation) : 0u;
}

error_t usb_tx(usb_ctx_t *ctx, const void *buffer, uint32_t length)
{
  error_t err = E_OK;
  uint32_t used;

  RETURN_IF(ctx == NULL, E_PARAM);
  RETURN_IF(length > 0u && buffer == NULL, E_PARAM);

  do {
    BREAK_IF_ACTION(usb_is_connected(ctx) == false, err = E_INVALACT);

    used = ctx->tx_write_pointer - ctx->tx_read_pointer;
    BREAK_IF_ACTION(length > ctx->init.tx_buffer_size - used, err = E_AGAIN);
    BREAK_IF(length == 0u);

    usb_ring_write(ctx->init.tx_buffer, ctx->init.tx_buffer_size, ctx->tx_write_pointer, buffer, length);
    BREAK_IF_ACTION(usb_is_connected(ctx) == false, err = E_INVALACT);
    ctx->tx_write_pointer += length;

  } while(0);

  return err;
}

error_t usb_rx(usb_ctx_t *ctx, void *buffer, uint32_t *p_length)
{
  error_t err = E_AGAIN;
  uint32_t length = 0u;

  RETURN_IF(ctx == NULL || p_length == NULL, E_PARAM);
  RETURN_IF(*p_length > 0u && buffer == NULL, E_PARAM);

  do {
    BREAK_IF(usb_is_connected(ctx) == false);
    length = MIN(ctx->rx_write_pointer - ctx->rx_read_pointer, *p_length);
    BREAK_IF(length == 0u);

    usb_ring_read(ctx->init.rx_buffer, ctx->init.rx_buffer_size, ctx->rx_read_pointer, buffer, length);
    BREAK_IF_ACTION(usb_is_connected(ctx) == false, length = 0u);
    ctx->rx_read_pointer += length;
    err = E_OK;

  } while(0);

  *p_length = length;

  return err;
}

error_t usb_rx_nocopy(usb_ctx_t *ctx, const void **p_buffer, uint32_t *p_length)
{
  error_t err = E_OK;
  uint32_t capacity;
  uint32_t position;
  uint32_t length;

  RETURN_IF(ctx == NULL || p_buffer == NULL || p_length == NULL, E_PARAM);

  capacity = *p_length;
  *p_length = 0u;
  *p_buffer = NULL;

  do {
    BREAK_IF(usb_is_connected(ctx) == false);
    position = ctx->rx_read_pointer & (ctx->init.rx_buffer_size - 1u);
    length = MIN(ctx->rx_write_pointer - ctx->rx_read_pointer, capacity);
    length = MIN(length, ctx->init.rx_buffer_size - position);
    BREAK_IF(length == 0u);

    *p_buffer = &ctx->init.rx_buffer[position];
    *p_length = length;

  } while(0);

  return err;
}

error_t usb_rx_release(usb_ctx_t *ctx, uint32_t length)
{
  error_t err = E_OK;
  uint32_t position;
  uint32_t available;

  RETURN_IF(ctx == NULL, E_PARAM);

  do {
    position = ctx->rx_read_pointer & (ctx->init.rx_buffer_size - 1u);
    available = MIN(ctx->rx_write_pointer - ctx->rx_read_pointer, ctx->init.rx_buffer_size - position);
    BREAK_IF_ACTION(length == 0u || length > available, err = E_PARAM);

    ctx->rx_read_pointer += length;

  } while(0);

  return err;
}
