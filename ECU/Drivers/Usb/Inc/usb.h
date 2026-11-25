/*
 * usb.h
 *
 *  Created on: 25 нояб. 2026 г.
 *      Author: VHEMaster
 */

#ifndef DRIVERS_USB_INC_USB_H_
#define DRIVERS_USB_INC_USB_H_

#include "common.h"
#include "versioned_usb.h"
#include "usb_fsm.h"

#define USB_RX_BUFFER_SIZE 4096u
#define USB_TX_BUFFER_SIZE 4096u
#define USB_PACKET_SIZE 64u
#define USB_TX_TRANSFER_SIZE 256u

typedef struct usb_ctx_tag {
    usb_config_t config;
    usb_fsm_t fsm;
    bool request_init;
    bool request_reset;
    error_t request_errcode;

    bool initialized;
    bool configured;

    uint32_t uid[3];

    bool attached;
    uint32_t generation;
    bool rx_armed;
    bool tx_busy;
    bool tx_zlp;

    uint32_t rx_read_pointer;
    uint32_t rx_write_pointer;

    uint32_t tx_read_pointer;
    uint32_t tx_write_pointer;
    uint32_t tx_length;

    uint8_t rx_packet[USB_PACKET_SIZE] ALIGNED(4u);
    uint8_t tx_packet[USB_TX_TRANSFER_SIZE] ALIGNED(4u);
    uint8_t rx_buffer[USB_RX_BUFFER_SIZE];
    uint8_t tx_buffer[USB_TX_BUFFER_SIZE];

}usb_ctx_t;

error_t usb_init(usb_ctx_t *ctx);
error_t usb_configure(usb_ctx_t *ctx, const usb_config_t *config, const uint32_t uid[3]);
error_t usb_reset(usb_ctx_t *ctx);

void usb_loop_main(usb_ctx_t *ctx);

error_t usb_tx(usb_ctx_t *ctx, const void *buffer, uint32_t length);
error_t usb_rx(usb_ctx_t *ctx, void *buffer, uint32_t *p_length);
bool usb_is_connected(const usb_ctx_t *ctx);
uint32_t usb_get_generation(const usb_ctx_t *ctx);

#endif /* DRIVERS_USB_INC_USB_H_ */
