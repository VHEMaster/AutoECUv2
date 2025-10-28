/*
 * config_isotp.h
 *
 *  Created on: Oct 23, 2025
 *      Author: vhemaster
 */

#ifndef CONFIG_COMMUNICATION_INC_CONFIG_ISOTP_H_
#define CONFIG_COMMUNICATION_INC_CONFIG_ISOTP_H_

#include "config_comm.h"
#include "isotp.h"

error_t ecu_comm_isotp_init(ecu_comm_isotp_t instance, isotp_ctx_t *ctx);
error_t ecu_comm_isotp_get_default_config(ecu_comm_isotp_t instance, isotp_config_t *config);
error_t ecu_comm_isotp_configure(ecu_comm_isotp_t instance, const isotp_config_t *config);
error_t ecu_comm_isotp_reset(ecu_comm_isotp_t instance);

error_t ecu_comm_isotp_frame_write_upstream(ecu_comm_isotp_t instance, const isotp_frame_t *frame);
error_t ecu_comm_isotp_frame_read_downstream(ecu_comm_isotp_t instance, isotp_frame_t *frame);

error_t ecu_comm_isotp_data_write_downstream(ecu_comm_isotp_t instance, const uint8_t *payload, uint16_t length);
error_t ecu_comm_isotp_data_read_upstream(ecu_comm_isotp_t instance, uint8_t *payload, uint16_t *length);
error_t ecu_comm_isotp_get_error_flag(ecu_comm_isotp_t instance, isotp_error_code_t *code);

#endif /* CONFIG_COMMUNICATION_INC_CONFIG_ISOTP_H_ */
