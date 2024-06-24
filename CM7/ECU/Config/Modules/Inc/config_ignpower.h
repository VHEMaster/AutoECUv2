/*
 * config_ignpower.h
 *
 *  Created on: Jun 23, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_INC_CONFIG_IGNPOWER_H_
#define CONFIG_INC_CONFIG_IGNPOWER_H_

#include "config_modules.h"
#include "ignpower.h"

#define ECU_MODULES_IGNPOWER_CALLBACKS_MAX   16

error_t ecu_modules_ignpower_init(ecu_module_ignpower_t instance, ignpower_ctx_t *ctx);
error_t ecu_modules_ignpower_get_default_config(ecu_module_ignpower_t instance, ignpower_config_t *config);
error_t ecu_modules_ignpower_configure(ecu_module_ignpower_t instance, const ignpower_config_t *config);
error_t ecu_modules_ignpower_reset(ecu_module_ignpower_t instance);

error_t ecu_modules_ignpower_get_data(ecu_module_ignpower_t instance, ignpower_data_t *data);
error_t ecu_modules_ignpower_get_diag(ecu_module_ignpower_t instance, ignpower_diag_t *diag);

error_t ecu_modules_ignpower_is_active(ecu_module_ignpower_t instance, bool *ignpower_active);

error_t ecu_modules_ignpower_register_cb(ecu_module_ignpower_t instance, ignpower_signal_update_cb_t callback, void *usrdata);

#endif /* CONFIG_INC_CONFIG_IGNPOWER_H_ */
