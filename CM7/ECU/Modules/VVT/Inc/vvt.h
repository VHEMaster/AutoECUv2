/*
 * vvt.h
 *
 *  Created on: Jun 9, 2024
 *      Author: VHEMaster
 */

#ifndef MODULES_VVT_INC_VVT_H_
#define MODULES_VVT_INC_VVT_H_

#include "common.h"
#include "versioned_vvt.h"
#include "config_gpio.h"
#include "time.h"

#define ECU_VVT_PWM_PERIOD    (250)

typedef union {
    uint32_t data;
    struct {

    }bits;
}vvt_diag_t;

typedef struct {
    bool enabled;
    float dutycycle_target;
    float dutycycle_current;
    float pos_target;
    float pos_current;
}vvt_data_t;

typedef struct {
    uint32_t dummy;
}vvt_init_ctx_t;

typedef struct {
    vvt_init_ctx_t init;
    vvt_config_t config;
    bool ready;
    bool configured;
    bool pwm_pin_locked;

    vvt_diag_t diag;
    vvt_data_t data;

    cmp_data_t cmp_data;
    cmp_diag_t cmp_diag;

    math_pid_ctx_t pid_position;
    math_pid_ctx_t pid_speed;

    input_id_t power_voltage_pin;
    output_id_t output_pwm_pin;

}vvt_ctx_t;

error_t vvt_init(vvt_ctx_t *ctx, const vvt_init_ctx_t *init_ctx);
error_t vvt_configure(vvt_ctx_t *ctx, const vvt_config_t *config);
error_t vvt_reset(vvt_ctx_t *ctx);

void vvt_cmp_signal_update(vvt_ctx_t *ctx, const cmp_data_t *data, const cmp_diag_t *diag);

error_t vvt_get_data(vvt_ctx_t *ctx, vvt_data_t *data);
error_t vvt_get_diag(vvt_ctx_t *ctx, vvt_diag_t *diag);

error_t vvt_set_enabled(vvt_ctx_t *ctx, bool enabled);
error_t vvt_set_target_position(vvt_ctx_t *ctx, float position);
error_t vvt_set_target_dutycycle(vvt_ctx_t *ctx, float dutycycle);

void vvt_loop_main(vvt_ctx_t *ctx);
void vvt_loop_slow(vvt_ctx_t *ctx);
void vvt_loop_fast(vvt_ctx_t *ctx);


#endif /* MODULES_VVT_INC_VVT_H_ */
