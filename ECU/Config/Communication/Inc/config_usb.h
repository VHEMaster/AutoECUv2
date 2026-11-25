/*
 * config_usb.h
 *
 *  Created on: Oct 23, 2025
 *      Author: vhemaster
 */

#ifndef CONFIG_COMMUNICATION_INC_CONFIG_USB_H_
#define CONFIG_COMMUNICATION_INC_CONFIG_USB_H_

#include "config_comm.h"
#include "versioned_usb.h"
#ifdef CORE_CM4
#include "usb.h"
#endif /* CORE_CM4 */

#ifdef CORE_CM4
error_t ecu_comm_usb_init(ecu_comm_usb_t instance, usb_ctx_t *ctx);
#endif /* CORE_CM4 */
error_t ecu_comm_usb_get_default_config(ecu_comm_usb_t instance, usb_config_t *config);
error_t ecu_comm_usb_configure(ecu_comm_usb_t instance, const usb_config_t *config);
error_t ecu_comm_usb_reset(ecu_comm_usb_t instance);

#endif /* CONFIG_COMMUNICATION_INC_CONFIG_USB_H_ */
