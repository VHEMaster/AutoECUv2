/*
 * tps.h
 *
 *  Created on: May 14, 2024
 *      Author: VHEMaster
 */

#ifndef SENSORS_TPS_INC_TPS_H_
#define SENSORS_TPS_INC_TPS_H_

#include "common.h"
#include "inputs.h"
#include "time.h"
#include "versioned_tps.h"

typedef union {
    uint32_t data;
    struct {
        bool signal1_level_low : 1;
        bool signal1_level_high : 1;
        bool signal2_level_low : 1;
        bool signal2_level_high : 1;
        bool signals_imbalance : 1;
    }bits;
}tps_diag_t;

typedef struct {
    uint8_t signals_count;
    bool active[TPS_CONFIG_SIGNALS_MAX];
    float voltages[TPS_CONFIG_SIGNALS_MAX];
    float positions_raw[TPS_CONFIG_SIGNALS_MAX];
    float position_unfiltered;
    float position_imbalance;
    float position;
    bool valid;
}tps_data_t;

typedef struct {
    uint32_t dummy;
}tps_init_ctx_t;

typedef struct {
    tps_init_ctx_t init;
    tps_config_t config;
    bool ready;
    bool configured;

    bool pins_locked[TPS_CONFIG_SIGNALS_MAX];
    input_id_t input_ids[TPS_CONFIG_SIGNALS_MAX];
    bool signal_failed[TPS_CONFIG_SIGNALS_MAX];

    tps_diag_t diag;
    tps_data_t data;

    bool started;
    time_us_t startup_time;
    time_us_t signal_poll_time;
    time_delta_us_t poll_delta;

}tps_ctx_t;

error_t tps_init(tps_ctx_t *ctx, const tps_init_ctx_t *init_ctx);
error_t tps_configure(tps_ctx_t *ctx, const tps_config_t *config);
error_t tps_reset(tps_ctx_t *ctx);

error_t tps_get_value(tps_ctx_t *ctx, tps_data_t *data);
error_t tps_get_diag(tps_ctx_t *ctx, tps_diag_t *diag);

void tps_loop_main(tps_ctx_t *ctx);
void tps_loop_slow(tps_ctx_t *ctx);
void tps_loop_fast(tps_ctx_t *ctx);

#endif /* SENSORS_TPS_INC_TPS_H_ */
