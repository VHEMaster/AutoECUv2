/*
 * config_etc.h
 *
 *  Created on: May 15, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_INC_CONFIG_ETC_H_
#define CONFIG_INC_CONFIG_ETC_H_

#include "config_modules.h"
#include "etc.h"

error_t ecu_modules_etc_init(ecu_module_etc_t instance, etc_ctx_t *ctx);
error_t ecu_modules_etc_get_default_config(ecu_module_etc_t instance, etc_config_t *config);
error_t ecu_modules_etc_configure(ecu_module_etc_t instance, const etc_config_t *config);
error_t ecu_modules_etc_reset(ecu_module_etc_t instance);

error_t ecu_modules_etc_set_enabled(ecu_module_etc_t instance, bool enabled);
error_t ecu_modules_etc_set_target_position(ecu_module_etc_t instance, float position);
error_t ecu_modules_etc_get_data(ecu_module_etc_t instance, etc_data_t *data);

#endif /* CONFIG_INC_CONFIG_ETC_H_ */
