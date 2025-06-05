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

#include "core_runtime_parameters.h"

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
    float advance_mean;
    float advance_requested_mean;
    float advance[ECU_BANK_MAX];
    float advance_requested[ECU_BANK_MAX];

    ecu_core_runtime_group_cylinder_ignition_ctx_t cylinders[ECU_CYLINDER_MAX];

}ecu_core_runtime_global_ignition_group_ctx_t;

typedef struct {
    bool initialized;
    ecu_core_runtime_cylinder_sequentialed_type_t sequentialed_mode;

    float lag_time;
    float phase;
    float phase_requested;
    float time_inject_mean;
    float dutycycle_max;
    float dutycycle_mean;
    float enrichment_late_phase;

    ecu_core_runtime_group_cylinder_injection_ctx_t cylinders[ECU_CYLINDER_MAX];
}ecu_core_runtime_global_injection_group_ctx_t;

typedef struct {
    bool valid;
    float ignition_advance_banked[ECU_BANK_MAX];
}ecu_core_runtime_global_ignition_input_ctx_t;

typedef struct {
    ecu_core_runtime_global_ignition_input_ctx_t input;

    float power_voltage;
    float signal_prepare_advance;
    ecu_core_runtime_global_ignition_group_ctx_t groups[ECU_CONFIG_IGNITION_GROUP_MAX];

    uint32_t process_update_trigger_counter;

}ecu_core_runtime_global_ignition_ctx_t;

typedef struct {
    bool valid;
    float injection_phase;
    float injection_mass_banked[ECU_BANK_MAX];
}ecu_core_runtime_global_injection_input_ctx_t;

typedef struct {
    ecu_core_runtime_global_injection_input_ctx_t input;

    float power_voltage;
    float signal_prepare_advance;
    ecu_core_runtime_global_injection_group_ctx_t groups[ECU_CONFIG_INJECTION_GROUP_MAX];

    uint32_t process_update_trigger_counter;

}ecu_core_runtime_global_injection_ctx_t;

typedef struct {
    bool ready;
    bool measuring_start;
    bool measuring_end;
    time_us_t time_start;
    time_us_t time_tdc;
    time_us_t time_end;

    float rough_value;
}ecu_core_runtime_group_cylinder_rough_ctx_t;

typedef struct {
    ecu_core_runtime_cylinder_sequentialed_type_t sequentialed_mode;

    ecu_core_runtime_group_cylinder_rough_ctx_t cylinders[ECU_CYLINDER_MAX];

}ecu_core_runtime_global_rough_ctx_t;

typedef struct {
    uint32_t banks_count;
    uint32_t cylinders_count;
    ecu_core_runtime_global_ignition_ctx_t ignition;
    ecu_core_runtime_global_injection_ctx_t injection;
    ecu_core_runtime_global_rough_ctx_t rough;

    ecu_core_runtime_global_parameters_ctx_t parameters;
    ecu_core_runtime_global_parameters_ctx_t parameters_simulated;

}ecu_core_runtime_global_ctx_t;

typedef struct {
    timing_req_t timing_req;
    timing_data_crankshaft_t crankshaft_data;
}ecu_core_runtime_cylinder_sequentialed_t;

typedef struct {
    float rough_value;
}ecu_core_runtime_cylinder_rough_t;

typedef struct {
    ecu_core_runtime_cylinder_sequentialed_t sequentialed[ECU_CORE_RUNTIME_CYLINDER_SEQUENTIALED_MAX];

}ecu_core_runtime_cylinder_ctx_t;

typedef struct {
    const ecu_core_runtime_global_parameters_sensor_value_ctx_t *sensors_aps[ECU_CONFIG_IO_APS_MAX];
    const ecu_core_runtime_global_parameters_sensor_value_ctx_t *sensors_ect[ECU_CONFIG_IO_ECT_MAX];
    const ecu_core_runtime_global_parameters_sensor_value_ctx_t *sensors_ckp[ECU_CONFIG_IO_CMP_MAX];
    const ecu_core_runtime_global_parameters_sensor_value_ctx_t *sensors_ops[ECU_CONFIG_IO_OPS_MAX];
    const ecu_core_runtime_global_parameters_sensor_value_ctx_t *sensors_ots[ECU_CONFIG_IO_OTS_MAX];
    const ecu_core_runtime_global_parameters_sensor_value_ctx_t *sensors_vss[ECU_CONFIG_IO_VSS_MAX];

    const ecu_core_runtime_global_parameters_module_coolingfan_ctx_t *modules_coolingfan[ECU_CONFIG_IO_COOLINGFAN_MAX];
    const ecu_core_runtime_global_parameters_module_ignpower_ctx_t *modules_ignpower[ECU_CONFIG_IO_IGNPOWER_MAX];
    const ecu_core_runtime_global_parameters_module_indication_ctx_t *modules_indication[ECU_CONFIG_IO_INDICATION_MAX];
    const ecu_core_runtime_global_parameters_module_timing_ctx_t *modules_timing[ECU_CONFIG_IO_TIMING_MAX];
}ecu_core_runtime_banked_global_parameters_ctx_t;

typedef struct {
    const ecu_core_runtime_global_parameters_sensor_value_ctx_t *sensors_cmp[ECU_CONFIG_IO_CMP_MAX];
    const ecu_core_runtime_global_parameters_sensor_value_ctx_t *sensors_egt[ECU_CONFIG_IO_EGT_MAX];
    const ecu_core_runtime_global_parameters_sensor_value_ctx_t *sensors_iat[ECU_CONFIG_IO_IAT_MAX];
    const ecu_core_runtime_global_parameters_sensor_value_ctx_t *sensors_maf[ECU_CONFIG_IO_MAF_MAX];
    const ecu_core_runtime_global_parameters_sensor_value_ctx_t *sensors_map[ECU_CONFIG_IO_MAP_MAX];
    const ecu_core_runtime_global_parameters_sensor_value_ctx_t *sensors_tps[ECU_CONFIG_IO_TPS_MAX];

    const ecu_core_runtime_global_parameters_device_stepper_ctx_t *devices_stepper[ECU_CONFIG_IO_STEPPER_MAX];
    const ecu_core_runtime_global_parameters_device_wbls_ctx_t *devices_wbls[ECU_CONFIG_IO_WBLS_MAX];

    const ecu_core_runtime_global_parameters_module_etc_ctx_t *modules_etc[ECU_CONFIG_IO_ETC_MAX];
    const ecu_core_runtime_global_parameters_module_vvt_ctx_t *modules_vvt[ECU_CONFIG_IO_VVT_MAX];
    const ecu_core_runtime_global_parameters_module_wgcv_ctx_t *modules_wgcv[ECU_CONFIG_IO_WGCV_MAX];
}ecu_core_runtime_banked_bank_parameters_ctx_t;

typedef struct {
    ecu_core_runtime_banked_global_parameters_ctx_t global;
    ecu_core_runtime_banked_bank_parameters_ctx_t banks[ECU_BANK_MAX];
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
