/*
 * etc.h
 *
 *  Created on: May 16, 2024
 *      Author: VHEMaster
 */

#ifndef DRIVERS_HL_ETC_INC_ETC_H_
#define DRIVERS_HL_ETC_INC_ETC_H_

#include "versioned_etc.h"
#include "errors.h"
#include "bool.h"
#include <stdint.h>

typedef struct {
    bool enabled;
    float target_position;
    float current_position;
    float dutycycle;
    uint32_t diag;
}etc_data_t;

typedef struct {
    uint32_t dummy;
}etc_init_ctx_t;

typedef struct {
    etc_init_ctx_t init;
    etc_config_t config;
    bool ready;
    bool configured;

    etc_data_t data;

}etc_ctx_t;

error_t etc_init(etc_ctx_t *ctx, const etc_init_ctx_t *init_ctx);
error_t etc_configure(etc_ctx_t *ctx, const etc_config_t *config);
error_t etc_reset(etc_ctx_t *ctx);

void etc_loop_main(etc_ctx_t *ctx);
void etc_loop_slow(etc_ctx_t *ctx);
void etc_loop_fast(etc_ctx_t *ctx);


#endif /* DRIVERS_HL_ETC_INC_ETC_H_ */
