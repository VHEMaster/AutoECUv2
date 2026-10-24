#ifndef CM4_CONFIG_COMMON_H
#define CM4_CONFIG_COMMON_H
#include "common.h"
#include "config_common_types.h"
error_t ecu_config_common_get_parameter_ptr_by_id(ecu_config_parameter_id_t id, ecu_core_runtime_value_ctx_t **parameter);
error_t ecu_config_common_get_parameter_value_by_id(ecu_config_parameter_id_t id, ecu_core_runtime_value_ctx_t *parameter);
#endif
