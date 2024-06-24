/*
 * config_ignition.h
 *
 *  Created on: Jun 23, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_INC_CONFIG_IGNITION_H_
#define CONFIG_INC_CONFIG_IGNITION_H_

#include "config_modules.h"
#include "ignition.h"

#define ECU_MODULES_IGNITION_CALLBACKS_MAX   16

error_t ecu_modules_ignition_init(ecu_module_ignition_t instance, ignition_ctx_t *ctx);
error_t ecu_modules_ignition_get_default_config(ecu_module_ignition_t instance, ignition_config_t *config);
error_t ecu_modules_ignition_configure(ecu_module_ignition_t instance, const ignition_config_t *config);
error_t ecu_modules_ignition_reset(ecu_module_ignition_t instance);

error_t ecu_modules_ignition_get_data(ecu_module_ignition_t instance, ignition_data_t *data);
error_t ecu_modules_ignition_get_diag(ecu_module_ignition_t instance, ignition_diag_t *diag);

error_t ecu_modules_ignition_is_active(ecu_module_ignition_t instance, bool *ignition_active);

error_t ecu_modules_ignition_register_cb(ecu_module_ignition_t instance, ignition_signal_update_cb_t callback, void *usrdata);

#endif /* CONFIG_INC_CONFIG_IGNITION_H_ */
