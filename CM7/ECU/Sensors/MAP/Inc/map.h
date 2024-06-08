/*
 * map.h
 *
 *  Created on: May 14, 2024
 *      Author: VHEMaster
 */

#ifndef SENSORS_MAP_INC_MAP_H_
#define SENSORS_MAP_INC_MAP_H_

#include "common.h"
#include "inputs.h"
#include "time.h"
#include "versioned_map.h"

#define MAP_FREQ_ITEMS_COUNT    2
#define MAP_FREQ_TIMEOUT_US     (100 * TIME_US_IN_MS)

typedef union {
    uint32_t data;
    struct {
        bool level_low : 1;
        bool level_high : 1;
        bool freq_low : 1;
        bool freq_high : 1;
        bool freq_no_signal : 1;
    }bits;
}map_diag_t;

typedef struct {
    float input_frequency;
    float input_voltage;
    float manifold_air_pressure;
    bool valid;
}map_data_t;

typedef struct {
    uint32_t dummy;
}map_init_ctx_t;

typedef struct {
    map_init_ctx_t init;
    map_config_t config;
    bool ready;
    bool configured;
    bool pin_locked;

    input_id_t input_id;

    uint8_t input_freq_index;
    time_us_t input_freq_times[MAP_FREQ_ITEMS_COUNT];
    float input_freq_values[MAP_FREQ_ITEMS_COUNT];

    map_data_t data;
    map_diag_t diag;

    bool started;
    time_us_t startup_time;
    time_us_t freq_last_time;

}map_ctx_t;

error_t map_init(map_ctx_t *ctx, const map_init_ctx_t *init_ctx);
error_t map_configure(map_ctx_t *ctx, const map_config_t *config);
error_t map_reset(map_ctx_t *ctx);

void map_loop_main(map_ctx_t *ctx);
void map_loop_slow(map_ctx_t *ctx);
void map_loop_fast(map_ctx_t *ctx);

error_t map_get_value(map_ctx_t *ctx, map_data_t *data);
error_t map_get_diag(map_ctx_t *ctx, map_diag_t *diag);

#endif /* SENSORS_MAP_INC_MAP_H_ */
