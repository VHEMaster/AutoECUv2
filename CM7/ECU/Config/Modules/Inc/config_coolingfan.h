/*
 * config_coolingfan.h
 *
 *  Created on: Jun 9, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_INC_CONFIG_COOLINGFAN_H_
#define CONFIG_INC_CONFIG_COOLINGFAN_H_

#include "config_modules.h"
#include "coolingfan.h"

error_t ecu_modules_coolingfan_init(ecu_module_coolingfan_t instance, coolingfan_ctx_t *ctx);
error_t ecu_modules_coolingfan_get_default_config(ecu_module_coolingfan_t instance, coolingfan_config_t *config);
error_t ecu_modules_coolingfan_configure(ecu_module_coolingfan_t instance, const coolingfan_config_t *config);
error_t ecu_modules_coolingfan_reset(ecu_module_coolingfan_t instance);

error_t ecu_modules_coolingfan_activate_trigger(ecu_module_coolingfan_t instance, bool activate_trigger);
error_t ecu_modules_coolingfan_emergency_trigger(ecu_module_coolingfan_t instance, bool emergency_trigger);
error_t ecu_modules_coolingfan_force_reset(ecu_module_coolingfan_t instance);
error_t ecu_modules_coolingfan_force_set(ecu_module_coolingfan_t instance, bool force_enabled);

error_t ecu_modules_coolingfan_get_data(ecu_module_coolingfan_t instance, coolingfan_data_t *data);
error_t ecu_modules_coolingfan_get_diag(ecu_module_coolingfan_t instance, coolingfan_diag_t *diag);

#endif /* CONFIG_INC_CONFIG_COOLINGFAN_H_ */
