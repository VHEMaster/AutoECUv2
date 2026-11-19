/*
 * etc_internal.c
 *
 *  Created on: May 16, 2024
 *      Author: VHEMaster
 */

#include "etc_internal.h"
#include "compiler.h"
#include "interpolation.h"
#include "math_fast.h"
#include "math_misc.h"
#include <math.h>

static float etc_internal_spring_interpolate(const etc_spring_table_t *table, float position, bool relaxing)
{
  const etc_spring_point_t *points = table->points;
  const etc_spring_point_t *left, *right;
  uint32_t last = table->points_count - 1;
  float left_effort, right_effort;

  if(position <= points[0].position) {
    left = right = &points[0];
  } else if(position >= points[last].position) {
    left = right = &points[last];
  } else {
    size_t index = math_binary_search_strided(&table->points, sizeof(*points), last, position);
    left = &points[index];
    right = &points[index + 1];
  }

  if(relaxing) {
    left_effort = left->effort_relaxation;
    right_effort = right->effort_relaxation;
  } else {
    left_effort = left->effort_tension;
    right_effort = right->effort_tension;
  }

  if(left == right) {
    return left_effort;
  }

  return math_interpolate(left->position, position, right->position, left_effort, right_effort);
}

static float etc_internal_spring_effort(const etc_ctx_t *ctx)
{
  float position = ctx->position_current;
  float parking = ctx->data.parking_position;

  if(position == parking) {
    return 0.0f;
  }

  bool below = position < parking;
  const etc_spring_table_t *table = below ? &ctx->config.spring_below_parking : &ctx->config.spring_above_parking;
  bool relaxing = below ? ctx->spring_approach_direction > 0 : ctx->spring_approach_direction < 0;
  float distance = fabsf(position - parking);

  if(distance < table->deadband) {
    /* Fade from the effort at the deadband boundary to zero at parking. */
    float boundary = below ? parking - table->deadband : parking + table->deadband;
    return etc_internal_spring_interpolate(table, boundary, relaxing) * (distance / table->deadband);
  }

  return etc_internal_spring_interpolate(table, position, relaxing);
}

static float etc_internal_limit_approach_speed(const etc_ctx_t *ctx, float error, float distance, float speed)
{
  float brake_distance = ctx->config.approach_brake_distance;

  /* Limit only speed commands pointing toward the position target. */
  if(distance >= brake_distance || !((error > 0.0f && speed > 0.0f) || (error < 0.0f && speed < 0.0f))) {
    return speed;
  }

  float position = ctx->position_current;
  float parking = ctx->data.parking_position;
  bool relaxing = (position > parking && error < 0.0f) ||
      (position < parking && error > 0.0f);
  float deceleration = relaxing ? ctx->config.relaxation_brake_deceleration :
      ctx->config.tensioning_brake_deceleration;
  float limit_squared = 2.0f * deceleration * distance;
  if(!(deceleration > 0.0f && speed * speed > limit_squared)) {
    return speed;
  }

  float limited_speed = copysignf(fast_sqrt(limit_squared), speed);
  if(distance <= 0.5f * brake_distance) {
    return limited_speed;
  }

  /* Blend into the limit over the outer half of the approach zone. */
  float blend = MIN(1.0f, 2.0f * (brake_distance - distance) / brake_distance);
  blend = blend * blend * (3.0f - 2.0f * blend);
  return BLEND(speed, limited_speed, blend);
}

