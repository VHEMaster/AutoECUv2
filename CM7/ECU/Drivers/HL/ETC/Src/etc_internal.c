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

  float speed, current, voltage, power_voltage, dutycycle;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->configured == false, err = E_NOTRDY);

    now = time_get_current_us();
    time_delta = time_diff(now, ctx->process_time);
    ctx->process_time = now;

    err = ecu_sensors_tps_get_value(ctx->config.sensor_tps, &ctx->tps_data);
    BREAK_IF(err != E_OK);

    ctx->position_prev = ctx->position_current;
    ctx->position_current = ctx->tps_data.position_unfiltered;
    ctx->current_speed = (ctx->position_current - ctx->position_prev) / ((float)time_delta * 0.000001f);

    (void)input_get_value(ctx->power_voltage_pin, &input_analog_value, NULL);
    power_voltage = (float)input_analog_value * INPUTS_ANALOG_MULTIPLIER_R;
    ctx->power_voltage = power_voltage;

    ctx->data.current_position = ctx->tps_data.position;

    enabled = ctx->data.enabled;
    err = ecu_devices_motor_set_enabled(ctx->config.sensor_tps, enabled);
    BREAK_IF(err != E_OK);
    err = ecu_devices_motor_set_frequency(ctx->config.sensor_tps, ctx->config.pwm_freq);
    BREAK_IF(err != E_OK);

    math_pid_set_koffs(&ctx->pid_position, &ctx->config.pid_position);
    math_pid_set_koffs(&ctx->pid_speed, &ctx->config.pid_speed);

    if(power_voltage < 5.0f) {
      ctx->data.diag |= ETC_DIAG_PWR_UNDERVOLT;
    }

    if(enabled == false) {
      ctx->data.dutycycle = 0;
      ctx->target_speed = 0;
      ctx->output_voltage = 0;
      math_pid_reset(&ctx->pid_position, now);
      math_pid_reset(&ctx->pid_speed, now);
      dutycycle = 0.0f;
    } else {

      math_pid_set_target(&ctx->pid_position, ctx->data.target_position);
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

      ctx->target_speed = speed;
      ctx->output_voltage = voltage;
    }

    err = ecu_devices_motor_set_dutycycle(ctx->config.device_motor, dutycycle);
    BREAK_IF(err != E_OK);

    ctx->data.diag = 0;


  } while(0);

  return err;
}
