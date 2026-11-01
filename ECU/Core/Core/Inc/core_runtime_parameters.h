/*
 * core_runtime_parameters.h
 *
 *  Created on: May 17, 2025
 *      Author: VHEMaster
 */

#ifndef CORE_CORE_INC_CORE_RUNTIME_PARAMETERS_H_
#define CORE_CORE_INC_CORE_RUNTIME_PARAMETERS_H_

#include "common.h"
#include "config_engine.h"

#define ECU_RUNTIME_PARAMETER_TRUE    (1.0f)
#define ECU_RUNTIME_PARAMETER_FALSE   (0.0f)

typedef struct {
    ecu_runtime_param_index_t count[ECU_COMMON_READ_WRITE_MAX];

    ecu_core_runtime_value_ctx_t *params[ECU_COMMON_READ_WRITE_MAX];
}ecu_core_runtime_global_instance_parameters_ctx_t;

typedef struct {
    ecu_core_runtime_global_instance_parameters_ctx_t sensors[ECU_SENSOR_TYPE_MAX][ECU_SENSOR_INSTANCE_MAX];
    ecu_core_runtime_global_instance_parameters_ctx_t devices[ECU_DEVICE_TYPE_MAX][ECU_DEVICE_INSTANCE_MAX];
    ecu_core_runtime_global_instance_parameters_ctx_t modules[ECU_MODULE_TYPE_MAX][ECU_MODULE_INSTANCE_MAX];
    ecu_core_runtime_global_instance_parameters_ctx_t timings[ECU_TIMING_TYPE_MAX][ECU_TIMING_INSTANCE_MAX];
}ecu_core_runtime_global_parameters_ctx_t;

/*
typedef struct {
    ecu_runtime_param_index_t count[ECU_COMMON_READ_WRITE_MAX];

    ecu_core_runtime_value_ctx_t *params[ECU_COMMON_READ_WRITE_MAX];
}ecu_core_runtime_global_instance_parameters_ctx_t;

typedef struct {
    ecu_config_common_entity_instance_t instances_count;
    ecu_core_runtime_global_instance_parameters_ctx_t *instances;
}ecu_core_runtime_global_type_parameters_ctx_t;

typedef struct {
    ecu_config_common_entity_type_t types_count;
    ecu_core_runtime_global_type_parameters_ctx_t *types;
}ecu_core_runtime_global_entity_parameters_ctx_t;

typedef struct {
    ecu_config_common_entity_t entities_count;
    ecu_core_runtime_global_entity_parameters_ctx_t *entities;


}ecu_core_runtime_global_parameters_ctx_t;
 */

#endif /* CORE_CORE_INC_CORE_RUNTIME_PARAMETERS_H_ */
