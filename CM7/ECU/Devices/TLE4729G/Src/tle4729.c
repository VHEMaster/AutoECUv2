/*
 * tle4729.c
 *
 *  Created on: Apr 6, 2024
 *      Author: VHEMaster
 */

#include "tle4729.h"
#include "compiler.h"
#include <string.h>

STATIC_INLINE void tle4729_set_mode(tle4729_ctx_t *ctx, tle4729_mode_t mode)
{
  uint32_t prim;

  if(ctx->current_mode != mode) {
    ctx->current_mode = mode;

    prim = EnterCritical();
    gpio_write(&ctx->init.i10, mode & 1);
    gpio_write(&ctx->init.i11, mode & 2);
    gpio_write(&ctx->init.i20, mode & 1);
    gpio_write(&ctx->init.i21, mode & 2);
    ExitCritical(prim);
  }
}

STATIC_INLINE void tle4729_step_increment(tle4729_ctx_t *ctx)
{
  bool ph1;
  bool ph2;
  uint32_t prim;
  uint8_t current_step = ctx->current_step;

  if(ctx->enabled == true) {
    if(ctx->current_speed > 0) {
      current_step = (current_step + 1) & 0x3;
      ctx->pos_current++;
    } else if(ctx->current_speed < 0) {
      current_step = (current_step - 1) & 0x3;
      ctx->pos_current--;
    }

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

error_t tle4729_configure(tle4729_ctx_t *ctx, const tle4729_config_t *config_ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || config_ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(config_ctx->pos_max <= config_ctx->pos_min, err = E_PARAM);
    BREAK_IF_ACTION(config_ctx->voltage_to_acceleration_steps.items == 0, err = E_PARAM);
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

void tle4729_loop_fast(tle4729_ctx_t *ctx)
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

          ctx->current_step_last = now;
          ctx->moving = true;
        }
      } else {
        if(time_diff(now, ctx->current_step_last) >= ctx->current_step_time) {
          tle4729_step_increment(ctx);

        }
      }

      if(ctx->moving == true) {
        mode = TLE4729_MODE_ACCELERATE;
      } else {
        mode = TLE4729_MODE_NORMAL;
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
  error_t err = E_NOTSUPPORT;

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
  error_t err = E_NOTSUPPORT;

  return err;
}

error_t tle4729_get_target(tle4729_ctx_t *ctx, int32_t *position)
{
  error_t err = E_NOTSUPPORT;

  return err;
}

error_t tle4729_set_current(tle4729_ctx_t *ctx, int32_t position)
{
  error_t err = E_NOTSUPPORT;

  return err;
}

error_t tle4729_get_current(tle4729_ctx_t *ctx, int32_t *position)
{
  error_t err = E_NOTSUPPORT;

  return err;
}

error_t tle4729_is_failure(tle4729_ctx_t *ctx, bool *failure)
{
  error_t err = E_NOTSUPPORT;

  return err;
}
