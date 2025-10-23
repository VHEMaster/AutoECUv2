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

error_t ecu_comm_can_init(ecu_comm_can_t instance, can_ctx_t *ctx);
error_t ecu_comm_can_get_default_config(ecu_comm_can_t instance, can_cfg_t *config);
error_t ecu_comm_can_configure(ecu_comm_can_t instance, const can_cfg_t *config);

#endif /* CONFIG_INC_CONFIG_CAN_H_ */
