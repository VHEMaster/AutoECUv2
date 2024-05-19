/*
 * maf.h
 *
 *  Created on: May 14, 2024
 *      Author: VHEMaster
 */

#ifndef SENSORS_MAF_INC_MAF_H_
#define SENSORS_MAF_INC_MAF_H_

#include "inputs.h"
#include "errors.h"
#include "time.h"
#include "versioned_maf.h"
#include <stdint.h>

#define MAF_FREQ_ITEMS_COUNT    2
#define MAF_FREQ_TIMEOUT_US     (100 * TIME_US_IN_MS)

typedef struct {
  bool level_low : 1;
  bool level_high : 1;
  bool freq_low : 1;
  bool freq_high : 1;
  bool freq_no_signal : 1;
}maf_diag_t;

typedef struct {
    float input_value;
    float output_value;
}maf_data_t;

typedef struct {
    uint32_t dummy;
}maf_init_ctx_t;

typedef struct {
    maf_init_ctx_t init;
    maf_config_t config;
    bool ready;
    bool configured;
    bool pin_locked;

    input_id_t input_id;

    uint8_t input_freq_index;
    time_us_t input_freq_times[MAF_FREQ_ITEMS_COUNT];
    float input_freq_values[MAF_FREQ_ITEMS_COUNT];

    maf_data_t data;
    maf_diag_t diag;

    bool started;
    time_us_t startup_time;
    time_us_t freq_last_time;

}maf_ctx_t;

error_t maf_init(maf_ctx_t *ctx, const maf_init_ctx_t *init_ctx);
error_t maf_configure(maf_ctx_t *ctx, const maf_config_t *config);
error_t maf_reset(maf_ctx_t *ctx);

void maf_loop_main(maf_ctx_t *ctx);
void maf_loop_slow(maf_ctx_t *ctx);
void maf_loop_fast(maf_ctx_t *ctx);

error_t maf_get_value(maf_ctx_t *ctx, maf_data_t *data);
error_t maf_get_diag(maf_ctx_t *ctx, maf_diag_t *diag);

#endif /* SENSORS_MAF_INC_MAF_H_ */
