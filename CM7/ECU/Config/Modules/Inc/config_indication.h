/*
 * config_indication.h
 *
 *  Created on: Jun 9, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_INC_CONFIG_INDICATION_H_
#define CONFIG_INC_CONFIG_INDICATION_H_

#include "config_modules.h"
#include "indication.h"

error_t ecu_modules_indication_init(ecu_module_indication_t instance, indication_ctx_t *ctx);
error_t ecu_modules_indication_get_default_config(ecu_module_indication_t instance, indication_config_t *config);
error_t ecu_modules_indication_configure(ecu_module_indication_t instance, const indication_config_t *config);
error_t ecu_modules_indication_reset(ecu_module_indication_t instance);

error_t ecu_modules_indication_manual_reset(ecu_module_indication_t instance);
error_t ecu_modules_indication_manual_set(ecu_module_indication_t instance, bool manual_enabled);
error_t ecu_modules_indication_force_reset(ecu_module_indication_t instance);
error_t ecu_modules_indication_force_set(ecu_module_indication_t instance, bool force_enabled);

error_t ecu_modules_indication_get_data(ecu_module_indication_t instance, indication_data_t *data);
error_t ecu_modules_indication_get_diag(ecu_module_indication_t instance, indication_diag_t *diag);

#endif /* CONFIG_INC_CONFIG_INDICATION_H_ */
