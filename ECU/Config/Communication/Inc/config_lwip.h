/*
 * config_lwip.h
 *
 *  Created on: 17 нояб. 2026 г.
 *      Author: VHEMaster
 */

#ifndef CONFIG_COMMUNICATION_INC_CONFIG_LWIP_H_
#define CONFIG_COMMUNICATION_INC_CONFIG_LWIP_H_

#include "config_comm.h"
#include "versioned_lwip.h"
#ifdef CORE_CM4
#include "lwip.h"
#endif /* CORE_CM4 */

#ifdef CORE_CM4
error_t ecu_comm_lwip_init(ecu_comm_lwip_t instance, lwip_ctx_t *ctx);
#endif /* CORE_CM4 */
error_t ecu_comm_lwip_get_default_config(ecu_comm_lwip_t instance, lwip_config_t *config);
error_t ecu_comm_lwip_configure(ecu_comm_lwip_t instance, const lwip_config_t *config);
error_t ecu_comm_lwip_reset(ecu_comm_lwip_t instance);

#endif /* CONFIG_COMMUNICATION_INC_CONFIG_LWIP_H_ */
