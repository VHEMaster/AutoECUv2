/*
 * tps.h
 *
 *  Created on: May 14, 2024
 *      Author: VHEMaster
 */

#ifndef SENSORS_TPS_INC_TPS_H_
#define SENSORS_TPS_INC_TPS_H_

#include "inputs.h"
#include "errors.h"
#include "versioned_tps.h"
#include <stdint.h>

typedef struct {
    uint32_t dummy;
}tps_init_ctx_t;

typedef struct {
    tps_init_ctx_t init;
    tps_config_t config;
    bool ready;
    bool configured;
}tps_ctx_t;

error_t tps_init(tps_ctx_t *ctx, const tps_init_ctx_t *init_ctx);
error_t tps_configure(tps_ctx_t *ctx, const tps_config_t *config);
error_t tps_reset(tps_ctx_t *ctx);

void tps_loop_main(tps_ctx_t *ctx);
void tps_loop_slow(tps_ctx_t *ctx);
void tps_loop_fast(tps_ctx_t *ctx);

#endif /* SENSORS_TPS_INC_TPS_H_ */
