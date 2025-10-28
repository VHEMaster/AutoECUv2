/*
 * config_can.h
 *
 *  Created on: Oct 19, 2025
 *      Author: VHEMaster
 */

#ifndef CONFIG_INC_CONFIG_CAN_H_
#define CONFIG_INC_CONFIG_CAN_H_

#include "config_comm.h"
#include "can.h"

typedef void (*ecu_comm_can_rx_callback_t)(ecu_comm_can_t instance, const can_message_t *message, void *usrdata);
typedef void (*ecu_comm_can_err_callback_t)(ecu_comm_can_t instance, void *usrdata);

error_t ecu_comm_can_init(ecu_comm_can_t instance, can_ctx_t *ctx);
error_t ecu_comm_can_get_default_config(ecu_comm_can_t instance, can_config_t *config);
error_t ecu_comm_can_configure(ecu_comm_can_t instance, const can_config_t *config);
error_t ecu_comm_can_reset(ecu_comm_can_t instance);

error_t ecu_comm_can_transmit(ecu_comm_can_t instance, const can_message_t *message);
error_t ecu_comm_can_receive(ecu_comm_can_t instance, can_message_t *message);

error_t ecu_comm_can_register_rx_callback(ecu_comm_can_t instance, uint32_t msg_id, can_rx_callback_func_t func, void *usrdata);
error_t ecu_comm_can_register_err_callback(ecu_comm_can_t instance, can_err_callback_func_t func, void *usrdata);

#endif /* CONFIG_INC_CONFIG_CAN_H_ */
