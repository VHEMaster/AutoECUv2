/*
 * tps.h
 *
 *  Created on: May 14, 2024
 *      Author: VHEMaster
 */

#ifndef SENSORS_TPS_INC_TPS_H_
#define SENSORS_TPS_INC_TPS_H_

#include "inputs.h"
#include "errors.h"
#include "time.h"
#include "versioned_tps.h"
#include <stdint.h>

typedef enum {
  TPS_DIAG_OK = 0,
  TPS_DIAG_SIGNAL_1_LEVEL_LOW = 1 << 0,
  TPS_DIAG_SIGNAL_1_LEVEL_HIGH = 1 << 1,
  TPS_DIAG_SIGNAL_2_LEVEL_LOW = 1 << 2,
  TPS_DIAG_SIGNAL_2_LEVEL_HIGH = 1 << 3,
  TPS_DIAG_SIGNALS_IMBALANCE = 1 << 4,
}tps_diag_t;

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
    float voltages[TPS_CONFIG_SIGNALS_MAX];
    float positions_raw[TPS_CONFIG_SIGNALS_MAX];
    float position_imbalance;
    float position_unfiltered;
    float position;

    tps_diag_t diag_value;

    bool started;
    time_us_t startup_time;
    time_us_t signal_poll_time;
    time_delta_us_t poll_delta;

}tps_ctx_t;

error_t tps_init(tps_ctx_t *ctx, const tps_init_ctx_t *init_ctx);
error_t tps_configure(tps_ctx_t *ctx, const tps_config_t *config);
error_t tps_reset(tps_ctx_t *ctx);

void tps_loop_main(tps_ctx_t *ctx);
void tps_loop_slow(tps_ctx_t *ctx);
void tps_loop_fast(tps_ctx_t *ctx);

#endif /* SENSORS_TPS_INC_TPS_H_ */
