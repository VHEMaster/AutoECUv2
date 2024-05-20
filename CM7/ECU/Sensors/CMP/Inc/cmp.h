/*
 * cmp.h
 *
 *  Created on: May 14, 2024
 *      Author: VHEMaster
 */

#ifndef SENSORS_CMP_INC_CMP_H_
#define SENSORS_CMP_INC_CMP_H_

#include "inputs.h"
#include "errors.h"
#include "time.h"
#include "versioned_cmp.h"
#include <stdint.h>

typedef union {
    uint32_t data;
    struct {

    }bits;
}cmp_diag_t;

typedef struct {

}cmp_data_t;

typedef struct {
    uint32_t dummy;
}cmp_init_ctx_t;

typedef struct {
    cmp_init_ctx_t init;
    cmp_config_t config;
    bool ready;
    bool configured;
    bool pin_locked;

    input_id_t input_id;

    cmp_data_t data;
    cmp_diag_t diag;

    bool started;
    time_us_t startup_time;

}cmp_ctx_t;

error_t cmp_init(cmp_ctx_t *ctx, const cmp_init_ctx_t *init_ctx);
error_t cmp_configure(cmp_ctx_t *ctx, const cmp_config_t *config);
error_t cmp_reset(cmp_ctx_t *ctx);

void cmp_loop_main(cmp_ctx_t *ctx);
void cmp_loop_slow(cmp_ctx_t *ctx);
void cmp_loop_fast(cmp_ctx_t *ctx);

error_t cmp_get_value(cmp_ctx_t *ctx, cmp_data_t *data);
error_t cmp_get_diag(cmp_ctx_t *ctx, cmp_diag_t *diag);

#endif /* SENSORS_CMP_INC_CMP_H_ */
