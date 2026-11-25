/*
 * usb_private.c
 *
 *  Created on: 25 нояб. 2026 г.
 *      Author: VHEMaster
 */

#include "compiler.h"
#include "usb_private.h"
#include "usbd_vendor.h"

static usb_ctx_t *usb_device_ctx;

void usb_set_device_ctx(usb_ctx_t *ctx)
{
  usb_device_ctx = ctx;
}

void usb_clear(usb_ctx_t *ctx)
{
  ctx->rx_read_pointer = 0u;
  ctx->rx_write_pointer = 0u;
  ctx->tx_read_pointer = 0u;
  ctx->tx_write_pointer = 0u;
  ctx->rx_armed = false;
  ctx->tx_busy = false;
  ctx->tx_zlp = false;
  ctx->tx_length = 0u;
}

void usb_ring_write(uint8_t *ring, uint32_t capacity, uint32_t offset, const void *data, uint32_t length)
{
  uint32_t position = offset & (capacity - 1u);
  uint32_t first = MIN(capacity - position, length);

  memcpy(ring + position, data, first);
  if(length > first) {
    memcpy(ring, (const uint8_t *)data + first, length - first);
  }
}

void usb_ring_read(const uint8_t *ring, uint32_t capacity, uint32_t offset, void *data, uint32_t length)
{
  uint32_t position = offset & (capacity - 1u);
  uint32_t first = MIN(capacity - position, length);

  memcpy(data, ring + position, first);
  if(length > first) {
    memcpy((uint8_t *)data + first, ring, length - first);
  }
}

void usb_arm_rx(usb_ctx_t *ctx, USBD_HandleTypeDef *pdev)
{
  uint32_t used;
  uint32_t prim;

  prim = EnterCritical();

  do {
    BREAK_IF(ctx->rx_armed || ATOMIC_LOAD(ctx->attached) == false);

    used = ctx->rx_write_pointer - ctx->rx_read_pointer;
    BREAK_IF(ctx->init.rx_buffer_size - used < ctx->init.rx_packet_size);
    BREAK_IF(USBD_VENDOR_Receive(pdev, ctx->init.rx_packet, ctx->init.rx_packet_size) != USBD_OK);

    ctx->rx_armed = true;

  } while(0);

  ExitCritical(prim);
}

void usb_start_tx(usb_ctx_t *ctx, USBD_HandleTypeDef *pdev)
{
  uint32_t length;
  uint32_t prim;

  prim = EnterCritical();
  do {
    BREAK_IF(ctx->tx_busy || ATOMIC_LOAD(ctx->attached) == false);
    BREAK_IF(pdev->dev_state != USBD_STATE_CONFIGURED);

    length = MIN(ctx->tx_write_pointer - ctx->tx_read_pointer, ctx->init.tx_packet_size);
    BREAK_IF(length == 0u && ctx->tx_zlp == false);

    if(length > 0u) {
      usb_ring_read(ctx->init.tx_buffer, ctx->init.tx_buffer_size, ctx->tx_read_pointer, ctx->init.tx_packet, length);
    }

    BREAK_IF(USBD_VENDOR_Transmit(pdev, ctx->init.tx_packet, length) != USBD_OK);

    ctx->tx_length = length;
    ctx->tx_busy = true;
    ctx->tx_zlp = false;

  } while(0);

  ExitCritical(prim);
}

bool usb_config_matches(const usb_ctx_t *ctx, const usb_config_t *config, const uint32_t uid[3])
{
  return ctx->config.enabled == config->enabled &&
    memcmp(ctx->uid, uid, sizeof(ctx->uid)) == 0;
}


void USBD_VENDOR_Opened(USBD_HandleTypeDef *pdev)
{
  usb_ctx_t *ctx = usb_device_ctx;

  UNUSED(pdev);

  do {
    BREAK_IF(ctx == NULL);

    usb_clear(ctx);
    ATOMIC_POST_INCREMENT(ctx->generation, 1u);
    ATOMIC_STORE(ctx->attached, true);

  } while(0);
}

void USBD_VENDOR_Closed(USBD_HandleTypeDef *pdev)
{
  usb_ctx_t *ctx = usb_device_ctx;

  UNUSED(pdev);

  do {
    BREAK_IF(ctx == NULL || ATOMIC_LOAD(ctx->attached) == false);

    ATOMIC_STORE(ctx->attached, false);
    ATOMIC_POST_INCREMENT(ctx->generation, 1u);
    usb_clear(ctx);

  } while(0);
}

void USBD_VENDOR_Received(USBD_HandleTypeDef *pdev, uint32_t length)
{
  usb_ctx_t *ctx = usb_device_ctx;
  uint32_t used;

  UNUSED(pdev);

  do {
    BREAK_IF(ctx == NULL || ctx->rx_armed == false);

    ctx->rx_armed = false;
    used = ctx->rx_write_pointer - ctx->rx_read_pointer;
    BREAK_IF(length == 0u || length > ctx->init.rx_packet_size || length > ctx->init.rx_buffer_size - used);

    usb_ring_write(ctx->init.rx_buffer, ctx->init.rx_buffer_size, ctx->rx_write_pointer, ctx->init.rx_packet, length);
    ctx->rx_write_pointer += length;

  } while(0);
}

void USBD_VENDOR_Transmitted(USBD_HandleTypeDef *pdev)
{
  usb_ctx_t *ctx = usb_device_ctx;

  UNUSED(pdev);

  do {
    BREAK_IF(ctx == NULL || ctx->tx_busy == false);

    ctx->tx_read_pointer += ctx->tx_length;
    ctx->tx_zlp = ctx->tx_length != 0u && (ctx->tx_length % ctx->init.rx_packet_size) == 0u;
    ctx->tx_length = 0u;
    ctx->tx_busy = false;

  } while(0);
}
