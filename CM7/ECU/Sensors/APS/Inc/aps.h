/*
 * aps.h
 *
 *  Created on: May 14, 2024
 *      Author: VHEMaster
 */

#ifndef SENSORS_APS_INC_APS_H_
#define SENSORS_APS_INC_APS_H_

#include "inputs.h"
#include "errors.h"
#include "versioned_aps.h"
#include <stdint.h>

typedef struct {
    uint32_t dummy;
}aps_init_ctx_t;

typedef struct {
    aps_init_ctx_t init;
    aps_config_t config;
    bool ready;
    bool configured;
}aps_ctx_t;

error_t aps_init(aps_ctx_t *ctx, const aps_init_ctx_t *init_ctx);
error_t aps_configure(aps_ctx_t *ctx, const aps_config_t *config);
error_t aps_reset(aps_ctx_t *ctx);

void aps_loop_main(aps_ctx_t *ctx);
void aps_loop_slow(aps_ctx_t *ctx);
void aps_loop_fast(aps_ctx_t *ctx);

#endif /* SENSORS_APS_INC_APS_H_ */
