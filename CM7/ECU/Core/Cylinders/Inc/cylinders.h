/*
 * cylinders.h
 *
 *  Created on: Jun 6, 2024
 *      Author: VHEMaster
 */

#ifndef DRIVERS_HL_CYLINDERS_INC_CYLINDERS_H_
#define DRIVERS_HL_CYLINDERS_INC_CYLINDERS_H_

#include "versioned_cylinders.h"
#include "errors.h"
#include "bool.h"
#include "time.h"
#include <stdint.h>

typedef union {
    uint32_t data;
    struct {

    }bits;
}cylinders_diag_t;

typedef struct {

}cylinders_data_t;

typedef struct {
    uint32_t dummy;
}cylinders_init_ctx_t;

typedef struct {
    cylinders_init_ctx_t init;
    ecu_cylinders_config_t config;
    bool ready;
    bool configured;

    cylinders_diag_t diag;
    cylinders_data_t data;

}cylinders_ctx_t;

error_t cylinders_init(cylinders_ctx_t *ctx, const cylinders_init_ctx_t *init_ctx);
error_t cylinders_configure(cylinders_ctx_t *ctx, const ecu_cylinders_config_t *config);
error_t cylinders_reset(cylinders_ctx_t *ctx);

error_t cylinders_get_data(cylinders_ctx_t *ctx, cylinders_data_t *data);
error_t cylinders_get_diag(cylinders_ctx_t *ctx, cylinders_diag_t *diag);

void cylinders_loop_main(cylinders_ctx_t *ctx);
void cylinders_loop_slow(cylinders_ctx_t *ctx);
void cylinders_loop_fast(cylinders_ctx_t *ctx);


#endif /* DRIVERS_HL_CYLINDERS_INC_CYLINDERS_H_ */
