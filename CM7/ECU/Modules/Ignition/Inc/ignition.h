/*
 * ignition.h
 *
 *  Created on: Jun 21, 2024
 *      Author: VHEMaster
 */

#ifndef MODULES_IGNITION_INC_IGNITION_H_
#define MODULES_IGNITION_INC_IGNITION_H_

#include "common.h"
#include "versioned_ignition.h"
#include "config_gpio.h"
#include "time.h"

typedef void (*ignition_signal_update_cb_t)(void *usrdata, bool ignition_active);

typedef union {
    uint32_t data;
    struct {
        bool output_relay_error : 1;
        bool input_signal_error : 1;
    }bits;
}ignition_diag_t;

typedef struct {
    bool enabled;

    bool ignition_active;
    bool output_active;

    bool components_trigger;
    bool crankshaft_trigger;

    bool components_operating;
    bool crankshaft_operating;
}ignition_data_t;

typedef struct {
    ignition_signal_update_cb_t signal_update_cb;
    void *signal_update_usrdata;
}ignition_init_ctx_t;

typedef struct {
    ignition_init_ctx_t init;
    ignition_config_t config;
    bool ready;
    bool configured;

    bool output_active_prev;

    bool input_signal_pin_locked;
    bool output_relay_pin_locked;
    bool min_delay_elapsed;
    bool min_operation_elapsed;
    bool min_crankshaft_elapsed;

    ignition_diag_t diag;
    ignition_data_t data;

    input_id_t input_signal_pin;
    output_id_t output_relay_pin;
    time_us_t status_time;
    time_us_t output_time;
    time_us_t operating_time;
    time_us_t crankshaft_time;

}ignition_ctx_t;

error_t ignition_init(ignition_ctx_t *ctx, const ignition_init_ctx_t *init_ctx);
error_t ignition_configure(ignition_ctx_t *ctx, const ignition_config_t *config);
error_t ignition_reset(ignition_ctx_t *ctx);


void ignition_ckp_signal_update(ignition_ctx_t *ctx, const ckp_data_t *data, const ckp_diag_t *diag);
error_t ignition_is_active(ignition_ctx_t *ctx, bool *ignition_active);

error_t ignition_get_data(ignition_ctx_t *ctx, ignition_data_t *data);
error_t ignition_get_diag(ignition_ctx_t *ctx, ignition_diag_t *diag);

void ignition_loop_main(ignition_ctx_t *ctx);
void ignition_loop_slow(ignition_ctx_t *ctx);
void ignition_loop_fast(ignition_ctx_t *ctx);


#endif /* MODULES_IGNITION_INC_IGNITION_H_ */
