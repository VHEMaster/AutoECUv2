/*
 * tle4729.c
 *
 *  Created on: Apr 6, 2024
 *      Author: VHEMaster
 */

#include "tle4729.h"
#include "compiler.h"
#include "interpolation.h"
#include <math.h>
#include <string.h>

ITCM_FUNC  STATIC_INLINE void tle4729_set_mode(tle4729_ctx_t *ctx, tle4729_mode_t mode)
{
  uint32_t prim;

  if(ctx->last_mode != mode) {
    ctx->last_mode = mode;
    ctx->current_mode = mode;

    prim = EnterCritical();
    gpio_write(&ctx->init.i10, mode & 1);
    gpio_write(&ctx->init.i11, mode & 2);
    gpio_write(&ctx->init.i20, mode & 1);
    gpio_write(&ctx->init.i21, mode & 2);
    ExitCritical(prim);
  }
}

ITCM_FUNC  STATIC_INLINE void tle4729_step_increment(tle4729_ctx_t *ctx)
{
  bool ph1;
  bool ph2;
  uint32_t prim;
  uint8_t current_step;

  if(ctx->enabled == true) {
    if(ctx->current_speed > 0) {
      ctx->pos_current++;
    } else if(ctx->current_speed < 0) {
      ctx->pos_current--;
    }
    current_step = ctx->pos_current & 3;

    if(ctx->init.ph1.port == ctx->init.ph2.port) {
      ctx->init.ph1.port->BSRR = ctx->init.ph_bsrr[current_step];
    } else {
      ph1 = (ctx->init.ph_bsrr[current_step] & ctx->init.ph1.pin) > 0;
      ph2 = (ctx->init.ph_bsrr[current_step] & ctx->init.ph2.pin) > 0;
      prim = EnterCritical();
      gpio_write(&ctx->init.ph1, ph1);
      gpio_write(&ctx->init.ph2, ph2);
      ExitCritical(prim);
    }

    ctx->current_step = current_step;
  }
}

ITCM_FUNC  STATIC_INLINE void tle4729_calculate_next_move(tle4729_ctx_t *ctx)
{
  float acceleration_steps = ctx->config.acceleration_steps;
  float r_acceleration_steps = 1.0f / acceleration_steps;
  float speed = ctx->current_speed;
  float step_time_mult = 1.0f;
  float pos_accel_add;
  time_us_t step_time_us;
  sMathInterpolateInput mii_step_time_mult, mii_step_time_ms;

  pos_accel_add = speed * acceleration_steps;

  if(ctx->pos_target - ctx->pos_current > pos_accel_add) {
    speed += r_acceleration_steps;
  } else if(ctx->pos_target - ctx->pos_current < pos_accel_add) {
    speed -= r_acceleration_steps;
  } else {

  }

  speed = CLAMP(speed, -1.0f, 1.0f);
  if(fabsf(speed) < (r_acceleration_steps * 0.5f)) {
    if(ctx->pos_target == ctx->pos_current) {
      ctx->moving = false;
      speed = 0;
    }
  } else {
    ctx->moving = true;
  }

  mii_step_time_mult = math_interpolate_input(ctx->pwr_voltage, ctx->config.voltage_to_step_time_mult.input, ctx->config.voltage_to_step_time_mult.items);
  step_time_mult = math_interpolate_1d(mii_step_time_mult, ctx->config.voltage_to_step_time_mult.output);

  mii_step_time_ms = math_interpolate_input(fabsf(speed), ctx->config.speed_to_step_time_ms.input, ctx->config.speed_to_step_time_ms.items);
  step_time_us = math_interpolate_1d(mii_step_time_ms, ctx->config.speed_to_step_time_ms.output) * TIME_US_IN_MS;
  step_time_us *= step_time_mult;

  ctx->current_step_time = step_time_us;
  ctx->current_speed = speed;
}

error_t tle4729_init(tle4729_ctx_t *ctx, const tle4729_init_ctx_t *init_ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || init_ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(gpio_valid(&init_ctx->i10) == false, err = E_PARAM);
    BREAK_IF_ACTION(gpio_valid(&init_ctx->i11) == false, err = E_PARAM);
    BREAK_IF_ACTION(gpio_valid(&init_ctx->i20) == false, err = E_PARAM);
    BREAK_IF_ACTION(gpio_valid(&init_ctx->i21) == false, err = E_PARAM);
    BREAK_IF_ACTION(gpio_valid(&init_ctx->ph1) == false, err = E_PARAM);
    BREAK_IF_ACTION(gpio_valid(&init_ctx->ph2) == false, err = E_PARAM);
    BREAK_IF_ACTION(gpio_valid(&init_ctx->error1) == false, err = E_PARAM);
    BREAK_IF_ACTION(gpio_valid(&init_ctx->error2) == false, err = E_PARAM);

    if(init_ctx != &ctx->init) {
      memcpy(&ctx->init, init_ctx, sizeof(tle4729_init_ctx_t));
    }

    ctx->init.ph_bsrr[0] = (ctx->init.ph1.pin | ctx->init.ph2.pin) << 16;
    ctx->init.ph_bsrr[1] = ctx->init.ph1.pin | (ctx->init.ph2.pin << 16);
    ctx->init.ph_bsrr[2] = ctx->init.ph1.pin | ctx->init.ph2.pin;
    ctx->init.ph_bsrr[3] = (ctx->init.ph1.pin << 16) | ctx->init.ph2.pin;

  } while(0);

  return err;
}

