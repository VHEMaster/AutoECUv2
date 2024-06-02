/*
 * egt.h
 *
 *  Created on: May 14, 2024
 *      Author: VHEMaster
 */

#ifndef SENSORS_EGT_INC_EGT_H_
#define SENSORS_EGT_INC_EGT_H_

#include "inputs.h"
#include "errors.h"
#include "time.h"
#include "versioned_egt.h"
#include <stdint.h>

typedef union {
    uint32_t data;
    struct {
        bool device_error : 1;
        bool temperature_too_low : 1;
        bool temperature_too_high : 1;
    }bits;
}egt_diag_t;

typedef struct {
    float sensor_temperature;
    float reference_temperature;
    bool valid;
}egt_data_t;

typedef struct {
    uint32_t dummy;
}egt_init_ctx_t;

typedef struct {
    egt_init_ctx_t init;
    egt_config_t config;
    bool ready;
    bool configured;

    max31855_data_t device_data;
    max31855_diag_t device_diag;

    egt_data_t data;
    egt_diag_t diag;

    bool started;
    time_us_t startup_time;

}egt_ctx_t;

error_t egt_init(egt_ctx_t *ctx, const egt_init_ctx_t *init_ctx);
error_t egt_configure(egt_ctx_t *ctx, const egt_config_t *config);
error_t egt_reset(egt_ctx_t *ctx);

void egt_loop_main(egt_ctx_t *ctx);
void egt_loop_slow(egt_ctx_t *ctx);
void egt_loop_fast(egt_ctx_t *ctx);

error_t egt_get_value(egt_ctx_t *ctx, egt_data_t *data);
error_t egt_get_diag(egt_ctx_t *ctx, egt_diag_t *diag);

#endif /* SENSORS_EGT_INC_EGT_H_ */
