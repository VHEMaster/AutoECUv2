/*
 * ect.h
 *
 *  Created on: May 14, 2024
 *      Author: VHEMaster
 */

#ifndef SENSORS_ECT_INC_ECT_H_
#define SENSORS_ECT_INC_ECT_H_

#include "inputs.h"
#include "errors.h"
#include "time.h"
#include "versioned_ect.h"
#include <stdint.h>

typedef union {
    uint32_t data;
    struct {
        bool level_low : 1;
        bool level_high : 1;
    }bits;
}ect_diag_t;

typedef struct {
    float input_value;
    float temperature;
    bool valid;
}ect_data_t;

typedef struct {
    uint32_t dummy;
}ect_init_ctx_t;

typedef struct {
    ect_init_ctx_t init;
    ect_config_t config;
    bool ready;
    bool configured;
    bool pin_locked;

    input_id_t input_id;

    ect_data_t data;
    ect_diag_t diag;

    bool started;
    time_us_t startup_time;
    time_us_t poll_time;
    time_delta_us_t poll_delta;

}ect_ctx_t;

error_t ect_init(ect_ctx_t *ctx, const ect_init_ctx_t *init_ctx);
error_t ect_configure(ect_ctx_t *ctx, const ect_config_t *config);
error_t ect_reset(ect_ctx_t *ctx);

void ect_loop_main(ect_ctx_t *ctx);
void ect_loop_slow(ect_ctx_t *ctx);
void ect_loop_fast(ect_ctx_t *ctx);

error_t ect_get_value(ect_ctx_t *ctx, ect_data_t *data);
error_t ect_get_diag(ect_ctx_t *ctx, ect_diag_t *diag);

#endif /* SENSORS_ECT_INC_ECT_H_ */
