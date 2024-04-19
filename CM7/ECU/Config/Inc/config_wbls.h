/*
 * config_wbls.h
 *
 *  Created on: Apr 19, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_INC_CONFIG_WBLS_H_
#define CONFIG_INC_CONFIG_WBLS_H_

#include "config_devices.h"
#include "cj125.h"

error_t ecu_devices_wbls_init(ecu_device_wbls_t instance, cj125_ctx_t *ctx);

#endif /* CONFIG_INC_CONFIG_WBLS_H_ */
