/*
 * iat.h
 *
 *  Created on: May 14, 2024
 *      Author: VHEMaster
 */

#ifndef SENSORS_IAT_INC_IAT_H_
#define SENSORS_IAT_INC_IAT_H_

#include "inputs.h"
#include "errors.h"
#include "versioned_iat.h"
#include <stdint.h>

typedef struct {
    uint32_t dummy;
}iat_init_ctx_t;

typedef struct {
    iat_init_ctx_t init;
    iat_config_t config;
    bool ready;
    bool configured;
}iat_ctx_t;

error_t iat_init(iat_ctx_t *ctx, const iat_init_ctx_t *init_ctx);
error_t iat_configure(iat_ctx_t *ctx, const iat_config_t *config);
error_t iat_reset(iat_ctx_t *ctx);

void iat_loop_main(iat_ctx_t *ctx);
void iat_loop_slow(iat_ctx_t *ctx);
void iat_loop_fast(iat_ctx_t *ctx);

#endif /* SENSORS_IAT_INC_IAT_H_ */
