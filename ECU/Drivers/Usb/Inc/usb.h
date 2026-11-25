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

typedef struct {
    uint8_t *rx_packet;
    uint8_t *tx_packet;
    uint8_t *rx_buffer;
    uint8_t *tx_buffer;
    uint32_t rx_packet_size;
    uint32_t tx_packet_size;
    uint32_t rx_buffer_size;
    uint32_t tx_buffer_size;

    uint16_t vid;
    uint16_t pid;
}usb_init_ctx_t;

typedef struct usb_ctx_tag {
    usb_init_ctx_t init;
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
    uint32_t ring_generation;
    bool rx_armed;
    bool tx_busy;
    bool tx_zlp;

    uint32_t rx_read_pointer;
    uint32_t rx_write_pointer;

    uint32_t tx_read_pointer;
    uint32_t tx_write_pointer;
    uint32_t tx_length;

}usb_ctx_t;

error_t usb_init(usb_ctx_t *ctx, const usb_init_ctx_t *init_ctx);
error_t usb_configure(usb_ctx_t *ctx, const usb_config_t *config, const uint32_t uid[3]);
error_t usb_reset(usb_ctx_t *ctx);

void usb_loop_main(usb_ctx_t *ctx);

error_t usb_tx(usb_ctx_t *ctx, const void *buffer, uint32_t length);
error_t usb_rx(usb_ctx_t *ctx, void *buffer, uint32_t *p_length);

/* Release a borrowed span in the same main-loop iteration, before usb_loop_main(). */
error_t usb_rx_nocopy(usb_ctx_t *ctx, const void **p_buffer, uint32_t *p_length);
error_t usb_rx_release(usb_ctx_t *ctx, uint32_t length);

bool usb_is_connected(const usb_ctx_t *ctx);
uint32_t usb_get_generation(const usb_ctx_t *ctx);

#endif /* DRIVERS_USB_INC_USB_H_ */
