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

typedef enum {
  ECU_DEVICE_FLASH_READ_PARAM_MAX
}ecu_device_flash_read_params_t;

typedef enum {
  ECU_DEVICE_FLASH_WRITE_PARAM_MAX
}ecu_device_flash_write_params_t;

error_t ecu_devices_flash_init(ecu_device_flash_t instance, qspi_ctx_t *ctx);
error_t ecu_devices_flash_reset(ecu_device_flash_t instance);

#endif /* CONFIG_INC_CONFIG_FLASH_H_ */
