/*
 * config_uds.h
 *
 *  Created on: Oct 23, 2025
 *      Author: vhemaster
 */

#ifndef CONFIG_COMMUNICATION_INC_CONFIG_UDS_H_
#define CONFIG_COMMUNICATION_INC_CONFIG_UDS_H_

#include "config_comm.h"
#include "versioned_uds.h"
#ifdef CORE_CM4
#include "uds.h"
#endif /* CORE_CM4 */

#ifdef CORE_CM4
error_t ecu_comm_uds_init(ecu_comm_uds_t instance, uds_ctx_t *ctx);
#endif /* CORE_CM4 */
error_t ecu_comm_uds_get_default_config(ecu_comm_uds_t instance, uds_config_t *config);
error_t ecu_comm_uds_configure(ecu_comm_uds_t instance, const uds_config_t *config);
error_t ecu_comm_uds_reset(ecu_comm_uds_t instance);

#endif /* CONFIG_COMMUNICATION_INC_CONFIG_UDS_H_ */
