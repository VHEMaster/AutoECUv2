/*
 * config_vvt.h
 *
 *  Created on: Jun 9, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_INC_CONFIG_VVT_H_
#define CONFIG_INC_CONFIG_VVT_H_

#include "config_modules.h"
#include "vvt.h"

error_t ecu_modules_vvt_init(ecu_module_vvt_t instance, vvt_ctx_t *ctx);
error_t ecu_modules_vvt_get_default_config(ecu_module_vvt_t instance, vvt_config_t *config);
error_t ecu_modules_vvt_configure(ecu_module_vvt_t instance, const vvt_config_t *config);
error_t ecu_modules_vvt_reset(ecu_module_vvt_t instance);

error_t ecu_modules_vvt_set_enabled(ecu_module_vvt_t instance, bool enabled);
error_t ecu_modules_vvt_set_target_position(ecu_module_vvt_t instance, float position);
error_t ecu_modules_vvt_set_target_dutycycle(ecu_module_vvt_t instance, float dutycycle);
error_t ecu_modules_vvt_get_data(ecu_module_vvt_t instance, vvt_data_t *data);
error_t ecu_modules_vvt_get_diag(ecu_module_vvt_t instance, vvt_diag_t *diag);

#endif /* CONFIG_INC_CONFIG_VVT_H_ */
