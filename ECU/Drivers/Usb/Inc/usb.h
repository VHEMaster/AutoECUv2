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

typedef struct usb_ctx_tag {
    usb_config_t config;
    usb_fsm_t fsm;
    bool request_init;
    bool request_reset;
    error_t request_errcode;

    bool initialized;
    bool configured;

    uint32_t uid[3];
}usb_ctx_t;

error_t usb_init(usb_ctx_t *ctx);
error_t usb_configure(usb_ctx_t *ctx, const usb_config_t *config, const uint32_t uid[3]);
error_t usb_reset(usb_ctx_t *ctx);

void usb_loop_main(usb_ctx_t *ctx);

#endif /* DRIVERS_USB_INC_USB_H_ */
