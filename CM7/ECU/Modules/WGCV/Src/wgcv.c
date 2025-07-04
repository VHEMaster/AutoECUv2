/*
 * wgcv.c
 *
 *  Created on: Jun 9, 2024
 *      Author: VHEMaster
 */

#include "wgcv.h"
#include "compiler.h"
#include "interpolation.h"
#include <string.h>

error_t wgcv_init(wgcv_ctx_t *ctx, const wgcv_init_ctx_t *init_ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || init_ctx == NULL, err = E_PARAM);

    memset(ctx, 0u, sizeof(wgcv_ctx_t));
    memcpy(&ctx->init, init_ctx, sizeof(wgcv_init_ctx_t));

    math_pid_init(&ctx->pid_boost);
    math_pid_init(&ctx->pid_speed);

    ctx->ready = true;

  } while(0);

  return err;
}

error_t wgcv_configure(wgcv_ctx_t *ctx, const wgcv_config_t *config)
{
  error_t err = E_OK;
  ecu_gpio_output_if_pwm_cfg_t pwm_config;
  time_us_t now;

  do {
    BREAK_IF_ACTION(ctx == NULL || config == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    if(ctx->configured != false) {
      err = wgcv_reset(ctx);
      BREAK_IF(err != E_OK);
    }

    ctx->configured = false;

    if(&ctx->config != config) {
      memcpy(&ctx->config, config, sizeof(wgcv_config_t));
    }

    if(ctx->config.enabled == true) {
      now = time_now_us();

      err = ecu_config_gpio_input_get_id(ctx->config.power_voltage_pin, &ctx->power_voltage_pin);
      BREAK_IF(err != E_OK);

      err = ecu_config_gpio_output_lock(ctx->config.output_pwm_pin);
      BREAK_IF(err != E_OK);
      ctx->pwm_pin_locked = true;

      err = ecu_config_gpio_output_set_mode(ctx->config.output_pwm_pin, ECU_GPIO_TYPE_PWM);
      BREAK_IF(err != E_OK);

      pwm_config.frequency = ctx->config.pwm_freq;
      pwm_config.pwm_period = ECU_WGCV_PWM_PERIOD;

      err = ecu_config_gpio_output_pwm_configure(ctx->config.output_pwm_pin, &pwm_config);
      BREAK_IF(err != E_OK);

      ctx->process_time = now;
      ctx->configured = true;
    }

  } while(0);

  if(err != E_OK) {
    (void)wgcv_reset(ctx);
  }

  return err;
}

error_t wgcv_reset(wgcv_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    if(ctx->pwm_pin_locked != false) {
      (void)ecu_config_gpio_output_unlock(ctx->config.output_pwm_pin);
      ctx->pwm_pin_locked = false;
    }
    ctx->configured = false;

  } while(0);

  return err;
}

