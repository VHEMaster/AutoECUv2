/*
 * cj125_heater.c
 *
 *  Created on: Apr 23, 2024
 *      Author: VHEMaster
 */

#include "cj125_heater.h"
#include "compiler.h"

error_t cj125_heater_fsm(cj125_ctx_t *ctx)
{
  error_t err = E_OK;
  time_us_t now, time_voltage_last;
  time_delta_us_t time_delta;

  while(true) {
    time_voltage_last = ctx->voltages_timestamp;
    now = time_get_current_us();
    time_delta = time_diff(now, time_voltage_last);
    math_pid_set_koffs(&ctx->heater_pid, &ctx->config.heater_pid_koffs);

    if(ctx->ready && ctx->configured && ctx->heater_ready &&
        time_delta <= CJ125_VOLTAGES_TIMEOUT_US) {
      switch(ctx->heater_fsm) {
        case CJ125_HEATER_RESET:
          math_pid_reset(&ctx->heater_pid, now);
          math_pid_set_clamp(&ctx->heater_pid, 0.0f, ctx->config.heater_initial_max_voltage);
          math_pid_set_target(&ctx->heater_pid, ctx->data.heat_ref_temp);
          ctx->data.heater_voltage = 0.0f;
          ctx->data.operating_status = CJ125_OPERATING_STATUS_IDLE;
          if(ctx->heatup_type > CJ125_HEATUP_TYPE_OFF) {
            ctx->heater_fsm = CJ125_HEATER_PREHEAT;
            ctx->data.operating_status = CJ125_OPERATING_STATUS_HEATUP;
            continue;
          }
          break;
        case CJ125_HEATER_PREHEAT:
          ctx->data.operating_status = CJ125_OPERATING_STATUS_HEATUP;
          ctx->data.heater_voltage = ctx->config.heater_preheat_voltage;
          if(ctx->heatup_type > CJ125_HEATUP_TYPE_PREHEAT) {
            ctx->data.heater_voltage = ctx->config.heater_initial_voltage;
            ctx->heater_fsm = CJ125_HEATER_HEATUP;
            ctx->heater_fsm_last = now;
            continue;
          } else if(ctx->heatup_type == CJ125_HEATUP_TYPE_OFF) {
            ctx->data.heater_voltage = 0.0f;
            ctx->heater_fsm = CJ125_HEATER_RESET;
          }
          break;
        case CJ125_HEATER_HEATUP:
          time_delta = time_diff(now, ctx->heater_fsm_last);
          ctx->heater_fsm_last = now;
          ctx->data.operating_status = CJ125_OPERATING_STATUS_HEATUP;
          ctx->data.heater_voltage += (float)time_delta / TIME_US_IN_S * ctx->config.heater_ramp_rate;
          if(ctx->data.heater_voltage >= ctx->config.heater_initial_max_voltage) {
            ctx->heater_fsm = CJ125_HEATER_HEATUP_WAITING;
            continue;
          } else if(ctx->heatup_type == CJ125_HEATUP_TYPE_OFF) {
            ctx->data.heater_voltage = 0.0f;
            ctx->heater_fsm = CJ125_HEATER_RESET;
          }
          break;
        case CJ125_HEATER_HEATUP_WAITING:
          time_delta = time_diff(now, ctx->heater_fsm_last);
          ctx->data.operating_status = CJ125_OPERATING_STATUS_HEATUP;
          ctx->data.heater_voltage = ctx->config.heater_initial_max_voltage;
          if(ctx->data.temp_value > ctx->data.heat_ref_temp) {
            ctx->heater_fsm = CJ125_HEATER_OPERATING;
            ctx->data.operating_status = CJ125_OPERATING_STATUS_OPERATING;
            ctx->heater_fsm_last = now;
            math_pid_reset(&ctx->heater_pid, now);
            math_pid_set_clamp(&ctx->heater_pid, 0.0f, ctx->config.heater_max_voltage);
            continue;
          } else if(time_delta >= ctx->config.heater_operating_timeout) {
            ctx->data.heater_voltage = 0;
            ctx->data.operating_status = CJ125_OPERATING_STATUS_ERROR;
            ctx->heater_fsm = CJ125_HEATER_ERROR;
          } else if(ctx->heatup_type == CJ125_HEATUP_TYPE_OFF) {
            ctx->data.heater_voltage = 0.0f;
            ctx->heater_fsm = CJ125_HEATER_RESET;
          }
          break;
        case CJ125_HEATER_OPERATING:
          time_delta = time_diff(now, ctx->heater_fsm_last);
          ctx->data.operating_status = CJ125_OPERATING_STATUS_OPERATING;
          if(time_delta >= ctx->config.heater_pid_update_period) {
            ctx->heater_fsm_last = now;
            ctx->data.heater_voltage = math_pid_update(&ctx->heater_pid, ctx->data.temp_value, now) + ctx->config.heater_nominal_voltage;
          } else if(ctx->heatup_type == CJ125_HEATUP_TYPE_OFF ) {
            ctx->data.heater_voltage = 0.0f;
            ctx->heater_fsm = CJ125_HEATER_RESET;
          }
          break;
        case CJ125_HEATER_ERROR:
          if(ctx->heatup_type <= CJ125_HEATUP_TYPE_PREHEAT) {
            ctx->heater_fsm = CJ125_HEATER_RESET;
          }
          break;
        default:
          break;
      }
    } else {
      ctx->data.heater_voltage = 0;
      ctx->data.operating_status = CJ125_OPERATING_STATUS_IDLE;
    }
    break;
  }

  return err;
}

error_t cj125_update_heater_voltage(cj125_ctx_t *ctx)
{
  error_t err = E_OK;
  float dutycycle = 0.0f;
  float power_voltage = ctx->data.pwr_voltage;
  float voltage = ctx->data.heater_voltage;

  if(power_voltage > CJ125_HEATER_MINIMUM_POWER_VOLTAGE) {
    dutycycle = voltage / power_voltage;
  }

  dutycycle = CLAMP(dutycycle, 0.0f, 1.0f);

  ctx->data.heater_dutycycle = dutycycle;

  if(dutycycle > 0.0f) {
    *ctx->heater.tim_pulse = (float)ctx->heater.tim_period * dutycycle * dutycycle;
  } else {
    *ctx->heater.tim_pulse = 0.0f;
  }

  if(dutycycle > 0) {
    if(gpio_valid(&ctx->heater.heater_en_pin)) {
      gpio_set(&ctx->heater.heater_en_pin);
    }
    if(gpio_valid(&ctx->heater.heater_nen_pin)) {
      gpio_reset(&ctx->heater.heater_nen_pin);
    }
  }

  return err;
}