error_t etc_internal_process(etc_ctx_t *ctx)
{
  error_t err = E_OK;
  time_us_t now;
  time_delta_us_t time_delta;
  bool enabled;
  input_value_t input_analog_value;

  // TODO: set minimum control time delta
  // TODO: derivative filters, plausibility, TPS validity?

  float speed, current, power_voltage, dutycycle, target;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->configured == false, err = E_NOTRDY);
    enabled = ctx->data.enabled;

    now = time_now_us();
    time_delta = time_diff(now, ctx->process_time);
    ctx->process_time = now;

    err = ecu_sensors_tps_get_value(ctx->config.sensor_tps, &ctx->tps_data);
    if(err != E_OK && err != E_AGAIN) {
      ctx->diag.bits.tps_handle_error = true;
    }

    err = ecu_sensors_tps_get_diag(ctx->config.sensor_tps, &ctx->tps_diag);
    if(err != E_OK && err != E_AGAIN) {
      ctx->diag.bits.tps_handle_error = true;
    }

    if(ctx->tps_data.data_valid != true) {
      ctx->diag.bits.tps_data_validity_error = true;
      enabled = false;
    } else if(ctx->tps_data.position_valid != true) {
      ctx->diag.bits.tps_pos_validity_error = true;
      enabled = false;
    }

    float position_prev = ctx->position_current;
    ctx->position_current = ctx->tps_data.position_unfiltered;
    ctx->current_speed = (ctx->position_current - position_prev) / ((float)time_delta * 0.000001f);

    (void)input_get_value(ctx->power_voltage_pin, &input_analog_value, NULL);
    power_voltage = (float)input_analog_value * INPUTS_ANALOG_MULTIPLIER_R;
    ctx->power_voltage = power_voltage;

    ctx->data.current_position = ctx->tps_data.position;
    target = ctx->data.target_position;
    target = CLAMP(target, 0.0f, 100.0f);

    if(ctx->diag.bits.position_reach_failure != false) {
      enabled = false;
    }

    ctx->diag.bits.tps_diag_error = ctx->tps_diag.data != 0;
    if(ctx->diag.bits.tps_diag_error) {
      enabled = false;
    }

    err = ecu_devices_motor_set_enabled(ctx->config.device_motor, enabled);
    if(err != E_OK && err != E_AGAIN) {
      ctx->diag.bits.motor_handle_error = true;
    }
    err = ecu_devices_motor_set_frequency(ctx->config.device_motor, ctx->config.pwm_freq);
    if(err != E_OK && err != E_AGAIN) {
      ctx->diag.bits.motor_handle_error = true;
    }
    err = ecu_devices_motor_get_diag(ctx->config.device_motor, &ctx->motor_diag);
    if(err != E_OK && err != E_AGAIN) {
      ctx->diag.bits.motor_handle_error = true;
    }

    l9960_status_openload_t motor_openload = ctx->motor_diag.bits.ol;
    ctx->motor_diag.bits.ol = 0;

    ctx->diag.bits.motor_error = ctx->motor_diag.data != 0 || motor_openload == L9960_STATUS_OL_OPENLOAD;

    math_pid_set_koffs(&ctx->pid_position, &ctx->config.pid_position);
    math_pid_set_koffs(&ctx->pid_speed, &ctx->config.pid_speed);

    if(enabled == false) {
      ctx->data.active = false;
      ctx->spring_approach_direction = 0;
      ctx->data.dutycycle = 0;
      ctx->target_speed = 0;
      ctx->data.output_voltage = 0;
      math_pid_reset(&ctx->pid_position, now);
      math_pid_reset(&ctx->pid_speed, now);
      ctx->pos_reach_time = now;
      dutycycle = 0.0f;
    } else {
      float position_error = target - ctx->position_current;
      float distance = fabsf(position_error);

      /* Keep the approach direction through settling at an unchanged target. */
      if(ctx->spring_approach_direction == 0 || target != ctx->spring_target_position) {
        if(position_error > 0.0f) {
          ctx->spring_approach_direction = 1;
        } else if(position_error < 0.0f) {
          ctx->spring_approach_direction = -1;
        }
        ctx->spring_target_position = target;
      }

      math_pid_set_target(&ctx->pid_position, target);
      speed = math_pid_update(&ctx->pid_position, ctx->position_current, now);
      speed = etc_internal_limit_approach_speed(ctx, position_error, distance, speed);

      math_pid_set_target(&ctx->pid_speed, speed);
      current = math_pid_update(&ctx->pid_speed, ctx->current_speed, now);

      current += etc_internal_spring_effort(ctx);

      dutycycle = current / power_voltage;
      dutycycle = CLAMP(dutycycle, -1.0f, 1.0f);

      if(dutycycle < 0) {
        dutycycle = -fast_sqrt(fabsf(dutycycle));
      } else if(dutycycle > 0) {
        dutycycle = fast_sqrt(dutycycle);
      }
      ctx->data.dutycycle = dutycycle;

      if(distance < ctx->config.motor_reach_threshold) {
        ctx->pos_reach_time = now;
        ctx->diag.bits.position_reach_failure = false;
      } else if(time_diff(now, ctx->pos_reach_time) > ctx->config.motor_reach_timeout) {
        ctx->diag.bits.position_reach_failure = true;
      }

      ctx->target_speed = speed;
      ctx->data.output_voltage = dutycycle * power_voltage;
      ctx->data.active = true;
    }

    err = ecu_devices_motor_set_dutycycle(ctx->config.device_motor, dutycycle);
    if(err != E_OK && err != E_AGAIN) {
      ctx->diag.bits.motor_pwm_error = true;
    }

  } while(0);

  return err;
}
