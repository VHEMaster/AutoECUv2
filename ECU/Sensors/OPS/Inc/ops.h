/*
 * ops.h
 *
 *  Created on: Jun 24, 2024
 *      Author: VHEMaster
 */

#ifndef SENSORS_OPS_INC_OPS_H_
#define SENSORS_OPS_INC_OPS_H_

#include "common.h"
#include "inputs.h"
#include "time.h"
#include "versioned_ops.h"

typedef union {
    uint32_t data;
    struct {
        bool level_low : 1;
        bool level_high : 1;
    }bits;
}ops_diag_t;

typedef struct {
    float input_value;
    float pressure;
    bool valid;
}ops_data_t;

typedef struct {
    uint32_t dummy;
}ops_init_ctx_t;

typedef struct {
    ops_init_ctx_t init;
    ops_config_t config;
    bool ready;
    bool configured;
    bool pin_locked;

    input_id_t input_id;

    ops_data_t data;
    ops_diag_t diag;

    bool started;
    time_us_t startup_time;
    time_us_t poll_time;
    time_delta_us_t poll_delta;

}ops_ctx_t;

error_t ops_init(ops_ctx_t *ctx, const ops_init_ctx_t *init_ctx);
error_t ops_configure(ops_ctx_t *ctx, const ops_config_t *config);
error_t ops_reset(ops_ctx_t *ctx);

void ops_loop_main(ops_ctx_t *ctx);
void ops_loop_slow(ops_ctx_t *ctx);
void ops_loop_fast(ops_ctx_t *ctx);

error_t ops_get_value(ops_ctx_t *ctx, ops_data_t *data);
error_t ops_get_diag(ops_ctx_t *ctx, ops_diag_t *diag);

#endif /* SENSORS_OPS_INC_OPS_H_ */
