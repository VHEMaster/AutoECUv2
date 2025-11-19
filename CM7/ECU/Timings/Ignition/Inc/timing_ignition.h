/*
 * timing_ignition.h
 *
 *  Created on: Aug 14, 2024
 *      Author: VHEMaster
 */

#ifndef CORE_CORE_INC_CORE_TIMING_IGNITION_H_
#define CORE_CORE_INC_CORE_TIMING_IGNITION_H_

#include "core.h"
#include "config_engine.h"

typedef struct {
    bool initialized;
    bool scheduled;
    bool ignited;

    float position;
    float advance;
    float degrees_per_cycle;
    float degrees_before_ignite;
    ecu_core_runtime_value_ctx_t ignition_acceptance;
}ignition_runtime_group_cylinder_ctx_t;

typedef struct {
    bool initialized;
    bool distributor;
    timing_base_runtime_cylinder_sequentialed_type_t sequentialed_mode;

    float saturation_time_table;
    float saturation_rpm_mult_table;

    float saturation_time;
    float advance_mean;
    float advance_requested_mean;
    float advance_cy[ECU_CYLINDER_MAX];
    float advance_input_cy[ECU_CYLINDER_MAX];
    float advance_requested_cy[ECU_CYLINDER_MAX];

    ignition_runtime_group_cylinder_ctx_t cylinders[ECU_CYLINDER_MAX];

}ignition_runtime_group_ctx_t;

typedef struct {
    ecu_core_runtime_value_ctx_t allowed;
    ecu_core_runtime_value_ctx_t ignition_advance;
}ignition_runtime_input_ctx_t;

typedef struct {
    ecu_core_runtime_value_ctx_t ignition_acceptance;
}ignition_runtime_cylinder_ctx_t;

typedef struct {
    ignition_runtime_input_ctx_t input_banked[ECU_BANK_MAX];

    float power_voltage;
    float signal_prepare_advance;
    ignition_runtime_group_ctx_t groups[IGNITION_CONFIG_GROUP_MAX];
    ignition_runtime_cylinder_ctx_t cylinders[ECU_CYLINDER_MAX];

    uint32_t process_update_trigger_counter;

}ignition_runtime_ctx_t;

typedef struct {
    const ecu_config_engine_calibration_t *calibration_config;

}ignition_init_ctx_t;

typedef struct {
    ignition_init_ctx_t init;
    ignition_config_t config;
    bool ready;
    bool configured;

    ignition_runtime_ctx_t runtime;

}ignition_ctx_t;

error_t ignition_init(ignition_ctx_t *ctx, const ignition_init_ctx_t *init_ctx);
error_t ignition_configure(ignition_ctx_t *ctx, const ignition_config_t *config);
error_t ignition_reset(ignition_ctx_t *ctx);

error_t ignition_get_runtime_data_ptr(ignition_ctx_t *ctx, ignition_runtime_ctx_t **runtime_data);

void ignition_signal_update_callback(ignition_ctx_t *ctx);

#endif /* CORE_CORE_INC_CORE_TIMING_IGNITION_H_ */
