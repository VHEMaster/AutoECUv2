/*
 * config_kwp.h
 *
 *  Created on: Oct 23, 2025
 *      Author: vhemaster
 */

#ifndef CONFIG_COMMUNICATION_INC_CONFIG_KWP_H_
#define CONFIG_COMMUNICATION_INC_CONFIG_KWP_H_

#include "config_comm.h"
#include "kwp.h"

error_t ecu_comm_kwp_init(ecu_comm_kwp_t instance, kwp_ctx_t *ctx);
error_t ecu_comm_kwp_get_default_config(ecu_comm_kwp_t instance, kwp_config_t *config);
error_t ecu_comm_kwp_configure(ecu_comm_kwp_t instance, const kwp_config_t *config);
error_t ecu_comm_kwp_reset(ecu_comm_kwp_t instance);

#endif /* CONFIG_COMMUNICATION_INC_CONFIG_KWP_H_ */
