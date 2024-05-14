/*
 * map.h
 *
 *  Created on: May 14, 2024
 *      Author: VHEMaster
 */

#ifndef SENSORS_MAP_INC_MAP_H_
#define SENSORS_MAP_INC_MAP_H_

#include "inputs.h"
#include "errors.h"
#include "versioned_map.h"
#include <stdint.h>

typedef struct {
    uint32_t dummy;
}map_init_ctx_t;

typedef struct {
    map_init_ctx_t init;
    map_config_t config;
    bool ready;
    bool configured;
}map_ctx_t;

error_t map_init(map_ctx_t *ctx, const map_init_ctx_t *init_ctx);
error_t map_configure(map_ctx_t *ctx, const map_config_t *config);
error_t map_reset(map_ctx_t *ctx);

void map_loop_main(map_ctx_t *ctx);
void map_loop_slow(map_ctx_t *ctx);
void map_loop_fast(map_ctx_t *ctx);

#endif /* SENSORS_MAP_INC_MAP_H_ */
