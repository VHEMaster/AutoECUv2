/*
 * usb_private.h
 *
 *  Created on: 25 нояб. 2026 г.
 *      Author: VHEMaster
 */

#ifndef DRIVERS_USB_INC_USB_PRIVATE_H_
#define DRIVERS_USB_INC_USB_PRIVATE_H_

#include "usb.h"
#include "usb_device.h"

void usb_set_device_ctx(usb_ctx_t *ctx);
void usb_sync_rings(usb_ctx_t *ctx);
void usb_ring_write(uint8_t *ring, uint32_t capacity, uint32_t offset, const void *data, uint32_t length);
void usb_ring_read(const uint8_t *ring, uint32_t capacity, uint32_t offset, void *data, uint32_t length);
void usb_arm_rx(usb_ctx_t *ctx, USBD_HandleTypeDef *pdev);
void usb_start_tx(usb_ctx_t *ctx, USBD_HandleTypeDef *pdev);
bool usb_config_matches(const usb_ctx_t *ctx, const usb_config_t *config, const uint32_t uid[3]);

#endif /* DRIVERS_USB_INC_USB_PRIVATE_H_ */
