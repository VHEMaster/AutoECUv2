/*
 * config_tunerstudio.h
 *
 *  Created on: Nov 26, 2026
 *      Author: VHEMaster
 */

#ifndef CONFIG_COMMUNICATION_INC_CONFIG_TUNERSTUDIO_H_
#define CONFIG_COMMUNICATION_INC_CONFIG_TUNERSTUDIO_H_

#include "config_comm.h"
#include "versioned_tunerstudio.h"
#include "tunerstudio_types.h"
#ifdef CORE_CM4
#include "tunerstudio.h"
#endif /* CORE_CM4 */

#ifdef CORE_CM4
error_t ecu_comm_tunerstudio_init(ecu_comm_tunerstudio_t instance, tunerstudio_ctx_t *ctx);
#endif /* CORE_CM4 */
error_t ecu_comm_tunerstudio_get_default_config(ecu_comm_tunerstudio_t instance, tunerstudio_config_t *config);
error_t ecu_comm_tunerstudio_configure(ecu_comm_tunerstudio_t instance, const tunerstudio_config_t *config);
error_t ecu_comm_tunerstudio_reset(ecu_comm_tunerstudio_t instance);

#endif /* CONFIG_COMMUNICATION_INC_CONFIG_TUNERSTUDIO_H_ */
