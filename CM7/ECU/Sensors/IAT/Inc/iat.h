/*
 * iat.h
 *
 *  Created on: May 14, 2024
 *      Author: VHEMaster
 */

#ifndef SENSORS_IAT_INC_IAT_H_
#define SENSORS_IAT_INC_IAT_H_

#include "inputs.h"
#include "errors.h"
#include "time.h"
#include "versioned_iat.h"
#include <stdint.h>

typedef union {
    uint32_t data;
    struct {
        bool level_low : 1;
        bool level_high : 1;
    }bits;
}iat_diag_t;

typedef struct {
    float input_value;
    float temperature;
    bool valid;
}iat_data_t;

typedef struct {
    uint32_t dummy;
}iat_init_ctx_t;

typedef struct {
    iat_init_ctx_t init;
    iat_config_t config;
    bool ready;
    bool configured;
    bool pin_locked;

    input_id_t input_id;

    iat_data_t data;
    iat_diag_t diag;

    bool started;
    time_us_t startup_time;
    time_us_t poll_time;
    time_delta_us_t poll_delta;

}iat_ctx_t;

error_t iat_init(iat_ctx_t *ctx, const iat_init_ctx_t *init_ctx);
error_t iat_configure(iat_ctx_t *ctx, const iat_config_t *config);
error_t iat_reset(iat_ctx_t *ctx);

void iat_loop_main(iat_ctx_t *ctx);
void iat_loop_slow(iat_ctx_t *ctx);
void iat_loop_fast(iat_ctx_t *ctx);

error_t iat_get_value(iat_ctx_t *ctx, iat_data_t *data);
error_t iat_get_diag(iat_ctx_t *ctx, iat_diag_t *diag);

#endif /* SENSORS_IAT_INC_IAT_H_ */
