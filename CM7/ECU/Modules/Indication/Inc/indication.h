/*
 * indication.h
 *
 *  Created on: Jun 24, 2024
 *      Author: VHEMaster
 */

#ifndef MODULES_INDICATION_INC_INDICATION_H_
#define MODULES_INDICATION_INC_INDICATION_H_

#include "common.h"
#include "versioned_indication.h"
#include "config_gpio.h"
#include "config_sensors.h"
#include "time.h"

typedef union {
    uint32_t data;
    struct {
        bool output_drive_error : 1;
    }bits;
}indication_diag_t;

typedef struct {
    bool enabled;
    bool indication_set;

    bool manual_engaged;
    bool manual_enabled;
    bool force_engaged;
    bool force_enabled;

    bool ignpower_on;
    bool ckp_triggered;
}indication_data_t;

typedef struct {
    uint32_t dummy;
}indication_init_ctx_t;

typedef struct {
    indication_init_ctx_t init;
    indication_config_t config;
    bool ready;
    bool configured;

    bool output_pin_locked;

    bool indication_set_prev;

    indication_diag_t diag;
    indication_data_t data;
    float ckp_rpm;

    output_id_t output_pin;

}indication_ctx_t;

error_t indication_init(indication_ctx_t *ctx, const indication_init_ctx_t *init_ctx);
error_t indication_configure(indication_ctx_t *ctx, const indication_config_t *config);
error_t indication_reset(indication_ctx_t *ctx);

void indication_ckp_signal_update(indication_ctx_t *ctx, const ckp_data_t *data, const ckp_diag_t *diag);
void indication_ignpower_update(indication_ctx_t *ctx, bool ignpower_on);

error_t indication_get_data(indication_ctx_t *ctx, indication_data_t *data);
error_t indication_get_diag(indication_ctx_t *ctx, indication_diag_t *diag);

error_t indication_manual_reset(indication_ctx_t *ctx);
error_t indication_manual_set(indication_ctx_t *ctx, bool manual_enabled);

error_t indication_force_reset(indication_ctx_t *ctx);
error_t indication_force_set(indication_ctx_t *ctx, bool force_enabled);

void indication_loop_main(indication_ctx_t *ctx);
void indication_loop_slow(indication_ctx_t *ctx);
void indication_loop_fast(indication_ctx_t *ctx);


#endif /* MODULES_INDICATION_INC_INDICATION_H_ */
