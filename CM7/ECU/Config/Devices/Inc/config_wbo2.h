/*
 * config_wbo2.h
 *
 *  Created on: Apr 19, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_INC_CONFIG_WBO2_H_
#define CONFIG_INC_CONFIG_WBO2_H_

#include "config_devices.h"
#include "cj125.h"

typedef enum {
  ECU_DEVICE_WBO2_READ_PARAM_LAMBDA_VALUE = 0,
  ECU_DEVICE_WBO2_READ_PARAM_OPERATING_STATUS,
  ECU_DEVICE_WBO2_READ_PARAM_MAX
}ecu_device_wbo2_read_params_t;

typedef enum {
  ECU_DEVICE_WBO2_WRITE_PARAM_HEATUP_TYPE = 0,
  ECU_DEVICE_WBO2_WRITE_PARAM_MAX
}ecu_device_wbo2_write_params_t;

error_t ecu_devices_wbo2_init(ecu_device_wbo2_t instance, cj125_ctx_t *ctx);
error_t ecu_devices_wbo2_get_default_config(ecu_device_wbo2_t instance, cj125_config_t *config);
error_t ecu_devices_wbo2_configure(ecu_device_wbo2_t instance, const cj125_config_t *config);
error_t ecu_devices_wbo2_reset(ecu_device_wbo2_t instance);

error_t ecu_devices_wbo2_set_heatup(ecu_device_wbo2_t instance, cj125_heatup_type_t type);
error_t ecu_devices_wbo2_get_data(ecu_device_wbo2_t instance, cj125_data_t *data);
error_t ecu_devices_wbo2_get_diag(ecu_device_wbo2_t instance, cj125_diag_t *diag);

#endif /* CONFIG_INC_CONFIG_WBO2_H_ */
