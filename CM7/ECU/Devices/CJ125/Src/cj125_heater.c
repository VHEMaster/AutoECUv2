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
  float voltage_temp;
  float clamp_temp;

  float heat_cplt_ref, heat_initial_ref, heat_val;
  bool heat_cplt_condition;
  bool heat_initial_condition;

  const cj125_config_stage_t *config_staged;
  cj125_config_staged_stage_t staged_stage;

  while(true) {
    time_voltage_last = ctx->voltages_timestamp;
    now = time_now_us();
    time_delta = time_diff(now, time_voltage_last);
    math_pid_set_koffs(&ctx->heater_pid, &ctx->config.heater_pid_koffs);

    clamp_temp = ctx->config.heater_max_voltage - ctx->config.heater_nominal_voltage;
    math_pid_set_clamp(&ctx->heater_pid, -clamp_temp, clamp_temp);

    if(ctx->config.staged_nunited == CJ125_CONFIG_STAGED_UNITED) {
      config_staged = &ctx->config.config_united;
    } else if(ctx->config.staged_nunited == CJ125_CONFIG_STAGED_STAGED) {
      staged_stage = ctx->data.staged_stage;
      if(staged_stage < CJ125_CONFIG_STAGED_STAGE_COUNT) {
        config_staged = &ctx->config.config_staged[staged_stage];
      } else {
        config_staged = NULL;
      }
    } else {
      config_staged = NULL;
    }

    if(ctx->ready && ctx->initialized && ctx->configured && ctx->heater_ready &&
        time_delta <= CJ125_VOLTAGES_TIMEOUT_US && ctx->diag.byte == CJ125_DIAG_OK
        && config_staged != NULL) {

      ctx->data.regs.init1.bits.pa = config_staged->pa_enabled;
      ctx->data.regs.init2.bits.enscun = config_staged->reg_enscun;
      ctx->data.regs.init2.bits.set_dia_q = config_staged->reg_set_dia_q;
      ctx->data.regs.init2.bits.pr = config_staged->pump_ref_current;

      heat_initial_condition = false;
      if(ctx->heater_fsm == CJ125_HEATER_HEATUP ||
          ctx->heater_fsm == CJ125_HEATER_HEATUP_WAITING_CPLT) {
        if(ctx->config.heated_value_thr_source == CJ125_CONFIG_HEATED_TEMP_THR_SRC_RESISTANCE) {
          heat_initial_ref = ctx->config.heatup_ref_resistance_initial;
          heat_val = ctx->data.heat_resistance;
          heat_initial_condition = heat_val <= heat_initial_ref;
        } else if(ctx->config.heated_value_thr_source == CJ125_CONFIG_HEATED_TEMP_THR_SRC_TEMPERATURE) {
          heat_initial_ref = ctx->config.heatup_ref_temperature_initial;
          heat_val = ctx->data.temp_value;
          heat_initial_condition = heat_val >= heat_initial_ref;
        } else {
          heat_initial_condition = false;
        }
      }

      heat_cplt_condition = false;
      if(ctx->heater_fsm == CJ125_HEATER_HEATUP ||
          ctx->heater_fsm == CJ125_HEATER_HEATUP_WAITING_CPLT) {
        if(ctx->config.heated_value_thr_source == CJ125_CONFIG_HEATED_TEMP_THR_SRC_RESISTANCE) {
          if(ctx->config.heated_value_thr_override) {
            heat_cplt_ref = ctx->config.heatup_ref_resistance_cplt;
          } else {
            heat_cplt_ref = ctx->data.heat_resistance;
          }
          heat_val = ctx->data.heat_resistance;
          heat_cplt_condition = heat_val <= heat_cplt_ref;
        } else if(ctx->config.heated_value_thr_source == CJ125_CONFIG_HEATED_TEMP_THR_SRC_TEMPERATURE) {
          if(ctx->config.heated_value_thr_override) {
            heat_cplt_ref = ctx->config.heatup_ref_temperature_cplt;
          } else {
            heat_cplt_ref = ctx->data.heat_ref_temp;
          }
          heat_val = ctx->data.temp_value;
          heat_cplt_condition = heat_val >= heat_cplt_ref;
        } else {
          heat_cplt_condition = false;
        }
      }

      switch(ctx->heater_fsm) {
        case CJ125_HEATER_RESET:
          math_pid_reset(&ctx->heater_pid, now);
          math_pid_set_target(&ctx->heater_pid, ctx->data.heat_ref_temp);
          ctx->data.heater_voltage = 0.0f;
          ctx->data.operating_status = CJ125_OPERATING_STATUS_IDLE;
          ctx->data.staged_stage = CJ125_CONFIG_STAGED_STAGE_IDLE;
          if(ctx->heatup_type > CJ125_HEATUP_TYPE_OFF) {
            ctx->heater_fsm = CJ125_HEATER_PREHEAT;
            ctx->data.operating_status = CJ125_OPERATING_STATUS_PREHEAT;
            continue;
          }
          break;
        case CJ125_HEATER_PREHEAT:
          ctx->data.operating_status = CJ125_OPERATING_STATUS_PREHEAT;
          ctx->data.staged_stage = CJ125_CONFIG_STAGED_STAGE_PREHEAT;
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

          if(!heat_initial_condition) {
            ctx->data.staged_stage = CJ125_CONFIG_STAGED_STAGE_HEATUP_INITIAL;
          } else  {
            ctx->data.staged_stage = CJ125_CONFIG_STAGED_STAGE_HEATUP_FINAL;
          }

          if(ctx->data.heater_voltage >= ctx->config.heater_initial_max_voltage) {
            ctx->heater_fsm = CJ125_HEATER_HEATUP_WAITING_CPLT;
            continue;
          } else if(heat_cplt_condition) {
            ctx->heater_fsm = CJ125_HEATER_HEATUP_WAITING_CPLT;
            continue;
          } else if(ctx->heatup_type == CJ125_HEATUP_TYPE_OFF) {
            ctx->data.heater_voltage = 0.0f;
            ctx->heater_fsm = CJ125_HEATER_RESET;
          }
          break;
        case CJ125_HEATER_HEATUP_WAITING_CPLT:
          time_delta = time_diff(now, ctx->heater_fsm_last);
          ctx->data.operating_status = CJ125_HEATER_HEATUP_WAITING_CPLT;
          ctx->data.heater_voltage = ctx->config.heater_initial_max_voltage;

          if(!heat_initial_condition) {
            ctx->data.staged_stage = CJ125_CONFIG_STAGED_STAGE_HEATUP_INITIAL;
          } else  {
            ctx->data.staged_stage = CJ125_CONFIG_STAGED_STAGE_HEATUP_FINAL;
          }

          if(heat_cplt_condition) {
            ctx->heater_fsm = CJ125_HEATER_OPERATING;
            ctx->data.operating_status = CJ125_OPERATING_STATUS_OPERATING;
            ctx->heater_fsm_last = now;
            math_pid_reset(&ctx->heater_pid, now);
            math_pid_set_target(&ctx->heater_pid, ctx->data.heat_ref_temp);
            continue;
          } else if(time_delta >= ctx->config.heater_temperature_timeout) {
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
          ctx->data.staged_stage = CJ125_CONFIG_STAGED_STAGE_OPERATING;
          if(time_delta >= ctx->config.heater_pid_update_period) {
            ctx->heater_fsm_last = now;

            voltage_temp = math_pid_update(&ctx->heater_pid, ctx->data.temp_value, now) + ctx->config.heater_nominal_voltage;
            ctx->data.heater_voltage = CLAMP(voltage_temp, 0.0f, ctx->config.heater_max_voltage);
          } else if(ctx->heatup_type == CJ125_HEATUP_TYPE_OFF) {
            ctx->data.heater_voltage = 0.0f;
            ctx->heater_fsm = CJ125_HEATER_RESET;
          } else if(ctx->data.ur_voltage >= CJ125_HEATER_OPERATING_UR_LIMIT_H) {
            ctx->data.heater_voltage = 0;
            ctx->data.operating_status = CJ125_OPERATING_STATUS_ERROR;
            ctx->heater_fsm = CJ125_HEATER_ERROR;
          }
          break;
        case CJ125_HEATER_ERROR:
          ctx->data.heater_voltage = 0.0f;
          if(ctx->heatup_type <= CJ125_HEATUP_TYPE_PREHEAT) {
            ctx->heater_fsm = CJ125_HEATER_RESET;
          }
          break;
        default:
          break;
      }
    } else {
      ctx->data.heater_voltage = 0.0f;
      ctx->data.operating_status = CJ125_OPERATING_STATUS_IDLE;
      ctx->data.staged_stage = CJ125_CONFIG_STAGED_STAGE_IDLE;
      ctx->heater_fsm = CJ125_HEATER_RESET;
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
  dutycycle *= dutycycle;

  ctx->data.heater_dutycycle = dutycycle;

  if(dutycycle > 0.0f) {
    *ctx->heater.tim_pulse = (float)ctx->heater.tim_period * dutycycle;
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
