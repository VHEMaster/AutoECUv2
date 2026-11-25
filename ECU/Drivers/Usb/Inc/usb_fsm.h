/*
 * usb_fsm.h
 */

#ifndef DRIVERS_USB_INC_USB_FSM_H_
#define DRIVERS_USB_INC_USB_FSM_H_

typedef enum {
  USB_FSM_CONDITION = 0,
  USB_FSM_STOP,
  USB_FSM_DEINIT,
  USB_FSM_SET_IDENTITY,
  USB_FSM_INIT,
  USB_FSM_REGISTER_CLASS,
  USB_FSM_START,
}usb_fsm_t;

struct usb_ctx_tag;

void usb_fsm(struct usb_ctx_tag *ctx);

#endif /* DRIVERS_USB_INC_USB_FSM_H_ */
