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

#endif /* CONFIG_COMMUNICATION_INC_CONFIG_ISOTP_H_ */
