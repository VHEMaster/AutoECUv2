/*
 * config_egt.h
 *
 *  Created on: Apr 16, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_INC_CONFIG_EGT_H_
#define CONFIG_INC_CONFIG_EGT_H_

#include "config_devices.h"
#include "max31855.h"

error_t ecu_devices_egt_init(ecu_device_egt_t instance, max31855_ctx_t *ctx);

#endif /* CONFIG_INC_CONFIG_EGT_H_ */
