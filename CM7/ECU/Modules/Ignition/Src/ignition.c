/*
 * ignition.c
 *
 *  Created on: Jun 21, 2024
 *      Author: VHEMaster
 */

#include "ignition.h"
#include "compiler.h"
#include <string.h>

error_t ignition_init(ignition_ctx_t *ctx, const ignition_init_ctx_t *init_ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || init_ctx == NULL, err = E_PARAM);

    memset(ctx, 0u, sizeof(ignition_ctx_t));
    memcpy(&ctx->init, init_ctx, sizeof(ignition_init_ctx_t));

    ctx->ready = true;

  } while(0);

  return err;
}

error_t ignition_configure(ignition_ctx_t *ctx, const ignition_config_t *config)
{
  error_t err = E_OK;
  time_us_t now;
  bool valid;

  do {
    BREAK_IF_ACTION(ctx == NULL || config == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    if(ctx->configured != false) {
      err = ignition_reset(ctx);
      BREAK_IF(err != E_OK);
    }

    ctx->configured = false;

    if(&ctx->config != config) {
      memcpy(&ctx->config, config, sizeof(ignition_config_t));
    }

    if(ctx->config.enabled == true) {
      err = ecu_config_gpio_input_valid(ctx->config.input_pin, &valid);
      BREAK_IF(err != E_OK);
      BREAK_IF_ACTION(valid != true, err = E_PARAM);

      if(valid) {
        err = ecu_config_gpio_input_lock(ctx->config.input_pin);
        BREAK_IF(err != E_OK);
        ctx->input_pin_locked = true;

        err = ecu_config_gpio_input_set_mode(ctx->config.input_pin, ECU_GPIO_INPUT_TYPE_DIGITAL);
        BREAK_IF(err != E_OK);

        err = ecu_config_gpio_input_get_id(ctx->config.input_pin, &ctx->input_pin);
        BREAK_IF(err != E_OK);
      }

      now = time_get_current_us();
      ctx->status_time = now;

      ctx->configured = true;
    }

  } while(0);

  if(err != E_OK) {
    (void)ignition_reset(ctx);
  }

  return err;
}

error_t ignition_reset(ignition_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    if(ctx->input_pin_locked != false) {
      (void)ecu_config_gpio_input_unlock(ctx->config.input_pin);
      ctx->input_pin_locked = false;
    }
    ctx->configured = false;

  } while(0);

  return err;
}

void ignition_loop_slow(ignition_ctx_t *ctx)
{
  time_delta_us_t time_debounce;
  input_value_t input_value;
  bool input_bool;
  time_us_t now;
  error_t err;

  if(ctx->ready) {
    if(ctx->configured) {
      now = time_get_current_us();

      input_bool = ctx->ignition_active;
      err = input_get_value(ctx->input_pin, &input_value, NULL);
      if(err == E_OK) {
        input_bool = input_value ? true : false;
        input_bool ^= ctx->config.input_gpio_inverted;
      } else if(err != E_AGAIN) {

      }

      if(input_bool != ctx->ignition_active) {
        if(input_bool) {
          time_debounce = ctx->config.input_debounce_off_to_on;
        } else {
          time_debounce = ctx->config.input_debounce_on_to_off;
        }

        if(time_diff(now, ctx->status_time) >= time_debounce) {
          ctx->ignition_active = input_bool;
          ctx->status_time = now;
          if(ctx->init.signal_update_cb) {
            ctx->init.signal_update_cb(ctx->init.signal_update_usrdata, ctx->ignition_active);
          }
        }
      } else {
        ctx->status_time = now;
      }
    }
  }
}

error_t ignition_is_active(ignition_ctx_t *ctx, bool *ignition_active)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ignition_active == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    *ignition_active = ctx->ignition_active;

  } while(0);

  return err;
}
