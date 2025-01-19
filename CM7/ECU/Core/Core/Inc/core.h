/*
 * core.h
 *
 *  Created on: Apr 3, 2024
 *      Author: VHEMaster
 */

#ifndef CORE_INC_CORE_H_
#define CORE_INC_CORE_H_

#include "common.h"
#include "config_engine.h"
#include "config_timing.h"

typedef enum {
  ECU_CORE_INIT_FSM_INITIAL = 0,
  ECU_CORE_INIT_FSM_INIT_FLASH,
  ECU_CORE_INIT_FSM_READ_FLASH,
  ECU_CORE_INIT_FSM_INIT_DEVS,
  ECU_CORE_INIT_FSM_INIT_SENS,
  ECU_CORE_INIT_FSM_INIT_MODULES,
  ECU_CORE_INIT_FSM_COMPLETED,

}ecu_core_init_fsm_t;

typedef enum {
  ECU_CORE_RUNTIME_CYLINDER_SEQUENTIALED_NONE = 0,
  ECU_CORE_RUNTIME_CYLINDER_SEQUENTIAL,
  ECU_CORE_RUNTIME_CYLINDER_SEMISEQUENTIAL_DISTRIBUTOR,
  ECU_CORE_RUNTIME_CYLINDER_SEQUENTIALED_MAX,
}ecu_core_runtime_cylinder_sequentialed_type_t;

typedef struct {
    bool initialized;
    bool scheduled;
    bool ignited;

    float position;
    float advance;
    float degrees_per_cycle;
    float degrees_before_ignite;
}ecu_core_runtime_group_cylinder_ignition_ctx_t;

typedef struct {
    bool initialized;
    bool scheduled;
    bool injected;

    float phase;
    float time_lag;
    float time_inject;
    float time_pulse;
    float degrees_per_cycle;
    float degrees_before_inject;
    float dutycycle;
}ecu_core_runtime_group_cylinder_injection_ctx_t;

typedef struct {
    bool initialized;
    bool distributor;
    ecu_core_runtime_cylinder_sequentialed_type_t sequentialed_mode;

    float saturation_time_table;
    float saturation_rpm_mult_table;

    float saturation_time;
    float advance;
    float advance_requested;

    ecu_core_runtime_group_cylinder_ignition_ctx_t cylinders[ECU_CYLINDER_MAX];

}ecu_core_runtime_global_ignition_group_ctx_t;

typedef struct {
    bool initialized;
    ecu_core_runtime_cylinder_sequentialed_type_t sequentialed_mode;

    float lag_time;
    float phase;
    float phase_requested;
    float time_inject;
    float dutycycle_max;
    float dutycycle_mean;

    ecu_core_runtime_group_cylinder_injection_ctx_t cylinders[ECU_CYLINDER_MAX];
}ecu_core_runtime_global_injection_group_ctx_t;

typedef struct {
    float power_voltage;
    float signal_prepare_advance;
    float ignition_advance;
    ecu_core_runtime_global_ignition_group_ctx_t groups[ECU_CONFIG_IGNITION_GROUP_MAX];

}ecu_core_runtime_global_ignition_ctx_t;


typedef struct {
    float power_voltage;
    float signal_prepare_advance;
    float injection_phase;
    ecu_core_runtime_global_injection_group_ctx_t groups[ECU_CONFIG_INJECTION_GROUP_MAX];

}ecu_core_runtime_global_injection_ctx_t;

typedef struct {
    ecu_cylinder_t cylinders_count;
    ecu_core_runtime_global_ignition_ctx_t ignition;
    ecu_core_runtime_global_injection_ctx_t injection;

}ecu_core_runtime_global_ctx_t;

typedef struct {
    timing_req_t timing_req;
    timing_data_crankshaft_t crankshaft_data;
}ecu_core_runtime_cylinder_sequentialed_t;

typedef struct {
    ecu_core_runtime_cylinder_sequentialed_t sequentialed[ECU_CORE_RUNTIME_CYLINDER_SEQUENTIALED_MAX];

}ecu_core_runtime_cylinder_ctx_t;

typedef struct {

    ecu_core_runtime_global_ctx_t global;
    ecu_core_runtime_cylinder_ctx_t cylinders[ECU_CYLINDER_MAX];

}ecu_core_runtime_ctx_t;

typedef struct {
    const ecu_config_global_engine_t *engine_config;
    const ecu_config_engine_calibration_t *calibration;
    ecu_config_engine_runtime_t *runtime_config;

    bool core_init_request;
    error_t core_init_errcode;

    ecu_core_init_fsm_t fsm_core_init;

    timing_ctx_t *timing_ctx;
    timing_data_t timing_data;
    timing_diag_t timing_diag;

    ecu_core_runtime_ctx_t runtime;
}ecu_core_ctx_t;

void core_init(void);
void core_loop_main(void);
void core_loop_slow(void);
void core_loop_fast(void);

#endif /* CORE_INC_CORE_H_ */
