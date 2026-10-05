/*
 * timing_cybalance.h
 *
 *  Created on: Jun 1, 2025
 *      Author: VHEMaster
 */

#ifndef CORE_CORE_INC_CORE_TIMING_CYBALANCE_H_
#define CORE_CORE_INC_CORE_TIMING_CYBALANCE_H_

#include "core.h"
#include "config_engine.h"

typedef struct {
    bool ready;
    bool measuring_start;
    bool measuring_end;
    time_us_t time_start;
    time_us_t time_tdc;
    time_us_t time_end;

    time_float_delta_us_t delta_btdc;
    time_float_delta_us_t delta_atdc;

    bool value_valid;
    float balance_value;
    float normalized_value;
}cybalance_runtime_cylinder_ctx_t;

typedef struct {
    cybalance_runtime_cylinder_ctx_t cylinders[ECU_CYLINDER_MAX];

    timing_base_runtime_cylinder_sequentialed_type_t sequentialed_mode;
}cybalance_runtime_ctx_t;

typedef struct {
    const ecu_config_engine_calibration_t *calibration_config;

}cybalance_init_ctx_t;

typedef struct {
    cybalance_init_ctx_t init;
    cybalance_config_t config;
    bool ready;
    bool configured;

    cybalance_runtime_ctx_t runtime;
    const timing_base_data_t *timing_base_data;

}cybalance_ctx_t;

error_t cybalance_init(cybalance_ctx_t *ctx, const cybalance_init_ctx_t *init_ctx);
error_t cybalance_configure(cybalance_ctx_t *ctx, const cybalance_config_t *config);
error_t cybalance_reset(cybalance_ctx_t *ctx);

error_t cybalance_get_runtime_data_ptr(cybalance_ctx_t *ctx, cybalance_runtime_ctx_t **runtime_data);

void cybalance_signal_update_callback(cybalance_ctx_t *ctx);

#endif /* CORE_CORE_INC_CORE_TIMING_CYBALANCE_H_ */
