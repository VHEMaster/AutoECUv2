/*
 * etc_fsm.c
 *
 *  Created on: May 16, 2024
 *      Author: VHEMaster
 */

#include "etc_fsm.h"
#include "config_motor.h"
#include "config_tps.h"

error_t etc_fsm(etc_ctx_t *ctx)
{
  error_t err = E_OK;
  time_us_t now;

  while(true) {
    now = time_get_current_us();

    switch(ctx->fsm_process) {
      case ETC_FSM_CONDITION:
        if(ctx->ready && ctx->reset_request == true && ctx->reset_errcode == E_AGAIN) {
          ctx->fsm_process = ETC_FSM_DISABLE;
          continue;
        }
        break;
      case ETC_FSM_DISABLE:
        err = ecu_devices_motor_set_enabled(ctx->config.device_motor, false);
        if(err == E_OK) {
          ctx->fsm_process = ETC_FSM_HWSC;
          continue;
        } else if(err != E_AGAIN) {
          ctx->reset_errcode = err;
          ctx->fsm_process = ETC_FSM_CONDITION;
        }
        break;
      case ETC_FSM_HWSC:
        err = ecu_devices_motor_hwsc(ctx->config.device_motor);
        if(err == E_OK) {
          ctx->fsm_process = ETC_FSM_DIAGOFF;
          continue;
        } else if(err != E_AGAIN) {
          ctx->reset_errcode = err;
          ctx->fsm_process = ETC_FSM_CONDITION;
        }
        break;
      case ETC_FSM_DIAGOFF:
        err = ecu_devices_motor_diagoff(ctx->config.device_motor);
        if(err == E_OK) {
          ctx->fsm_process = ETC_FSM_TPS_WAIT;
          ctx->reset_time = now;
        } else if(err != E_AGAIN) {
          ctx->reset_errcode = err;
          ctx->fsm_process = ETC_FSM_CONDITION;
        }
        break;
      case ETC_FSM_TPS_WAIT:
        err = ecu_sensors_tps_get_value(ctx->config.sensor_tps, &ctx->tps_data);
        if(err == E_OK) {
          if(ctx->tps_data.data_valid == true) {
            ctx->default_position_sampling = ctx->tps_data.position;
            ctx->default_position_samples = 1;
            ctx->reset_time = now;
            ctx->fsm_process = ETC_FSM_DEFAULT_SAMPLING;
            break;
          }
        } else if(err != E_AGAIN) {
          ctx->reset_errcode = err;
          ctx->fsm_process = ETC_FSM_CONDITION;
          break;
        }

        if(time_diff(now, ctx->reset_time) > ETC_TPS_TIMEOUT_US) {
          err = E_TIMEOUT;
          ctx->reset_errcode = err;
          ctx->fsm_process = ETC_FSM_CONDITION;
        }
        break;
      case ETC_FSM_DEFAULT_SAMPLING:
        if(time_diff(now, ctx->reset_time) > ETC_TPS_DEFAULT_POSITION_SAMPLE_PERIOD_US) {
          err = ecu_sensors_tps_get_value(ctx->config.sensor_tps, &ctx->tps_data);
          if(err == E_OK) {
            if(ctx->tps_data.data_valid == true) {
              ctx->default_position_sampling += ctx->tps_data.position;
              if(++ctx->default_position_samples >= ETC_TPS_DEFAULT_POSITION_SAMPLES) {
                ctx->default_position_sampling /= ctx->default_position_samples;
                ctx->data.default_position = ctx->default_position_sampling;

                err = E_OK;
                ctx->reset_errcode = err;
                ctx->fsm_process = ETC_FSM_CONDITION;
                break;
              }
            } else {
              err = E_FAULT;
            }
          } else if(err == E_AGAIN) {
            err = E_FAULT;
          }
        }

        if(err != E_OK && err != E_AGAIN) {
          ctx->reset_errcode = err;
          ctx->fsm_process = ETC_FSM_CONDITION;
        }
        break;
      default:
        break;
    }
    break;
  }

  return err;
}
