/*
 * timing_knockwindow.h
 *
 *  Created on: 5 окт. 2026 г.
 *      Author: VHEMaster
 */

#ifndef TIMINGS_KNOCK_INC_TIMING_KNOCKWINDOW_H_
#define TIMINGS_KNOCK_INC_TIMING_KNOCKWINDOW_H_

#include "core.h"
#include "config_engine.h"

typedef struct {
    ecu_core_runtime_value_ctx_t allowed;
    ecu_core_runtime_value_ctx_t knock_window_start;
    ecu_core_runtime_value_ctx_t knock_window_end;
}knockwindow_runtime_input_ctx_t;

typedef struct {
    knockwindow_runtime_input_ctx_t inputs;

    timing_base_runtime_cylinder_sequentialed_type_t sequentialed_mode;
}knockwindow_runtime_ctx_t;

typedef struct {
    const ecu_config_engine_calibration_t *calibration_config;

}knockwindow_init_ctx_t;

typedef struct {
    knockwindow_init_ctx_t init;
    knockwindow_config_t config;
    bool ready;
    bool configured;

    knockwindow_runtime_ctx_t runtime;
    const timing_base_data_t *timing_base_data;

}knockwindow_ctx_t;

error_t knockwindow_init(knockwindow_ctx_t *ctx, const knockwindow_init_ctx_t *init_ctx);
error_t knockwindow_configure(knockwindow_ctx_t *ctx, const knockwindow_config_t *config);
error_t knockwindow_reset(knockwindow_ctx_t *ctx);

error_t knockwindow_get_runtime_data_ptr(knockwindow_ctx_t *ctx, knockwindow_runtime_ctx_t **runtime_data);

void knockwindow_signal_update_callback(knockwindow_ctx_t *ctx);

#endif /* TIMINGS_KNOCK_INC_TIMING_KNOCKWINDOW_H_ */
