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
#include "versioned_egt.h"
#include <stdint.h>

typedef struct {
    uint32_t dummy;
}egt_init_ctx_t;

typedef struct {
    egt_init_ctx_t init;
    egt_config_t config;
    bool ready;
    bool configured;
}egt_ctx_t;

error_t egt_init(egt_ctx_t *ctx, const egt_init_ctx_t *init_ctx);
error_t egt_configure(egt_ctx_t *ctx, const egt_config_t *config);
error_t egt_reset(egt_ctx_t *ctx);

void egt_loop_main(egt_ctx_t *ctx);
void egt_loop_slow(egt_ctx_t *ctx);
void egt_loop_fast(egt_ctx_t *ctx);

#endif /* SENSORS_EGT_INC_EGT_H_ */
