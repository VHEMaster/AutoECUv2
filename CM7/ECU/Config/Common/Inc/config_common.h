/*
 * config_common.h
 *
 *  Created on: Nov 9, 2025
 *      Author: VHEMaster
 */

#ifndef CONFIG_COMMON_INC_CONFIG_COMMON_H_
#define CONFIG_COMMON_INC_CONFIG_COMMON_H_

#include "common.h"
#include "core.h"

#include "config_common_types.h"

error_t ecu_config_common_init(ecu_core_ctx_t *ctx);

error_t ecu_config_common_get_entity_max(ecu_config_common_entity_t *entity_max);
error_t ecu_config_common_get_entity_type_max(ecu_config_common_entity_t entity, ecu_config_common_entity_type_t *type_max);
error_t ecu_config_common_get_entity_type_instance_max(ecu_config_common_entity_t entity, ecu_config_common_entity_type_t type, ecu_config_common_entity_instance_t *instance_max);
error_t ecu_config_common_get_entity_type_params_count(ecu_config_common_entity_t entity, ecu_config_common_entity_type_t type, ecu_config_common_read_write_t read_write, ecu_runtime_param_index_t *params_max);
error_t ecu_config_common_get_entity_type_instance_params(ecu_config_common_entity_t entity, ecu_config_common_entity_type_t type, ecu_config_common_entity_instance_t instance, ecu_config_common_read_write_t read_write, ecu_core_runtime_value_ctx_t **params, ecu_runtime_param_index_t *params_max);

error_t ecu_config_common_get_parameter_ptr_by_id(ecu_config_parameter_id_t id, const ecu_core_runtime_value_ctx_t **parameter);
error_t ecu_config_common_get_parameter_value_by_id(ecu_config_parameter_id_t id, ecu_core_runtime_value_ctx_t *parameter);

#endif /* CONFIG_COMMON_INC_CONFIG_COMMON_H_ */
