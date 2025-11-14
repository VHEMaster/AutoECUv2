/*
 * core_runtime.h
 *
 *  Created on: Jun 5, 2025
 *      Author: VHEMaster
 */

#ifndef CORE_CORE_INC_CORE_RUNTIME_H_
#define CORE_CORE_INC_CORE_RUNTIME_H_

#include "common.h"
#include "config_engine.h"

#include "core_runtime_parameters.h"
#include "core_runtime_banked.h"

typedef enum {
  ECU_CORE_RUNTIME_PARAMS_VIRT_SOURCE_SIMULATED = 0,
  ECU_CORE_RUNTIME_PARAMS_VIRT_SOURCE_DIAG,
  ECU_CORE_RUNTIME_PARAMS_VIRT_SOURCE_CANBUS,
  ECU_CORE_RUNTIME_PARAMS_VIRT_SOURCE_INTERNAL,
  ECU_CORE_RUNTIME_PARAMS_VIRT_SOURCE_MAX,
}ecu_core_runtime_parameters_virtual_source_t;

typedef struct {
    bool runup_flag;
    bool pos_idle_flag;
    bool rpm_idle_flag;
    bool econ_idle_flag;
    bool idle_flag;

    time_float_s_t runned_time_overall;
    time_float_s_t running_time_current;

    uint32_t injection_revs_counter;
    uint32_t injection_startup_revs_counter;

    time_us_t calc_tick_last;
}ecu_core_runtime_global_misc_ctx_t;

typedef struct {

}ecu_core_runtime_global_powermoding_ctx_t;

typedef struct {
    uint32_t banks_count;
    uint32_t cylinders_count;

    ecu_core_runtime_global_parameters_ctx_t parameters;
    ecu_core_runtime_global_parameters_ctx_t parameters_virtual[ECU_CORE_RUNTIME_PARAMS_VIRT_SOURCE_MAX];

    ecu_core_runtime_global_misc_ctx_t misc;

}ecu_core_runtime_global_ctx_t;

typedef struct {
    float rough_value;
}ecu_core_runtime_cylinder_rough_t;

typedef struct {

}ecu_core_runtime_cylinder_ctx_t;

typedef struct {
    ecu_core_runtime_global_ctx_t global;
    ecu_core_runtime_banked_ctx_t banked;
    ecu_core_runtime_cylinder_ctx_t cylinders[ECU_CYLINDER_MAX];

}ecu_core_runtime_ctx_t;

#endif /* CORE_CORE_INC_CORE_RUNTIME_H_ */
