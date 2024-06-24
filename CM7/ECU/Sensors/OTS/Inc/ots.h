/*
 * ots.h
 *
 *  Created on: Jun 24, 2024
 *      Author: VHEMaster
 */

#ifndef SENSORS_OTS_INC_OTS_H_
#define SENSORS_OTS_INC_OTS_H_

#include "common.h"
#include "inputs.h"
#include "time.h"
#include "versioned_ots.h"

typedef union {
    uint32_t data;
    struct {
        bool level_low : 1;
        bool level_high : 1;
    }bits;
}ots_diag_t;

typedef struct {
    float input_value;
    float temperature;
    bool valid;
}ots_data_t;

typedef struct {
    uint32_t dummy;
}ots_init_ctx_t;

typedef struct {
    ots_init_ctx_t init;
    ots_config_t config;
    bool ready;
    bool configured;
    bool pin_locked;

    input_id_t input_id;

    ots_data_t data;
    ots_diag_t diag;

    bool started;
    time_us_t startup_time;
    time_us_t poll_time;
    time_delta_us_t poll_delta;

}ots_ctx_t;

error_t ots_init(ots_ctx_t *ctx, const ots_init_ctx_t *init_ctx);
error_t ots_configure(ots_ctx_t *ctx, const ots_config_t *config);
error_t ots_reset(ots_ctx_t *ctx);

void ots_loop_main(ots_ctx_t *ctx);
void ots_loop_slow(ots_ctx_t *ctx);
void ots_loop_fast(ots_ctx_t *ctx);

error_t ots_get_value(ots_ctx_t *ctx, ots_data_t *data);
error_t ots_get_diag(ots_ctx_t *ctx, ots_diag_t *diag);

#endif /* SENSORS_OTS_INC_OTS_H_ */
