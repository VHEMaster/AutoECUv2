/*
 * ignpower.h
 *
 *  Created on: Jun 21, 2024
 *      Author: VHEMaster
 */

#ifndef MODULES_IGNPOWER_INC_IGNPOWER_H_
#define MODULES_IGNPOWER_INC_IGNPOWER_H_

#include "common.h"
#include "versioned_ignpower.h"
#include "config_gpio.h"
#include "time.h"

typedef void (*ignpower_signal_update_cb_t)(void *usrdata, bool ignpower_active);

typedef union {
    uint32_t data;
    struct {
        bool output_relay_error : 1;
        bool input_signal_error : 1;
    }bits;
}ignpower_diag_t;

typedef struct {
    bool enabled;

    bool ignpower_active;
    bool output_active;

    bool components_trigger;
    bool crankshaft_trigger;

    bool components_operating;
    bool crankshaft_operating;
}ignpower_data_t;

typedef struct {
    ignpower_signal_update_cb_t signal_update_cb;
    void *signal_update_usrdata;
}ignpower_init_ctx_t;

typedef struct {
    ignpower_init_ctx_t init;
    ignpower_config_t config;
    bool ready;
    bool configured;

    bool output_active_prev;

    bool input_signal_pin_locked;
    bool output_relay_pin_locked;
    bool min_delay_elapsed;
    bool min_operation_elapsed;
    bool min_crankshaft_elapsed;

    ignpower_diag_t diag;
    ignpower_data_t data;

    input_id_t input_signal_pin;
    output_id_t output_relay_pin;
    time_us_t status_time;
    time_us_t output_time;
    time_us_t operating_time;
    time_us_t crankshaft_time;

}ignpower_ctx_t;

error_t ignpower_init(ignpower_ctx_t *ctx, const ignpower_init_ctx_t *init_ctx);
error_t ignpower_configure(ignpower_ctx_t *ctx, const ignpower_config_t *config);
error_t ignpower_reset(ignpower_ctx_t *ctx);


void ignpower_ckp_signal_update(ignpower_ctx_t *ctx, const ckp_data_t *data, const ckp_diag_t *diag);
error_t ignpower_is_active(ignpower_ctx_t *ctx, bool *ignpower_active);

error_t ignpower_get_data(ignpower_ctx_t *ctx, ignpower_data_t *data);
error_t ignpower_get_diag(ignpower_ctx_t *ctx, ignpower_diag_t *diag);

void ignpower_loop_main(ignpower_ctx_t *ctx);
void ignpower_loop_slow(ignpower_ctx_t *ctx);
void ignpower_loop_fast(ignpower_ctx_t *ctx);


#endif /* MODULES_IGNPOWER_INC_IGNPOWER_H_ */