error_t tle4729_reset(tle4729_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);

    // Nothing to do here

  } while(0);

  return err;
}


error_t tle4729_configure(tle4729_ctx_t *ctx, const tle4729_config_t *config_ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || config_ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(config_ctx->pos_max <= config_ctx->pos_min, err = E_PARAM);
    BREAK_IF_ACTION(config_ctx->voltage_to_step_time_mult.items == 0, err = E_PARAM);
    BREAK_IF_ACTION(config_ctx->speed_to_step_time_ms.items == 0, err = E_PARAM);

    if(config_ctx != &ctx->config) {
      memcpy(&ctx->config, config_ctx, sizeof(tle4729_config_t));
    }

  } while(0);

  return err;
}
void tle4729_loop_main(tle4729_ctx_t *ctx)
{

}

void tle4729_loop_slow(tle4729_ctx_t *ctx)
{
  error_t err = E_OK;
  bool error_state = false;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);

    error_state |= !gpio_read(&ctx->init.error1);
    error_state |= !gpio_read(&ctx->init.error2);

    ctx->failure = error_state;

  } while(0);

  if(err != E_OK && err != E_AGAIN) {
    //TODO: set DTC here??
  }
}

ITCM_FUNC  void tle4729_loop_fast(tle4729_ctx_t *ctx)
{
  error_t err = E_OK;
  time_us_t now;
  tle4729_mode_t mode = ctx->current_mode;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);

    now = time_get_current_us();

    if(ctx->enabled) {
      if(ctx->moving == false) {
        if(ctx->pos_target != ctx->pos_current) {
          tle4729_calculate_next_move(ctx);
          tle4729_step_increment(ctx);
          ctx->current_step_last = now;
        }
      } else {
        if(time_diff(now, ctx->current_step_last) >= ctx->current_step_time) {
          tle4729_calculate_next_move(ctx);
          tle4729_step_increment(ctx);
          ctx->current_step_last = now;
        }
      }

      if(ctx->moving == true) {
        mode = TLE4729_MODE_ACCELERATE;
        ctx->last_moving = now;
      } else {
        if(mode == TLE4729_MODE_ACCELERATE) {
          if(time_diff(now, ctx->last_moving) >= TLE4729_ACC_TO_NORM_TRANS_DELAY_US) {
            mode = TLE4729_MODE_NORMAL;
          }
        } else if(mode == TLE4729_MODE_NORMAL) {
          if(time_diff(now, ctx->last_moving) >= TLE4729_ACC_TO_NORM_TRANS_DELAY_US + TLE4729_NORM_TO_HOLD_TRANS_DELAY_US) {
            mode = TLE4729_MODE_HOLD;
          }
        } else if(mode == TLE4729_MODE_STBY) {
          mode = TLE4729_MODE_HOLD;
        }
      }
    } else {
      mode = TLE4729_MODE_STBY;
    }

    tle4729_set_mode(ctx, mode);

  } while(0);

  if(err != E_OK && err != E_AGAIN) {
    //TODO: set DTC here??
  }
}

error_t tle4729_enable(tle4729_ctx_t *ctx, bool enabled)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);

    ctx->enabled = enabled;

  } while(0);

  return err;
}

error_t tle4729_set_pwr_voltage(tle4729_ctx_t *ctx, float pwr_voltage)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);

    ctx->pwr_voltage = pwr_voltage;

  } while(0);

  return err;
}

error_t tle4729_set_target(tle4729_ctx_t *ctx, int32_t position)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);

    ctx->pos_target = CLAMP(position, ctx->config.pos_min, ctx->config.pos_max);

  } while(0);

  return err;
}

error_t tle4729_get_target(tle4729_ctx_t *ctx, int32_t *position)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || position, err = E_PARAM);

    *position = ctx->pos_target;

  } while(0);

  return err;
}

error_t tle4729_set_current(tle4729_ctx_t *ctx, int32_t position)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);

    ctx->pos_current = CLAMP(position, ctx->config.pos_min, ctx->config.pos_max);

  } while(0);

  return err;
}

error_t tle4729_get_current(tle4729_ctx_t *ctx, int32_t *position)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || position, err = E_PARAM);

    *position = ctx->pos_current;

  } while(0);

  return err;
}

error_t tle4729_pos_reset(tle4729_ctx_t *ctx, int32_t position)
{
  error_t err = E_OK;
  uint32_t prim;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);

    position = CLAMP(position, ctx->config.pos_min, ctx->config.pos_max);

    prim = EnterCritical();
    ctx->pos_target = position;
    ctx->pos_current = position;
    ExitCritical(prim);

  } while(0);

  return err;
}

error_t tle4729_is_failure(tle4729_ctx_t *ctx, bool *failure)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || failure, err = E_PARAM);

    *failure = ctx->failure;

  } while(0);

  return err;
}
