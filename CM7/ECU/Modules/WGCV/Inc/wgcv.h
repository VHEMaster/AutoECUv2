/*
 * wgcv.h
 *
 *  Created on: Jun 9, 2024
 *      Author: VHEMaster
 */

#ifndef MODULES_WGCV_INC_WGCV_H_
#define MODULES_WGCV_INC_WGCV_H_

#include "common.h"
#include "versioned_wgcv.h"
#include "config_gpio.h"
#include "time.h"

#define ECU_WGCV_PWM_PERIOD    (250)

typedef union {
    uint32_t data;
    struct {

    }bits;
}wgcv_diag_t;

typedef struct {
    bool enabled;
    float pwm_dutycycle;
    float input_dutycycle;
}wgcv_data_t;

typedef struct {
    uint32_t dummy;
}wgcv_init_ctx_t;

typedef struct {
    wgcv_init_ctx_t init;
    wgcv_config_t config;
    bool ready;
    bool configured;
    bool pwm_pin_locked;

    wgcv_diag_t diag;
    wgcv_data_t data;

    math_pid_ctx_t pid_position;
    math_pid_ctx_t pid_speed;

    input_id_t power_voltage_pin;
    output_id_t output_pwm_pin;

}wgcv_ctx_t;

error_t wgcv_init(wgcv_ctx_t *ctx, const wgcv_init_ctx_t *init_ctx);
error_t wgcv_configure(wgcv_ctx_t *ctx, const wgcv_config_t *config);
error_t wgcv_reset(wgcv_ctx_t *ctx);

void wgcv_cmp_signal_update(wgcv_ctx_t *ctx, const cmp_data_t *data, const cmp_diag_t *diag);

error_t wgcv_get_data(wgcv_ctx_t *ctx, wgcv_data_t *data);
error_t wgcv_get_diag(wgcv_ctx_t *ctx, wgcv_diag_t *diag);

error_t wgcv_set_enabled(wgcv_ctx_t *ctx, bool enabled);
error_t wgcv_set_dutycycle(wgcv_ctx_t *ctx, float dutycycle);

void wgcv_loop_main(wgcv_ctx_t *ctx);
void wgcv_loop_slow(wgcv_ctx_t *ctx);
void wgcv_loop_fast(wgcv_ctx_t *ctx);


#endif /* MODULES_WGCV_INC_WGCV_H_ */
