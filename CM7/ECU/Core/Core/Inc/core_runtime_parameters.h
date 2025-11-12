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

#define ECU_RUNTIME_SENSORS_READ_PARAMETERS_MAX   (1)

#define ECU_RUNTIME_DEVICES_READ_PARAMETERS_MAX   (8)
#define ECU_RUNTIME_DEVICES_WRITE_PARAMETERS_MAX  (8)

#define ECU_RUNTIME_MODULES_READ_PARAMETERS_MAX   (8)
#define ECU_RUNTIME_MODULES_WRITE_PARAMETERS_MAX  (8)

#define ECU_RUNTIME_TIMINGS_READ_PARAMETERS_MAX   (32)
#define ECU_RUNTIME_TIMINGS_WRITE_PARAMETERS_MAX  (8)

typedef uint8_t ecu_runtime_param_index_t;

typedef struct {
    ecu_core_runtime_value_ctx_t read[ECU_RUNTIME_SENSORS_READ_PARAMETERS_MAX];
}ecu_core_runtime_global_parameters_sensor_ctx_t;

typedef struct {
    ecu_core_runtime_value_ctx_t read[ECU_RUNTIME_DEVICES_READ_PARAMETERS_MAX];
    ecu_core_runtime_value_ctx_t write[ECU_RUNTIME_DEVICES_WRITE_PARAMETERS_MAX];
}ecu_core_runtime_global_parameters_device_ctx_t;

typedef struct {
    ecu_core_runtime_value_ctx_t read[ECU_RUNTIME_MODULES_READ_PARAMETERS_MAX];
    ecu_core_runtime_value_ctx_t write[ECU_RUNTIME_MODULES_WRITE_PARAMETERS_MAX];
}ecu_core_runtime_global_parameters_module_ctx_t;

typedef struct {
    ecu_core_runtime_value_ctx_t read[ECU_RUNTIME_TIMINGS_READ_PARAMETERS_MAX];
    ecu_core_runtime_value_ctx_t write[ECU_RUNTIME_TIMINGS_WRITE_PARAMETERS_MAX];
}ecu_core_runtime_global_parameters_timing_ctx_t;


typedef struct {
    ecu_core_runtime_global_parameters_sensor_ctx_t sensors[ECU_SENSOR_TYPE_MAX][ECU_SENSOR_INSTANCE_MAX];
    ecu_core_runtime_global_parameters_device_ctx_t devices[ECU_DEVICE_TYPE_MAX][ECU_DEVICE_INSTANCE_MAX];
    ecu_core_runtime_global_parameters_module_ctx_t modules[ECU_MODULE_TYPE_MAX][ECU_MODULE_INSTANCE_MAX];
    ecu_core_runtime_global_parameters_timing_ctx_t timings[ECU_TIMING_TYPE_MAX];
}ecu_core_runtime_global_parameters_ctx_t;

#endif /* CORE_CORE_INC_CORE_RUNTIME_PARAMETERS_H_ */
