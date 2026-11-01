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
#include "config_map.h"
#include "time.h"

#define ECU_WGCV_PWM_PERIOD    (250)
#define ECU_WGCV_BOOST_UPDATE_TIMEOUT_US    (100 * TIME_US_IN_MS)

typedef union {
    uint32_t data;
    struct {
        bool output_drive_error : 1;
        bool map_handle_error : 1;
        bool map_diag_error : 1;
        bool map_poll_error : 1;
    }bits;
}wgcv_diag_t;

typedef struct {
    bool enabled;
    float pwm_dutycycle;
    float input_dutycycle;
    float pid_dutycycle;
    bool force_input_engaged;
    float force_input_dutycycle;
    bool force_pwm_engaged;
    float force_pwm_dutycycle;

    float target_boost;
    float actual_boost;
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

    map_data_t map_data;
    map_diag_t map_diag;

    float pwm_dutycycle_prev;
    float power_voltage;

    bool actual_boost_updated;

    math_pid_ctx_t pid_boost;
    math_pid_ctx_t pid_speed;

    float boost_current;
    float boost_prev;
    float current_speed;
    float target_speed;

    input_id_t power_voltage_pin;
    output_id_t output_pwm_pin;
    time_us_t process_time;
    time_us_t boost_update_last;

}wgcv_ctx_t;

error_t wgcv_init(wgcv_ctx_t *ctx, const wgcv_init_ctx_t *init_ctx);
error_t wgcv_configure(wgcv_ctx_t *ctx, const wgcv_config_t *config);
error_t wgcv_reset(wgcv_ctx_t *ctx);

void wgcv_cmp_signal_update(wgcv_ctx_t *ctx, const cmp_data_t *data, const cmp_diag_t *diag);

error_t wgcv_get_data(wgcv_ctx_t *ctx, wgcv_data_t *data);
error_t wgcv_get_diag(wgcv_ctx_t *ctx, wgcv_diag_t *diag);

error_t wgcv_set_enabled(wgcv_ctx_t *ctx, bool enabled);
error_t wgcv_set_dutycycle(wgcv_ctx_t *ctx, float dutycycle);

error_t wgcv_set_target_boost(wgcv_ctx_t *ctx, float target_boost);

error_t wgcv_force_input_reset(wgcv_ctx_t *ctx);
error_t wgcv_force_input_set(wgcv_ctx_t *ctx, float dutycycle);
error_t wgcv_force_pwm_reset(wgcv_ctx_t *ctx);
error_t wgcv_force_pwm_set(wgcv_ctx_t *ctx, float dutycycle);

void wgcv_loop_main(wgcv_ctx_t *ctx);
void wgcv_loop_slow(wgcv_ctx_t *ctx);
void wgcv_loop_fast(wgcv_ctx_t *ctx);


#endif /* MODULES_WGCV_INC_WGCV_H_ */
