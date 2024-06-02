/*
 * vss.h
 *
 *  Created on: Jun 2, 2024
 *      Author: VHEMaster
 */

#ifndef SENSORS_VSS_INC_VSS_H_
#define SENSORS_VSS_INC_VSS_H_

#include "inputs.h"
#include "errors.h"
#include "time.h"
#include "versioned_vss.h"
#include <stdint.h>

#define VSS_FREQ_ITEMS_COUNT    2
#define VSS_FREQ_TIMEOUT_US     (500 * TIME_US_IN_MS)

typedef union {
    uint32_t data;
    struct {
        bool speed_too_high : 1;
    }bits;
}vss_diag_t;

typedef struct {
    float pulses_per_second;
    float speed;
    bool valid;
}vss_data_t;

typedef struct {
    uint32_t dummy;
}vss_init_ctx_t;

typedef struct {
    vss_init_ctx_t init;
    vss_config_t config;
    bool ready;
    bool configured;
    bool pin_locked;

    input_id_t input_id;

    uint8_t input_freq_index;
    uint8_t input_freq_count;
    time_us_t input_freq_times[VSS_FREQ_ITEMS_COUNT];
    float input_freq_values[VSS_FREQ_ITEMS_COUNT];

    vss_data_t data;
    vss_diag_t diag;

    bool started;
    time_us_t startup_time;
    time_us_t freq_last_time;

}vss_ctx_t;

error_t vss_init(vss_ctx_t *ctx, const vss_init_ctx_t *init_ctx);
error_t vss_configure(vss_ctx_t *ctx, const vss_config_t *config);
error_t vss_reset(vss_ctx_t *ctx);

void vss_loop_main(vss_ctx_t *ctx);
void vss_loop_slow(vss_ctx_t *ctx);
void vss_loop_fast(vss_ctx_t *ctx);

error_t vss_get_value(vss_ctx_t *ctx, vss_data_t *data);
error_t vss_get_diag(vss_ctx_t *ctx, vss_diag_t *diag);

#endif /* SENSORS_VSS_INC_VSS_H_ */
