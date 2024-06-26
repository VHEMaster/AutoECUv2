/*
 * config_wgcv.h
 *
 *  Created on: Jun 9, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_INC_CONFIG_WGCV_H_
#define CONFIG_INC_CONFIG_WGCV_H_

#include "config_modules.h"
#include "wgcv.h"

error_t ecu_modules_wgcv_init(ecu_module_wgcv_t instance, wgcv_ctx_t *ctx);
error_t ecu_modules_wgcv_get_default_config(ecu_module_wgcv_t instance, wgcv_config_t *config);
error_t ecu_modules_wgcv_configure(ecu_module_wgcv_t instance, const wgcv_config_t *config);
error_t ecu_modules_wgcv_reset(ecu_module_wgcv_t instance);

error_t ecu_modules_wgcv_set_enabled(ecu_module_wgcv_t instance, bool enabled);
error_t ecu_modules_wgcv_set_dutycycle(ecu_module_wgcv_t instance, float dutycycle);
error_t ecu_modules_wgcv_get_data(ecu_module_wgcv_t instance, wgcv_data_t *data);
error_t ecu_modules_wgcv_get_diag(ecu_module_wgcv_t instance, wgcv_diag_t *diag);

#endif /* CONFIG_INC_CONFIG_WGCV_H_ */
