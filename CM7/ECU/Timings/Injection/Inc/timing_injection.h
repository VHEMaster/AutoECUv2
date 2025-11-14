/*
 * timing_injection.h
 *
 *  Created on: Aug 14, 2024
 *      Author: VHEMaster
 */

#ifndef CORE_CORE_INC_CORE_TIMING_INJECTION_H_
#define CORE_CORE_INC_CORE_TIMING_INJECTION_H_

#include "core.h"

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
}injection_runtime_group_cylinder_ctx_t;

typedef struct {
    bool initialized;
    timing_base_runtime_cylinder_sequentialed_type_t sequentialed_mode;

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

    injection_runtime_group_cylinder_ctx_t cylinders[ECU_CYLINDER_MAX];
}injection_runtime_group_ctx_t;

typedef struct {
    ecu_core_runtime_value_ctx_t allowed;
    ecu_core_runtime_value_ctx_t injection_phase;
    ecu_core_runtime_value_ctx_t injection_mass;
}injection_runtime_input_ctx_t;

typedef struct {
    injection_runtime_input_ctx_t input_banked[ECU_BANK_MAX];

    float power_voltage;
    float signal_prepare_advance;
    injection_runtime_group_ctx_t groups[INJECTION_CONFIG_GROUP_MAX];

    uint32_t process_update_trigger_counter;

}injection_runtime_ctx_t;

typedef struct {

}injection_init_ctx_t;

typedef struct {
    injection_init_ctx_t init;
    injection_config_t config;
    bool ready;
    bool configured;

    injection_runtime_ctx_t runtime;
    timing_base_data_t timing_base_data;

}injection_ctx_t;

error_t injection_init(injection_ctx_t *ctx, const injection_init_ctx_t *init_ctx);
error_t injection_configure(injection_ctx_t *ctx, const injection_config_t *config);
error_t injection_reset(injection_ctx_t *ctx);

void injection_signal_update_callback(injection_ctx_t *ctx);

#endif /* CORE_CORE_INC_CORE_TIMING_INJECTION_H_ */
