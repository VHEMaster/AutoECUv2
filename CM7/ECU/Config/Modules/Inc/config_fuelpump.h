/*
 * config_fuelpump.h
 *
 *  Created on: Jun 9, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_INC_CONFIG_FUELPUMP_H_
#define CONFIG_INC_CONFIG_FUELPUMP_H_

#include "config_modules.h"
#include "fuelpump.h"

error_t ecu_modules_fuelpump_init(ecu_module_fuelpump_t instance, fuelpump_ctx_t *ctx);
error_t ecu_modules_fuelpump_get_default_config(ecu_module_fuelpump_t instance, fuelpump_config_t *config);
error_t ecu_modules_fuelpump_configure(ecu_module_fuelpump_t instance, const fuelpump_config_t *config);
error_t ecu_modules_fuelpump_reset(ecu_module_fuelpump_t instance);

error_t ecu_modules_fuelpump_manual_set(ecu_module_fuelpump_t instance, bool manual_enabled);
error_t ecu_modules_fuelpump_force_reset(ecu_module_fuelpump_t instance);
error_t ecu_modules_fuelpump_force_set(ecu_module_fuelpump_t instance, bool force_enabled);

error_t ecu_modules_fuelpump_get_data(ecu_module_fuelpump_t instance, fuelpump_data_t *data);
error_t ecu_modules_fuelpump_get_diag(ecu_module_fuelpump_t instance, fuelpump_diag_t *diag);

#endif /* CONFIG_INC_CONFIG_FUELPUMP_H_ */
