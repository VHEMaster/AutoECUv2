/*
 * config_doip.h
 *
 *  Created on: Nov 18, 2026
 *      Author: VHEMaster
 */

#ifndef CONFIG_COMMUNICATION_INC_CONFIG_DOIP_H_
#define CONFIG_COMMUNICATION_INC_CONFIG_DOIP_H_

#include "config_comm.h"
#include "versioned_doip.h"
#include "doip_types.h"
#ifdef CORE_CM4
#include "doip.h"
#endif /* CORE_CM4 */

#ifdef CORE_CM4
error_t ecu_comm_doip_init(ecu_comm_doip_t instance, doip_ctx_t *ctx);
#endif /* CORE_CM4 */
error_t ecu_comm_doip_get_default_config(ecu_comm_doip_t instance, doip_config_t *config);
error_t ecu_comm_doip_configure(ecu_comm_doip_t instance, const doip_config_t *config);
error_t ecu_comm_doip_reset(ecu_comm_doip_t instance);

#endif /* CONFIG_COMMUNICATION_INC_CONFIG_DOIP_H_ */
