/*
 * config_tcs.h
 *
 *  Created on: Apr 16, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_INC_CONFIG_TCS_H_
#define CONFIG_INC_CONFIG_TCS_H_

#include "config_devices.h"
#include "max31855.h"

error_t ecu_devices_tcs_init(ecu_device_tcs_t instance, max31855_ctx_t *ctx);

#endif /* CONFIG_INC_CONFIG_TCS_H_ */
