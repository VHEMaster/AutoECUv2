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

typedef struct {

}ecu_core_runtime_global_ignition_ctx_t;

typedef struct {

}ecu_core_runtime_global_injection_ctx_t;

typedef struct {
    ecu_cylinder_t cylinders_count;
    ecu_core_runtime_global_ignition_ctx_t ignition;
    ecu_core_runtime_global_injection_ctx_t injection;

}ecu_core_runtime_global_ctx_t;

typedef struct {

}ecu_core_runtime_cylinder_ignition_ctx_t;

typedef struct {

}ecu_core_runtime_cylinder_injection_ctx_t;

typedef enum {
  ECU_CORE_RUNTIME_CYLINDER_SEQUENTIAL = 0,
  ECU_CORE_RUNTIME_CYLINDER_SEMISEQUENTIAL_DISTRIBUTOR,
  ECU_CORE_RUNTIME_CYLINDER_SEQUENTIALED_MAX,
}ecu_core_runtime_cylinder_sequentialed_type_t;

typedef struct {
    timing_req_t timing_req;
    timing_data_crankshaft_t crankshaft_data;
}ecu_core_runtime_cylinder_sequentialed_t;

typedef struct {
    ecu_core_runtime_cylinder_sequentialed_t sequentialed[ECU_CORE_RUNTIME_CYLINDER_SEQUENTIALED_MAX];
    ecu_core_runtime_cylinder_ignition_ctx_t ignition;
    ecu_core_runtime_cylinder_injection_ctx_t injection;

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
