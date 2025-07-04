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
  ECU_CORE_RUNTIME_CYLINDER_SEQUENTIALED_NONE = 0,
  ECU_CORE_RUNTIME_CYLINDER_SEQUENTIAL,
  ECU_CORE_RUNTIME_CYLINDER_SEMISEQUENTIAL_DISTRIBUTOR,
  ECU_CORE_RUNTIME_CYLINDER_SEQUENTIALED_MAX,
}ecu_core_runtime_cylinder_sequentialed_type_t;

typedef struct {
    bool valid;
    float ignition_advance;
}ecu_core_runtime_group_cylinder_ignition_acceptance_ctx_t;

typedef struct {
    bool initialized;
    bool scheduled;
    bool ignited;

    float position;
    float advance;
    float degrees_per_cycle;
    float degrees_before_ignite;
    ecu_core_runtime_group_cylinder_ignition_acceptance_ctx_t ignition_acceptance;
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
    bool valid;
    float ignition_advance;
}ecu_core_runtime_cylinder_ignition_acceptance_ctx_t;

typedef struct {
    bool initialized;
    bool distributor;
    ecu_core_runtime_cylinder_sequentialed_type_t sequentialed_mode;

    float saturation_time_table;
    float saturation_rpm_mult_table;

    float saturation_time;
    float advance_mean;
    float advance_requested_mean;
    float advance_cy[ECU_CYLINDER_MAX];
    float advance_input_cy[ECU_CYLINDER_MAX];
    float advance_requested_cy[ECU_CYLINDER_MAX];

    ecu_core_runtime_group_cylinder_ignition_ctx_t cylinders[ECU_CYLINDER_MAX];

}ecu_core_runtime_global_ignition_group_ctx_t;

typedef struct {
    bool initialized;
    ecu_core_runtime_cylinder_sequentialed_type_t sequentialed_mode;

    float phase_banked[ECU_BANK_MAX];
    float phase_requested_banked[ECU_BANK_MAX];
    float phase_mean;
    float lag_time;
    float time_inject_mean;
    float dutycycle_max;
    float dutycycle_mean;
    float enrichment_late_phase;

    float injector_input_pressure[ECU_BANK_MAX];
    float injector_output_pressure[ECU_BANK_MAX];
    float injector_pressure_diff[ECU_BANK_MAX];

    float injector_input_pressure_mean;
    float injector_output_pressure_mean;
    float injector_pressure_diff_mean;

    ecu_core_runtime_group_cylinder_injection_ctx_t cylinders[ECU_CYLINDER_MAX];
}ecu_core_runtime_global_injection_group_ctx_t;

typedef struct {
    bool allowed;
    float ignition_advance;
}ecu_core_runtime_global_ignition_input_ctx_t;

typedef struct {
    bool input_valid;
    ecu_core_runtime_global_ignition_input_ctx_t input_banked[ECU_BANK_MAX];

    float power_voltage;
    float signal_prepare_advance;
    ecu_core_runtime_global_ignition_group_ctx_t groups[ECU_CONFIG_IGNITION_GROUP_MAX];

    uint32_t process_update_trigger_counter;

}ecu_core_runtime_global_ignition_ctx_t;

typedef struct {
    bool allowed;
    float injection_phase;
    float injection_mass;
}ecu_core_runtime_global_injection_input_ctx_t;

typedef struct {
    bool input_valid;
    ecu_core_runtime_global_injection_input_ctx_t input_banked[ECU_BANK_MAX];

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
    uint32_t banks_count;
    uint32_t cylinders_count;
    ecu_core_runtime_global_ignition_ctx_t ignition;
    ecu_core_runtime_global_injection_ctx_t injection;
    ecu_core_runtime_global_rough_ctx_t rough;

    ecu_core_runtime_global_parameters_ctx_t parameters;
    ecu_core_runtime_global_parameters_ctx_t parameters_simulated;

    ecu_core_runtime_global_misc_ctx_t misc;

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
    ecu_core_runtime_cylinder_ignition_acceptance_ctx_t ignition_acceptance;

}ecu_core_runtime_cylinder_ctx_t;

typedef struct {
    ecu_core_runtime_global_ctx_t global;
    ecu_core_runtime_banked_ctx_t banked;
    ecu_core_runtime_cylinder_ctx_t cylinders[ECU_CYLINDER_MAX];

}ecu_core_runtime_ctx_t;

#endif /* CORE_CORE_INC_CORE_RUNTIME_H_ */
