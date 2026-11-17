/*
 * config_lwipdrv.h
 *
 *  Created on: 17 нояб. 2026 г.
 *      Author: VHEMaster
 */

#ifndef CONFIG_COMMUNICATION_INC_CONFIG_LWIPDRV_H_
#define CONFIG_COMMUNICATION_INC_CONFIG_LWIPDRV_H_

#include "config_comm.h"
#include "versioned_lwipdrv.h"
#ifdef CORE_CM4
#include "lwipdrv.h"
#endif /* CORE_CM4 */

#ifdef CORE_CM4
error_t ecu_comm_lwipdrv_init(ecu_comm_lwipdrv_t instance, lwipdrv_ctx_t *ctx);
#endif /* CORE_CM4 */
error_t ecu_comm_lwipdrv_get_default_config(ecu_comm_lwipdrv_t instance, lwipdrv_config_t *config);
error_t ecu_comm_lwipdrv_configure(ecu_comm_lwipdrv_t instance, const lwipdrv_config_t *config);
error_t ecu_comm_lwipdrv_reset(ecu_comm_lwipdrv_t instance);

#endif /* CONFIG_COMMUNICATION_INC_CONFIG_LWIPDRV_H_ */
