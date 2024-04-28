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
error_t ecu_devices_wbls_reset(ecu_device_wbls_t instance);
error_t ecu_devices_wbls_set_heatup(ecu_device_wbls_t instance, cj125_heatup_type_t type);
error_t ecu_devices_wbls_get_data(ecu_device_wbls_t instance, cj125_data_t *data);
error_t ecu_devices_wbls_get_diag(ecu_device_wbls_t instance, cj125_diag_t *diag);

#endif /* CONFIG_INC_CONFIG_WBLS_H_ */
