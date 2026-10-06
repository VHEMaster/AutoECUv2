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
#include "timing_base.h"

typedef struct knockwindow_sampling_cplt_ctx_tag knockwindow_sampling_cplt_ctx_t;

typedef void (*knockwindow_sampling_cplt_cb_t)(void *usrdata, const knockwindow_sampling_cplt_ctx_t *cplt_ctx);

typedef struct {
    ecu_core_runtime_value_ctx_t allowed;
    ecu_core_runtime_value_ctx_t knock_window_start;
    ecu_core_runtime_value_ctx_t knock_window_end;
}knockwindow_runtime_input_ctx_t;

typedef enum {
  KNOCKWINDOW_STATE_SYNC = 0,
  KNOCKWINDOW_STATE_WAITING,
  KNOCKWINDOW_STATE_PREPARED,
  KNOCKWINDOW_STATE_SAMPLING,
  KNOCKWINDOW_STATE_MAX
}knockwindow_runtime_state_t;

typedef struct knockwindow_sampling_cplt_ctx_tag {
    const uint16_t *samples_buffer;
    uint16_t samples_count;
    time_us_t time_start;
    time_us_t time_cplt;
    float position_start;
    float position_cplt;
}knockwindow_sampling_cplt_ctx_t;

typedef struct {
    knockwindow_runtime_input_ctx_t inputs;

    knockwindow_runtime_state_t state;
    bool cplt_irq;
    bool error_irq;
    uint16_t samples_requested;

    float position_start;
    float position_cplt;

}knockwindow_runtime_ctx_t;

typedef struct {
    const ecu_config_engine_calibration_t *calibration_config;

    knockwindow_sampling_cplt_cb_t callback;
    void *callback_usrdata;

}knockwindow_init_ctx_t;

typedef struct {
    knockwindow_init_ctx_t init;
    knockwindow_config_t config;
    bool ready;
    bool configured;

    knockwindow_runtime_ctx_t runtime;
    const timing_base_data_t *timing_base_data;
    pulsedadc_ctx_t *pulsedadc_ctx;
    knockwindow_sampling_cplt_ctx_t sampling_cplt_ctx;

}knockwindow_ctx_t;

error_t knockwindow_init(knockwindow_ctx_t *ctx, const knockwindow_init_ctx_t *init_ctx);
error_t knockwindow_configure(knockwindow_ctx_t *ctx, const knockwindow_config_t *config);
error_t knockwindow_reset(knockwindow_ctx_t *ctx);

error_t knockwindow_get_runtime_data_ptr(knockwindow_ctx_t *ctx, knockwindow_runtime_ctx_t **runtime_data);

void knockwindow_signal_update_callback(knockwindow_ctx_t *ctx);

#endif /* TIMINGS_KNOCK_INC_TIMING_KNOCKWINDOW_H_ */
