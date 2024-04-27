/*
 * tle4729.h
 *
 *  Created on: Mar 19, 2024
 *      Author: VHEMaster
 */

#ifndef DRIVERS_TLE4729G_INC_TLE4729_H_
#define DRIVERS_TLE4729G_INC_TLE4729_H_

#include "errors.h"
#include "time.h"
#include "gpio.h"

#define TLE4729_RELATION_ITEMS_MAX    (16)
#define TLE4729_ACC_TO_NORM_TRANS_DELAY_US    (200 * TIME_US_IN_MS)
#define TLE4729_NORM_TO_HOLD_TRANS_DELAY_US   (600 * TIME_US_IN_MS)

typedef enum {
  TLE4729_MODE_STBY = 0,
  TLE4729_MODE_HOLD,
  TLE4729_MODE_NORMAL,
  TLE4729_MODE_ACCELERATE,
  TLE4729_MODE_MAX
}tle4729_mode_t;

typedef struct {
    uint8_t items;
    float input[TLE4729_RELATION_ITEMS_MAX];
    float output[TLE4729_RELATION_ITEMS_MAX];
}tle7429_config_relation_t;

typedef struct {
    gpio_t i10;
    gpio_t i11;
    gpio_t i20;
    gpio_t i21;
    gpio_t ph1;
    gpio_t ph2;
    gpio_t error1;
    gpio_t error2;

    uint32_t ph_bsrr[4];

}tle4729_init_ctx_t;

typedef struct {
    tle7429_config_relation_t voltage_to_step_time_mult;
    tle7429_config_relation_t speed_to_step_time_ms;
    uint8_t acceleration_steps;

    int32_t pos_min;
    int32_t pos_max;

}tle4729_config_t;

typedef struct {
    tle4729_init_ctx_t init;
    tle4729_config_t config;
    bool enabled;
    bool failure;
    bool moving;
    float pwr_voltage;
    uint8_t current_step;
    tle4729_mode_t current_mode;
    float current_speed;
    time_delta_us_t current_step_time;
    time_us_t current_step_last;
    time_us_t last_moving;
    tle4729_mode_t last_mode;

    int32_t pos_target;
    int32_t pos_current;

}tle4729_ctx_t;

error_t tle4729_init(tle4729_ctx_t *ctx, const tle4729_init_ctx_t *init_ctx);
error_t tle4729_configure(tle4729_ctx_t *ctx, const tle4729_config_t *config_ctx);
void tle4729_loop_main(tle4729_ctx_t *ctx);
void tle4729_loop_slow(tle4729_ctx_t *ctx);
void tle4729_loop_fast(tle4729_ctx_t *ctx);

error_t tle4729_enable(tle4729_ctx_t *ctx, bool enabled);

error_t tle4729_set_pwr_voltage(tle4729_ctx_t *ctx, float pwr_voltage);

error_t tle4729_set_target(tle4729_ctx_t *ctx, int32_t position);
error_t tle4729_get_target(tle4729_ctx_t *ctx, int32_t *position);
error_t tle4729_set_current(tle4729_ctx_t *ctx, int32_t position);
error_t tle4729_get_current(tle4729_ctx_t *ctx, int32_t *position);
error_t tle4729_is_failure(tle4729_ctx_t *ctx, bool *failure);

#endif /* DRIVERS_TLE4729G_INC_TLE4729_H_ */
