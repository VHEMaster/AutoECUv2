/*
 * aps.h
 *
 *  Created on: May 14, 2024
 *      Author: VHEMaster
 */

#ifndef SENSORS_APS_INC_APS_H_
#define SENSORS_APS_INC_APS_H_

#include "inputs.h"
#include "errors.h"
#include "time.h"
#include "versioned_aps.h"
#include <stdint.h>

typedef union {
    uint32_t data;
    struct {
        bool signal1_level_low : 1;
        bool signal1_level_high : 1;
        bool signal2_level_low : 1;
        bool signal2_level_high : 1;
        bool signals_imbalance : 1;
    }bits;
}aps_diag_t;

typedef struct {
    uint8_t signals_count;
    bool active[APS_CONFIG_SIGNALS_MAX];
    float voltages[APS_CONFIG_SIGNALS_MAX];
    float positions_raw[APS_CONFIG_SIGNALS_MAX];
    float position_unfiltered;
    float position_imbalance;
    float position;
    bool data_valid;
}aps_data_t;

typedef struct {
    uint32_t dummy;
}aps_init_ctx_t;

typedef struct {
    aps_init_ctx_t init;
    aps_config_t config;
    bool ready;
    bool configured;

    bool pins_locked[APS_CONFIG_SIGNALS_MAX];
    input_id_t input_ids[APS_CONFIG_SIGNALS_MAX];
    bool signal_failed[APS_CONFIG_SIGNALS_MAX];

    aps_diag_t diag;
    aps_data_t data;

    bool started;
    time_us_t startup_time;
    time_us_t signal_poll_time;
    time_delta_us_t poll_delta;

}aps_ctx_t;

error_t aps_init(aps_ctx_t *ctx, const aps_init_ctx_t *init_ctx);
error_t aps_configure(aps_ctx_t *ctx, const aps_config_t *config);
error_t aps_reset(aps_ctx_t *ctx);

error_t aps_get_value(aps_ctx_t *ctx, aps_data_t *data);
error_t aps_get_diag(aps_ctx_t *ctx, aps_diag_t *diag);

void aps_loop_main(aps_ctx_t *ctx);
void aps_loop_slow(aps_ctx_t *ctx);
void aps_loop_fast(aps_ctx_t *ctx);

#endif /* SENSORS_APS_INC_APS_H_ */
