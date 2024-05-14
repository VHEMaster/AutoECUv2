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
#include "versioned_ect.h"
#include <stdint.h>

typedef struct {
    uint32_t dummy;
}ect_init_ctx_t;

typedef struct {
    ect_init_ctx_t init;
    ect_config_t config;
    bool ready;
    bool configured;
}ect_ctx_t;

error_t ect_init(ect_ctx_t *ctx, const ect_init_ctx_t *init_ctx);
error_t ect_configure(ect_ctx_t *ctx, const ect_config_t *config);
error_t ect_reset(ect_ctx_t *ctx);

void ect_loop_main(ect_ctx_t *ctx);
void ect_loop_slow(ect_ctx_t *ctx);
void ect_loop_fast(ect_ctx_t *ctx);

#endif /* SENSORS_ECT_INC_ECT_H_ */
