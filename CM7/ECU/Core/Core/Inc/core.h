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
    bool dutycycle_limit_flag;

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
    float input_ignition_advance;

    float power_voltage;
    float signal_prepare_advance;
    ecu_core_runtime_global_ignition_group_ctx_t groups[ECU_CONFIG_IGNITION_GROUP_MAX];

    uint32_t process_update_trigger_counter;

}ecu_core_runtime_global_ignition_ctx_t;


typedef struct {
    float input_injection_phase;
    float input_injection_mass;

    float power_voltage;
    float signal_prepare_advance;
    ecu_core_runtime_global_injection_group_ctx_t groups[ECU_CONFIG_INJECTION_GROUP_MAX];

    uint32_t process_update_trigger_counter;

}ecu_core_runtime_global_injection_ctx_t;

typedef struct {

    bool read_valid;
}ecu_core_runtime_global_parameters_device_wbls_ctx_t;

typedef struct {

    bool read_valid;
}ecu_core_runtime_global_parameters_device_stepper_ctx_t;

typedef struct {
    ecu_core_runtime_global_parameters_device_wbls_ctx_t wbls[ECU_DEVICE_WBLS_MAX];
    ecu_core_runtime_global_parameters_device_stepper_ctx_t stepper[ECU_DEVICE_STEPPER_MAX];
}ecu_core_runtime_global_parameters_devices_ctx_t;

typedef struct {
    float value;
    bool read_valid;
}ecu_core_runtime_global_parameters_sensor_value_ctx_t;

typedef struct {

    bool read_valid;
}ecu_core_runtime_global_parameters_module_timing_ctx_t;

typedef struct {

    bool write_valid;
}ecu_core_runtime_global_parameters_module_etc_ctx_t;

typedef struct {

    bool write_valid;
}ecu_core_runtime_global_parameters_module_vvt_ctx_t;

typedef struct {

    bool write_valid;
}ecu_core_runtime_global_parameters_module_coolingfan_ctx_t;

typedef struct {

    bool read_valid;
    bool write_valid;
}ecu_core_runtime_global_parameters_module_ignpower_ctx_t;

typedef struct {

    bool write_valid;
}ecu_core_runtime_global_parameters_module_indication_ctx_t;

typedef struct {

    bool write_valid;
}ecu_core_runtime_global_parameters_module_wgcv_ctx_t;

typedef struct {
    ecu_core_runtime_global_parameters_module_timing_ctx_t timing[ECU_MODULE_TIMING_MAX];
    ecu_core_runtime_global_parameters_module_etc_ctx_t etc[ECU_MODULE_ETC_MAX];
    ecu_core_runtime_global_parameters_module_vvt_ctx_t vvt[ECU_MODULE_VVT_MAX];
    ecu_core_runtime_global_parameters_module_coolingfan_ctx_t coolingfan[ECU_MODULE_VVT_MAX];
    ecu_core_runtime_global_parameters_module_ignpower_ctx_t ignpower[ECU_MODULE_IGNPOWER_MAX];
    ecu_core_runtime_global_parameters_module_indication_ctx_t indication[ECU_MODULE_IGNPOWER_MAX];
    ecu_core_runtime_global_parameters_module_wgcv_ctx_t wgcv[ECU_MODULE_WGCV_MAX];
}ecu_core_runtime_global_parameters_modules_ctx_t;

typedef struct {
    ecu_core_runtime_global_parameters_sensor_value_ctx_t sensors[ECU_SENSOR_TYPE_MAX][ECU_SENSOR_INSTANCE_MAX];
    ecu_core_runtime_global_parameters_devices_ctx_t devices;
    ecu_core_runtime_global_parameters_modules_ctx_t modules;
}ecu_core_runtime_global_parameters_ctx_t;

typedef struct {
    uint32_t cylinders_count;
    ecu_core_runtime_global_ignition_ctx_t ignition;
    ecu_core_runtime_global_injection_ctx_t injection;

    ecu_core_runtime_global_parameters_ctx_t parameters;

}ecu_core_runtime_global_ctx_t;

typedef struct {
    timing_req_t timing_req;
    timing_data_crankshaft_t crankshaft_data;
}ecu_core_runtime_cylinder_sequentialed_t;

typedef struct {
    ecu_core_runtime_cylinder_sequentialed_t sequentialed[ECU_CORE_RUNTIME_CYLINDER_SEQUENTIALED_MAX];

}ecu_core_runtime_cylinder_ctx_t;

typedef struct {

}ecu_core_runtime_banked_bank_ctx_t;

typedef struct {
    ecu_core_runtime_banked_bank_ctx_t banks[ECU_BANK_MAX];
}ecu_core_runtime_banked_ctx_t;

typedef struct {

    ecu_core_runtime_global_ctx_t global;
    ecu_core_runtime_banked_ctx_t banked;
    ecu_core_runtime_cylinder_ctx_t cylinders[ECU_CYLINDER_MAX];

}ecu_core_runtime_ctx_t;

typedef struct {
    const ecu_config_global_engine_t *engine_config;
    const ecu_config_engine_calibration_t *calibration;
    ecu_config_engine_runtime_t *runtime_config;

    bool initialized;
    bool core_init_request;
    error_t core_init_errcode;

    ecu_core_init_fsm_t fsm_core_init;

    timing_ctx_t *timing_ctx;
    timing_data_t timing_data;
    timing_diag_t timing_diag;

    ecu_core_runtime_ctx_t runtime;
    uint32_t process_update_trigger_counter;
}ecu_core_ctx_t;

void core_init(void);
void core_loop_main(void);
void core_loop_slow(void);
void core_loop_fast(void);

#endif /* CORE_INC_CORE_H_ */
