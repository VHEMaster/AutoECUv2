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

typedef struct {
    ignition_signal_update_cb_t signal_update_cb;
    void *signal_update_usrdata;
}ignition_init_ctx_t;

typedef struct {
    ignition_init_ctx_t init;
    ignition_config_t config;
    bool ready;
    bool configured;

    bool ignition_active;

    bool input_pin_locked;

    input_id_t input_pin;
    time_us_t status_time;

}ignition_ctx_t;

error_t ignition_init(ignition_ctx_t *ctx, const ignition_init_ctx_t *init_ctx);
error_t ignition_configure(ignition_ctx_t *ctx, const ignition_config_t *config);
error_t ignition_reset(ignition_ctx_t *ctx);

error_t ignition_is_active(ignition_ctx_t *ctx, bool *ignition_active);

void ignition_loop_main(ignition_ctx_t *ctx);
void ignition_loop_slow(ignition_ctx_t *ctx);
void ignition_loop_fast(ignition_ctx_t *ctx);


#endif /* MODULES_IGNITION_INC_IGNITION_H_ */
