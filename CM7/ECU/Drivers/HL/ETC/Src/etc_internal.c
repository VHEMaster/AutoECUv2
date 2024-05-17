/*
 * etc_internal.c
 *
 *  Created on: May 16, 2024
 *      Author: VHEMaster
 */

#include "etc_internal.h"
#include "compiler.h"
#include "math_fast.h"
#include <math.h>

error_t etc_internal_process(etc_ctx_t *ctx)
{
  error_t err = E_OK;
  time_us_t now;
  time_delta_us_t time_delta;
  bool enabled;
  input_value_t input_analog_value;

  float speed, current, voltage, power_voltage, dutycycle, target;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->configured == false, err = E_NOTRDY);
    enabled = ctx->data.enabled;

    now = time_get_current_us();
    time_delta = time_diff(now, ctx->process_time);
    ctx->process_time = now;

    err = ecu_sensors_tps_get_value(ctx->config.sensor_tps, &ctx->tps_data);
    BREAK_IF(err != E_OK);

    err = ecu_sensors_tps_get_diag(ctx->config.device_motor, &ctx->tps_diag);
    BREAK_IF(err != E_OK);

    ctx->position_prev = ctx->position_current;
    ctx->position_current = ctx->tps_data.position_unfiltered;
    ctx->current_speed = (ctx->position_current - ctx->position_prev) / ((float)time_delta * 0.000001f);

    (void)input_get_value(ctx->power_voltage_pin, &input_analog_value, NULL);
    power_voltage = (float)input_analog_value * INPUTS_ANALOG_MULTIPLIER_R;
    ctx->power_voltage = power_voltage;

    ctx->data.current_position = ctx->tps_data.position;
    target = ctx->data.target_position;
    target = CLAMP(target, 0.0f, 100.0f);

    if(ctx->diag.bits.position_reach_failure != false) {
      enabled = false;
    }

    if(ctx->tps_diag.data != 0) {
      enabled = false;
      ctx->diag.bits.tps_error = true;
    } else {
      ctx->diag.bits.tps_error = false;
    }

    err = ecu_devices_motor_set_enabled(ctx->config.device_motor, enabled);
    BREAK_IF(err != E_OK);
    err = ecu_devices_motor_set_frequency(ctx->config.device_motor, ctx->config.pwm_freq);
    BREAK_IF(err != E_OK);
    err = ecu_devices_motor_get_diag(ctx->config.device_motor, &ctx->motor_diag);
    BREAK_IF(err != E_OK);

    l9960_status_openload_t motor_openload = ctx->motor_diag.bits.ol;
    ctx->motor_diag.bits.ol = 0;

    if(ctx->motor_diag.data != 0 || motor_openload == L9960_STATUS_OL_OPENLOAD) {
      ctx->diag.bits.motor_error = true;
    } else {
      ctx->diag.bits.motor_error = false;
    }

    math_pid_set_koffs(&ctx->pid_position, &ctx->config.pid_position);
    math_pid_set_koffs(&ctx->pid_speed, &ctx->config.pid_speed);

    if(power_voltage < 5.0f) {
      ctx->diag.bits.pwr_undervoltage = true;
    } else {
      ctx->diag.bits.pwr_undervoltage = false;
    }

    if(power_voltage > 18.0f) {
      ctx->diag.bits.pwr_overvoltage = true;
      enabled = false;
    } else {
      ctx->diag.bits.pwr_overvoltage = false;
    }

    if(enabled == false) {
      ctx->data.active = false;
      ctx->data.dutycycle = 0;
      ctx->target_speed = 0;
      ctx->output_voltage = 0;
      math_pid_reset(&ctx->pid_position, now);
      math_pid_reset(&ctx->pid_speed, now);
      ctx->pos_reach_time = now;
      dutycycle = 0.0f;
    } else {

      math_pid_set_target(&ctx->pid_position, target);
      speed = math_pid_update(&ctx->pid_position, ctx->position_current, now);

      math_pid_set_target(&ctx->pid_speed, speed);
      current = math_pid_update(&ctx->pid_speed, ctx->current_speed, now);

      dutycycle = current / power_voltage;
      dutycycle = CLAMP(dutycycle, -1.0f, 1.0f);

      if(dutycycle < 0) {
        dutycycle = -fast_sqrt(fabsf(dutycycle));
      } else if(dutycycle > 0) {
        dutycycle = fast_sqrt(dutycycle);
      }
      voltage = dutycycle * power_voltage;

      if(fabsf(ctx->position_current - target) < ctx->config.motor_reach_threshold) {
        ctx->pos_reach_time = now;
        ctx->diag.bits.position_reach_failure = false;
      } else if(time_diff(now, ctx->pos_reach_time) > ctx->config.motor_reach_timeout) {
        ctx->diag.bits.position_reach_failure = true;
      }

      ctx->target_speed = speed;
      ctx->output_voltage = voltage;
      ctx->data.active = true;
    }

    err = ecu_devices_motor_set_dutycycle(ctx->config.device_motor, dutycycle);
    BREAK_IF(err != E_OK);




  } while(0);

  return err;
}
