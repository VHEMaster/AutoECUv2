/*
 * usb_fsm.c
 */

#include "usb.h"
#include "usb_device.h"
#include "usbd_desc.h"
#include "usbd_vendor.h"

void usb_fsm(usb_ctx_t *ctx)
{
  error_t err = E_OK;

  while(true) {
    switch(ctx->fsm) {
      case USB_FSM_CONDITION:
        if(ATOMIC_LOAD(ctx->request_init) == true && ATOMIC_LOAD(ctx->request_errcode) == E_AGAIN) {
          if(ctx->configured) {
            ctx->fsm = USB_FSM_STOP;
            continue;
          }
          if(!ctx->request_reset && ctx->config.enabled) {
            ctx->fsm = USB_FSM_SET_IDENTITY;
            continue;
          }

          ATOMIC_STORE(ctx->request_errcode, E_OK);
        }
        break;
      case USB_FSM_STOP:
        ctx->configured = false;
        USB_DEVICE_Stop();

        ctx->fsm = USB_FSM_DEINIT;
        continue;
      case USB_FSM_DEINIT:
        if(USB_DEVICE_DeInit() != USBD_OK) {
          err = E_FAULT;
        }
        if(err == E_OK && !ctx->request_reset && ctx->config.enabled) {
          ctx->fsm = USB_FSM_SET_IDENTITY;
          continue;
        }

        ctx->fsm = USB_FSM_CONDITION;
        ATOMIC_STORE(ctx->request_errcode, err);
        break;
      case USB_FSM_SET_IDENTITY:
        USBD_FS_SetIdentity(ctx->config.vid, ctx->config.pid, ctx->uid);

        ctx->fsm = USB_FSM_INIT;
        continue;
      case USB_FSM_INIT:
        if(USB_DEVICE_Init() == USBD_OK) {
          ctx->fsm = USB_FSM_REGISTER_CLASS;
        } else {
          err = E_FAULT;
          ctx->fsm = USB_FSM_DEINIT;
        }
        continue;
      case USB_FSM_REGISTER_CLASS:
        if(USBD_RegisterClass(&hUsbDeviceFS, &USBD_VENDOR) == USBD_OK) {
          ctx->fsm = USB_FSM_START;
        } else {
          err = E_FAULT;
          ctx->fsm = USB_FSM_DEINIT;
        }
        continue;
      case USB_FSM_START:
        if(USB_DEVICE_Start() != USBD_OK) {
          err = E_FAULT;
          ctx->fsm = USB_FSM_STOP;
          continue;
        }

        ctx->configured = true;
        ctx->fsm = USB_FSM_CONDITION;
        ATOMIC_STORE(ctx->request_errcode, E_OK);
        break;
      default:
        ctx->fsm = USB_FSM_CONDITION;
        ATOMIC_STORE(ctx->request_errcode, E_FAULT);
        break;
    }
    break;
  }
}
