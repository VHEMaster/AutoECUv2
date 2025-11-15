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

typedef enum {
  ECU_DEVICE_TCS_READ_PARAM_TEMPERATURE = 0,
  ECU_DEVICE_TCS_READ_PARAM_MAX
}ecu_device_read_read_params_t;

typedef enum {
  ECU_DEVICE_TCS_WRITE_PARAM_MAX
}ecu_device_tcs_write_params_t;

error_t ecu_devices_tcs_init(ecu_device_tcs_t instance, max31855_ctx_t *ctx);
error_t ecu_devices_tcs_get_data(ecu_device_tcs_t instance, max31855_data_t *data);
error_t ecu_devices_tcs_get_diag(ecu_device_tcs_t instance, max31855_diag_t *diag);

#endif /* CONFIG_INC_CONFIG_TCS_H_ */
