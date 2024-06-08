/*
 * config_cylinders.h
 *
 *  Created on: Jun 6, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_INC_CONFIG_CYLINDERS_H_
#define CONFIG_INC_CONFIG_CYLINDERS_H_

#include "config_core.h"
#include "cylinders.h"

error_t ecu_core_component_cylinders_init(ecu_core_component_cylinders_t instance, cylinders_ctx_t *ctx);
error_t ecu_core_component_cylinders_get_default_config(ecu_core_component_cylinders_t instance, ecu_cylinders_config_t *config);
error_t ecu_core_component_cylinders_configure(ecu_core_component_cylinders_t instance, const ecu_cylinders_config_t *config);
error_t ecu_core_component_cylinders_reset(ecu_core_component_cylinders_t instance);

error_t ecu_core_component_cylinders_get_data(ecu_core_component_cylinders_t instance, cylinders_data_t *data);
error_t ecu_core_component_cylinders_get_diag(ecu_core_component_cylinders_t instance, cylinders_diag_t *diag);

#endif /* CONFIG_INC_CONFIG_CYLINDERS_H_ */
