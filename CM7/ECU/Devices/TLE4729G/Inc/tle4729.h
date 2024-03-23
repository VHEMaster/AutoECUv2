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

#include "../../../Drivers/Spi/Inc/spi.h"

#define MAX31855_DEFAULT_POLL_PERIOD_US   (100 * TIME_MS_TO_US)

typedef enum {
  TLE4729_MODE_STBY = 0,
  TLE4729_MODE_HOLD,
  TLE4729_MODE_NORMAL,
  TLE4729_MODE_ACCELERATE
}tle4729_mode_t;

typedef struct {
    gpio_t i10;
    gpio_t i11;
    gpio_t i20;
    gpio_t i21;
    gpio_t ph1;
    gpio_t ph2;
    gpio_t error1;
    gpio_t error2;
    bool failure;

    int32_t pos_min;
    int32_t pos_max;
    int32_t pos_target;
    int32_t pos_current;
    time_delta_us_t us_per_step;
    tle4729_mode_t mode;
}tle4729_ctx_t;

error_t tle4729_init(tle4729_ctx_t *ctx);
error_t tle4729_loop_main(tle4729_ctx_t *ctx);
error_t tle4729_loop_slow(tle4729_ctx_t *ctx);
error_t tle4729_loop_fast(tle4729_ctx_t *ctx);

error_t tle4729_enable(tle4729_ctx_t *ctx, bool enabled);
error_t tle4729_set_clamp(tle4729_ctx_t *ctx, int32_t pos_min, int32_t pos_max);
error_t tle4729_set_mode(tle4729_ctx_t *ctx, tle4729_mode_t mode);
error_t tle4729_set_us_per_step(tle4729_ctx_t *ctx, time_delta_us_t us_per_step);

error_t tle4729_set_target(tle4729_ctx_t *ctx, int32_t position);
error_t tle4729_set_current(tle4729_ctx_t *ctx, int32_t position);
bool tle4729_is_moving(tle4729_ctx_t *ctx);
bool tle4729_is_failure(tle4729_ctx_t *ctx);

#endif /* DRIVERS_TLE4729G_INC_TLE4729_H_ */
