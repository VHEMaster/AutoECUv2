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

typedef enum {
  MAF_DIAG_OK = 0,
  MAF_DIAG_LEVEL_LOW = 1,
  MAF_DIAG_LEVEL_HIGH = 2,
  MAF_DIAG_FREQ_LOW = 4,
  MAF_DIAG_FREQ_HIGH = 8,
}maf_diag_t;

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
    time_us_t input_freq_times[2];

    float input_value;
    float output_value;
    maf_diag_t diag_value;

    bool started;
    time_us_t startup_time;

}maf_ctx_t;

error_t maf_init(maf_ctx_t *ctx, const maf_init_ctx_t *init_ctx);
error_t maf_configure(maf_ctx_t *ctx, const maf_config_t *config);
error_t maf_reset(maf_ctx_t *ctx);

void maf_loop_main(maf_ctx_t *ctx);
void maf_loop_slow(maf_ctx_t *ctx);
void maf_loop_fast(maf_ctx_t *ctx);

#endif /* SENSORS_MAF_INC_MAF_H_ */
