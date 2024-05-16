/*
 * config_flash.h
 *
 *  Created on: Apr 16, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_INC_CONFIG_FLASH_H_
#define CONFIG_INC_CONFIG_FLASH_H_

#include "config_devices.h"
#include "qspi.h"

error_t ecu_devices_flash_init(ecu_device_flash_t instance, qspi_ctx_t *ctx);
error_t ecu_devices_flash_reset(ecu_device_flash_t instance);

#endif /* CONFIG_INC_CONFIG_FLASH_H_ */
