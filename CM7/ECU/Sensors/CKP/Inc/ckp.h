/*
 * ckp.h
 *
 *  Created on: May 14, 2024
 *      Author: VHEMaster
 */

#ifndef SENSORS_CKP_INC_CKP_H_
#define SENSORS_CKP_INC_CKP_H_

#include "inputs.h"
#include "errors.h"
#include "versioned_ckp.h"
#include <stdint.h>

typedef struct {
    uint32_t dummy;
}ckp_init_ctx_t;

typedef struct {
    ckp_init_ctx_t init;
    ckp_config_t config;
    bool ready;
    bool configured;
}ckp_ctx_t;

error_t ckp_init(ckp_ctx_t *ctx, const ckp_init_ctx_t *init_ctx);
error_t ckp_configure(ckp_ctx_t *ctx, const ckp_config_t *config);
error_t ckp_reset(ckp_ctx_t *ctx);

void ckp_loop_main(ckp_ctx_t *ctx);
void ckp_loop_slow(ckp_ctx_t *ctx);
void ckp_loop_fast(ckp_ctx_t *ctx);

#endif /* SENSORS_CKP_INC_CKP_H_ */
