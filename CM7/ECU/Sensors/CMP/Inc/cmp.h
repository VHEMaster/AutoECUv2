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
#include "versioned_cmp.h"
#include <stdint.h>

typedef struct {
    uint32_t dummy;
}cmp_init_ctx_t;

typedef struct {
    cmp_init_ctx_t init;
    cmp_config_t config;
    bool ready;
    bool configured;
}cmp_ctx_t;

error_t cmp_init(cmp_ctx_t *ctx, const cmp_init_ctx_t *init_ctx);
error_t cmp_configure(cmp_ctx_t *ctx, const cmp_config_t *config);
error_t cmp_reset(cmp_ctx_t *ctx);

void cmp_loop_main(cmp_ctx_t *ctx);
void cmp_loop_slow(cmp_ctx_t *ctx);
void cmp_loop_fast(cmp_ctx_t *ctx);

#endif /* SENSORS_CMP_INC_CMP_H_ */
