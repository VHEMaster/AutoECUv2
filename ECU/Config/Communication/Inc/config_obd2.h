/*
 * config_obd2.h
 *
 *  Created on: Oct 23, 2025
 *      Author: vhemaster
 */

#ifndef CONFIG_COMMUNICATION_INC_CONFIG_OBD2_H_
#define CONFIG_COMMUNICATION_INC_CONFIG_OBD2_H_

#include "config_comm.h"
#include "versioned_obd2.h"
#ifdef CORE_CM4
#include "obd2.h"
#endif /* CORE_CM4 */

#ifdef CORE_CM4
error_t ecu_comm_obd2_init(ecu_comm_obd2_t instance, obd2_ctx_t *ctx);
#endif /* CORE_CM4 */
error_t ecu_comm_obd2_get_default_config(ecu_comm_obd2_t instance, obd2_config_t *config);
error_t ecu_comm_obd2_configure(ecu_comm_obd2_t instance, const obd2_config_t *config);
error_t ecu_comm_obd2_reset(ecu_comm_obd2_t instance);

#endif /* CONFIG_COMMUNICATION_INC_CONFIG_OBD2_H_ */
