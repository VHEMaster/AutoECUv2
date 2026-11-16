/*
 * config_rs422.h
 *
 *  Created on: Nov 16, 2025
 *      Author: VHEMaster
 */

#ifndef CONFIG_INC_CONFIG_RS422_H_
#define CONFIG_INC_CONFIG_RS422_H_

#include "config_comm.h"
#include "versioned_rs422.h"
#include "rs422_types.h"
#ifdef CORE_CM4
#include "rs422.h"
#endif /* CORE_CM4 */

#ifdef CORE_CM4
error_t ecu_comm_rs422_init(ecu_comm_rs422_t instance, rs422_ctx_t *ctx);
#endif /* CORE_CM4 */
error_t ecu_comm_rs422_get_default_config(ecu_comm_rs422_t instance, rs422_config_t *config);
error_t ecu_comm_rs422_configure(ecu_comm_rs422_t instance, const rs422_config_t *config);
error_t ecu_comm_rs422_reset(ecu_comm_rs422_t instance);

error_t ecu_comm_rs422_transmit(ecu_comm_rs422_t instance, const void *buffer, uint32_t length);
error_t ecu_comm_rs422_receive(ecu_comm_rs422_t instance, void *buffer, uint32_t *p_length);

#endif /* CONFIG_INC_CONFIG_RS422_H_ */
