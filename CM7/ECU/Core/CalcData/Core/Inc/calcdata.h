/*
 * calcdata.h
 *
 *  Created on: Feb 16, 2025
 *      Author: VHEMaster
 */

#ifndef CORE_CALCDATA_INC_CORE_CALCDATA_H_
#define CORE_CALCDATA_INC_CORE_CALCDATA_H_

#include "core.h"

typedef enum {
  CALCDATA_CALC_TYPE_GLOBAL = 0,
  CALCDATA_CALC_TYPE_BANKED,
  CALCDATA_CALC_TYPE_MAX
}ecu_core_calcdata_calc_type_t;

typedef enum {
  ECU_TIMING_TYPE_IGNITION,
  ECU_TIMING_TYPE_INJECTION,
  ECU_TIMING_TYPE_MAX
}ecu_timing_type_t;

typedef struct {
    ecu_core_calcdata_calc_type_t type;
    ecu_bank_t bank;
}ecu_core_calcdata_calc_type_ctx_t;

typedef void (*ecu_core_calcdata_sensor_read_func_t)(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata, ecu_core_runtime_value_ctx_t *result);
typedef void (*ecu_core_calcdata_module_proc_func_t)(ecu_core_ctx_t *ctx, ecu_module_instance_t instance, void *userdata);
typedef void (*ecu_core_calcdata_device_proc_func_t)(ecu_core_ctx_t *ctx, ecu_device_instance_t instance, void *userdata);
typedef void (*ecu_core_calcdata_timing_proc_func_t)(ecu_core_ctx_t *ctx, void *userdata);

typedef struct {
    ecu_sensor_instance_t max;
    ecu_core_calcdata_sensor_read_func_t func_read;
    void *userdata;
}ecu_core_calcdata_sensor_ctx_t;

typedef struct {
    ecu_module_instance_t max;
    ecu_core_calcdata_module_proc_func_t func_read;
    ecu_core_calcdata_module_proc_func_t func_write;
    void *userdata;
}ecu_core_calcdata_module_ctx_t;

typedef struct {
    ecu_device_instance_t max;
    ecu_core_calcdata_device_proc_func_t func_read;
    ecu_core_calcdata_device_proc_func_t func_write;
    void *userdata;
}ecu_core_calcdata_device_ctx_t;

typedef struct {
    ecu_core_calcdata_timing_proc_func_t func_read;
    ecu_core_calcdata_timing_proc_func_t func_write;
    void *userdata;
}ecu_core_calcdata_timing_ctx_t;

typedef struct {
  ecu_core_calcdata_sensor_ctx_t sensors[ECU_SENSOR_TYPE_MAX];
}ecu_core_calcdata_sensors_ctx_t;

typedef struct {
  ecu_core_calcdata_module_ctx_t modules[ECU_MODULE_TYPE_MAX];
}ecu_core_calcdata_modules_ctx_t;

typedef struct {
  ecu_core_calcdata_device_ctx_t devices[ECU_DEVICE_TYPE_MAX];
}ecu_core_calcdata_devices_ctx_t;

typedef struct {
  ecu_core_calcdata_timing_ctx_t timings[ECU_TIMING_TYPE_MAX];
}ecu_core_calcdata_timings_ctx_t;

void core_calcdata_loop_slow(ecu_core_ctx_t *ctx);

#endif /* CORE_CALCDATA_INC_CORE_CALCDATA_H_ */
