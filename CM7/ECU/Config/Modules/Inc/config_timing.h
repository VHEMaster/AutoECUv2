/*
 * config_timing.h
 *
 *  Created on: Jun 6, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_INC_CONFIG_TIMING_H_
#define CONFIG_INC_CONFIG_TIMING_H_

#include "config_modules.h"
#include "timing.h"

#define ECU_MODULES_TIMING_CALLBACKS_MAX   4

error_t ecu_modules_timing_init(ecu_module_timing_t instance, timing_ctx_t *ctx);
error_t ecu_modules_timing_get_default_config(ecu_module_timing_t instance, timing_config_t *config);
error_t ecu_modules_timing_configure(ecu_module_timing_t instance, const timing_config_t *config);
error_t ecu_modules_timing_reset(ecu_module_timing_t instance);

error_t ecu_modules_timing_register_cb(ecu_module_timing_t instance, timing_signal_update_cb_t callback, void *usrdata);

error_t ecu_modules_timing_get_data(ecu_module_timing_t instance, timing_data_t *data);
error_t ecu_modules_timing_get_diag(ecu_module_timing_t instance, timing_diag_t *diag);

#endif /* CONFIG_INC_CONFIG_TIMING_H_ */
