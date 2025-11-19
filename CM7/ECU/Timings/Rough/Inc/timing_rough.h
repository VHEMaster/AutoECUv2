/*
 * timing_rough.h
 *
 *  Created on: Jun 1, 2025
 *      Author: VHEMaster
 */

#ifndef CORE_CORE_INC_CORE_TIMING_ROUGH_H_
#define CORE_CORE_INC_CORE_TIMING_ROUGH_H_

#include "core.h"
#include "config_engine.h"

typedef struct {
    bool ready;
    bool measuring_start;
    bool measuring_end;
    time_us_t time_start;
    time_us_t time_tdc;
    time_us_t time_end;

    float rough_value;
}rough_runtime_cylinder_ctx_t;

typedef struct {
    rough_runtime_cylinder_ctx_t cylinders[ECU_CYLINDER_MAX];

    timing_base_runtime_cylinder_sequentialed_type_t sequentialed_mode;
}rough_runtime_ctx_t;

typedef struct {
    const ecu_config_engine_calibration_t *calibration_config;

}rough_init_ctx_t;

typedef struct {
    rough_init_ctx_t init;
    rough_config_t config;
    bool ready;
    bool configured;

    rough_runtime_ctx_t runtime;
    timing_base_data_t timing_base_data;

}rough_ctx_t;

error_t rough_init(rough_ctx_t *ctx, const rough_init_ctx_t *init_ctx);
error_t rough_configure(rough_ctx_t *ctx, const rough_config_t *config);
error_t rough_reset(rough_ctx_t *ctx);

error_t rough_get_runtime_data_ptr(rough_ctx_t *ctx, rough_runtime_ctx_t **runtime_data);

void rough_signal_update_callback(rough_ctx_t *ctx);

#endif /* CORE_CORE_INC_CORE_TIMING_ROUGH_H_ */
