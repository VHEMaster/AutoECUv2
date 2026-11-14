/*
 * config_common_private.h
 *
 *  Created on: 14 нояб. 2026 г.
 *      Author: VHEMaster
 */

#ifndef CONFIG_COMMON_INC_CONFIG_COMMON_PRIVATE_H_
#define CONFIG_COMMON_INC_CONFIG_COMMON_PRIVATE_H_

#include "common.h"

error_t ecu_config_common_mdma_init(void);
error_t ecu_config_common_mdma_copy(void *dst, const void *src, uint32_t bytes);
error_t ecu_config_common_mdma_poll(void);

#endif /* CONFIG_COMMON_INC_CONFIG_COMMON_PRIVATE_H_ */