void wgcv_loop_slow(wgcv_ctx_t *ctx)
{
  error_t err = E_OK;
  time_us_t now;
  sMathInterpolateInput interpolate_input_min = {0};
  sMathInterpolateInput interpolate_input_max = {0};
  input_value_t input_analog_value;
  float input_dutycycle;
  float pid_dutycycle;
  float output_dutycycle;
  float power_voltage;
  float dutycycle_min;
  float dutycycle_max;
  bool pid_reset = false;
  bool sens_map_reset = false;
  time_delta_us_t time_delta;

  float pid_speed;
  float pid_current;

  if(ctx->ready) {
    if(ctx->configured) {
      now = time_now_us();
      time_delta = time_diff(now, ctx->process_time);
      ctx->process_time = now;

      (void)input_get_value(ctx->power_voltage_pin, &input_analog_value, NULL);
      power_voltage = (float)input_analog_value * INPUTS_ANALOG_MULTIPLIER_R;
      ctx->power_voltage = power_voltage;

      input_dutycycle = ctx->data.input_dutycycle;

      if(ctx->data.force_input_engaged) {
        input_dutycycle = ctx->data.force_input_dutycycle;
      } else {
        ctx->data.force_input_dutycycle = input_dutycycle;
      }

      if(ctx->config.sensor_map < ECU_SENSOR_MAP_MAX) {
        err = ecu_sensors_map_get_value(ctx->config.sensor_map, &ctx->map_data);
        if(err != E_OK && err != E_AGAIN) {
          ctx->diag.bits.map_handle_error = true;
        } else if(err == E_OK) {
          ctx->data.actual_boost = ctx->map_data.manifold_air_pressure - ctx->config.sensor_map_atmospheric_pressure;
          ctx->actual_boost_updated = true;
        }

        err = ecu_sensors_map_get_diag(ctx->config.sensor_map, &ctx->map_diag);
        if(err != E_OK && err != E_AGAIN) {
          ctx->diag.bits.map_handle_error = true;
        }

        if(ctx->map_diag.data != 0) {
          ctx->diag.bits.map_diag_error = true;
        } else {
          ctx->diag.bits.map_diag_error = false;
        }

        if(ctx->actual_boost_updated) {
          ctx->actual_boost_updated = false;
          ctx->boost_update_last = now;
          math_pid_set_koffs(&ctx->pid_boost, &ctx->config.pid_boost);
          math_pid_set_koffs(&ctx->pid_speed, &ctx->config.pid_speed);

          ctx->boost_prev = ctx->boost_current;
          ctx->boost_current = ctx->data.actual_boost;
          ctx->current_speed = (ctx->boost_current - ctx->boost_prev) / ((float)time_delta * 0.000001f);

          if(ctx->data.target_boost > 0.0f) {
            math_pid_set_target(&ctx->pid_boost, ctx->data.target_boost);
            pid_speed = math_pid_update(&ctx->pid_boost, ctx->boost_current, now);

            math_pid_set_target(&ctx->pid_speed, pid_speed);
            pid_current = math_pid_update(&ctx->pid_speed, ctx->current_speed, now);

            pid_current = CLAMP(pid_current, -1.0f, 1.0f);
          } else {
            pid_current = 0.0f;
            pid_reset = true;
          }
          ctx->data.pid_dutycycle = pid_current;
        } else if(time_diff(now, ctx->boost_update_last) >= ECU_WGCV_BOOST_UPDATE_TIMEOUT_US) {
          ctx->diag.bits.map_poll_error = true;
          sens_map_reset = true;
        }
      } else {
        sens_map_reset = true;
      }

      if(sens_map_reset) {
        ctx->boost_update_last = now;
        ctx->data.pid_dutycycle = 0.0f;
        ctx->data.actual_boost = 0.0f;
        pid_current = 0.0f;
        pid_reset = true;
      }

      pid_dutycycle = ctx->data.pid_dutycycle;

      input_dutycycle += pid_dutycycle;

      interpolate_input_min = math_interpolate_input(power_voltage, ctx->config.voltage_to_pwm_dutycycle.full_closed.input, ctx->config.voltage_to_pwm_dutycycle.full_closed.items);
      interpolate_input_max = math_interpolate_input(power_voltage, ctx->config.voltage_to_pwm_dutycycle.full_open.input, ctx->config.voltage_to_pwm_dutycycle.full_open.items);

      dutycycle_min = math_interpolate_1d(interpolate_input_min, ctx->config.voltage_to_pwm_dutycycle.full_closed.output);
      dutycycle_max = math_interpolate_1d(interpolate_input_max, ctx->config.voltage_to_pwm_dutycycle.full_open.output);

      if(input_dutycycle <= ctx->config.input_dutycycle_min) {
        output_dutycycle = ctx->config.pwm_dutycycle_min;
      } else if(input_dutycycle >= ctx->config.input_dutycycle_max) {
        output_dutycycle = ctx->config.pwm_dutycycle_max;
      } else {
        output_dutycycle = input_dutycycle;
        output_dutycycle *= dutycycle_max - dutycycle_min;
        output_dutycycle += dutycycle_min;
        output_dutycycle = CLAMP(output_dutycycle, dutycycle_min, dutycycle_max);
      }

      if(ctx->data.force_pwm_engaged) {
        output_dutycycle = ctx->data.force_pwm_dutycycle;
        pid_reset = true;
      } else {
        ctx->data.force_pwm_dutycycle = output_dutycycle;
      }

      ctx->data.pwm_dutycycle = output_dutycycle;

      if(ctx->pwm_dutycycle_prev != output_dutycycle) {
        if(ctx->pwm_pin_locked) {
          err = ecu_config_gpio_output_pwm_set_dutycycle(ctx->config.output_pwm_pin, output_dutycycle);
          if(err == E_OK) {
            ctx->pwm_dutycycle_prev = output_dutycycle;
          } else {
            ctx->diag.bits.output_drive_error = true;
          }
        } else {
          ctx->pwm_dutycycle_prev = output_dutycycle;
        }
      }

      if(pid_reset) {
        math_pid_reset(&ctx->pid_boost, now);
        math_pid_reset(&ctx->pid_speed, now);
      }
    }
  }
}

error_t wgcv_get_data(wgcv_ctx_t *ctx, wgcv_data_t *data)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(data == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    *data = ctx->data;

  } while(0);

  return err;
}

error_t wgcv_get_diag(wgcv_ctx_t *ctx, wgcv_diag_t *diag)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(diag == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    *diag = ctx->diag;

  } while(0);

  return err;
}

error_t wgcv_set_enabled(wgcv_ctx_t *ctx, bool enabled)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->configured == false, err = E_NOTRDY);

    ctx->data.enabled = enabled;

  } while(0);

  return err;
}

error_t wgcv_set_dutycycle(wgcv_ctx_t *ctx, float dutycycle)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->configured == false, err = E_NOTRDY);

    ctx->data.input_dutycycle = dutycycle;

  } while(0);

  return err;
}

error_t wgcv_set_target_boost(wgcv_ctx_t *ctx, float target_boost)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->configured == false, err = E_NOTRDY);

    ctx->data.target_boost = MAX(target_boost, 0.0f);

  } while(0);

  return err;
}

error_t wgcv_force_input_reset(wgcv_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->configured == false, err = E_NOTRDY);

    ctx->data.force_input_engaged = false;

  } while(0);

  return err;
}

error_t wgcv_force_input_set(wgcv_ctx_t *ctx, float dutycycle)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->configured == false, err = E_NOTRDY);

    ctx->data.force_input_engaged = true;
    ctx->data.force_input_dutycycle = dutycycle;

  } while(0);

  return err;
}

error_t wgcv_force_pwm_reset(wgcv_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->configured == false, err = E_NOTRDY);

    ctx->data.force_pwm_engaged = false;

  } while(0);

  return err;
}

error_t wgcv_force_pwm_set(wgcv_ctx_t *ctx, float dutycycle)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->configured == false, err = E_NOTRDY);

    ctx->data.force_pwm_engaged = true;
    ctx->data.force_pwm_dutycycle = dutycycle;

  } while(0);

  return err;
}
