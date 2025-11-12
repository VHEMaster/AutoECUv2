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

typedef enum {
  ECU_DEVICE_WBLS_READ_PARAM_LAMBDA_VALUE = 0,
  ECU_DEVICE_WBLS_READ_PARAM_OPERATING_STATUS,
  ECU_DEVICE_WBLS_READ_PARAM_MAX,
}ecu_device_wbls_read_params_t;

typedef enum {
  ECU_DEVICE_WBLS_WRITE_PARAM_HEATUP_TYPE = 0,
  ECU_DEVICE_WBLS_WRITE_PARAM_MAX,
}ecu_device_wbls_write_params_t;

error_t ecu_devices_wbls_init(ecu_device_wbls_t instance, cj125_ctx_t *ctx);
error_t ecu_devices_wbls_get_default_config(ecu_device_wbls_t instance, cj125_config_t *config);
error_t ecu_devices_wbls_configure(ecu_device_wbls_t instance, const cj125_config_t *config);
error_t ecu_devices_wbls_reset(ecu_device_wbls_t instance);

error_t ecu_devices_wbls_set_heatup(ecu_device_wbls_t instance, cj125_heatup_type_t type);
error_t ecu_devices_wbls_get_data(ecu_device_wbls_t instance, cj125_data_t *data);
error_t ecu_devices_wbls_get_diag(ecu_device_wbls_t instance, cj125_diag_t *diag);

#endif /* CONFIG_INC_CONFIG_WBLS_H_ */
