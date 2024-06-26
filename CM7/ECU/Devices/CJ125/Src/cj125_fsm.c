/*
 * cj125_fsm.c
 *
 *  Created on: Apr 19, 2024
 *      Author: VHEMaster
 */

#include "cj125_internal.h"
#include "cj125_fsm.h"
#include "cj125_reg.h"
#include "compiler.h"
#include "errors.h"
#include "time.h"

#include <limits.h>

ITCM_FUNC static error_t cj125_fsm_reset(cj125_ctx_t *ctx)
{
  error_t err;
  time_us_t now;
  uint32_t prim;

  while(true) {
    err = E_AGAIN;
    now = time_now_us();

    switch(ctx->reset_fsm) {
      case CJ125_RESET_CONDITION:
        if(ctx->ready == true && ctx->reset_request == true && ctx->reset_errcode == E_AGAIN) {
          if(gpio_valid(&ctx->init.nrst_pin)) {
            gpio_set(&ctx->init.nrst_pin);
          }
          ctx->reset_fsm = CJ125_RESET_IDENT;
          ctx->request.bytes[0] = CJ125_REG_RD_IDENT;
          ctx->request.bytes[1] = 0;
          ctx->data_lambda_valid = false;
          ctx->data_temp_valid = false;
          ctx->heatup_type = CJ125_HEATUP_TYPE_OFF;
          ctx->initialized = false;
          ctx->configured = false;
          ctx->data.heater_voltage = 0;
          ctx->data.operating_status = CJ125_OPERATING_STATUS_IDLE;
          ctx->diag_timestamp = now;
          err = E_AGAIN;
        } else {
          err = E_OK;
        }
        break;
      case CJ125_RESET_IDENT:
        err = cj125_serial_operation(ctx, ctx->request, &ctx->response);
        if(err == E_OK) {
          ctx->regs.ident.data = ctx->response.bytes[1];
          ctx->reset_fsm = CJ125_RESET_CHECK;
          err = E_AGAIN;
          continue;
        } else if(err != E_AGAIN) {
          ctx->initialized = false;
          ctx->reset_fsm = CJ125_RESET_CONDITION;
          ctx->reset_errcode = err;
        }
        break;
      case CJ125_RESET_CHECK:
        if(ctx->regs.ident.bits.ic_number == CJ125_REG_IC_NUMBER) {
          ctx->reset_fsm = CJ125_RESET_REQUEST;
          ctx->regs.init2.data = 0;
          ctx->regs.init2.bits.sreset = CJ125_SRESET_SET;
          ctx->request.bytes[0] = CJ125_REG_WR_INIT2;
          ctx->request.bytes[1] = ctx->regs.init2.data;
          err = E_AGAIN;
          continue;
        } else {
          err = E_BADRESP;
          ctx->initialized = false;
          ctx->reset_fsm = CJ125_RESET_CONDITION;
          ctx->reset_errcode = err;
        }
        break;
      case CJ125_RESET_REQUEST:
        err = cj125_serial_operation(ctx, ctx->request, &ctx->response);
        if(err == E_OK) {
          ctx->reset_fsm = CJ125_RESET_CALIB_INIT_READ;
          ctx->request.bytes[0] = CJ125_REG_RD_INIT1;
          ctx->data_lambda_valid = 0;
          ctx->data_temp_valid = 0;
          err = E_AGAIN;
          continue;
        } else if(err != E_AGAIN) {
          ctx->initialized = false;
          ctx->reset_fsm = CJ125_RESET_CONDITION;
          ctx->reset_errcode = err;
        }
        break;
      case CJ125_RESET_CALIB_INIT_READ:
        err = cj125_serial_operation(ctx, ctx->request, &ctx->response);
        if(err == E_OK) {
          ctx->calib_init1_byte = ctx->response.bytes[1];
          ctx->reset_fsm = CJ125_RESET_CALIB_INIT_WRITE;
          ctx->regs.init1.data = ctx->calib_init1_byte;
          ctx->regs.init1.bits.la = CJ125_LA_RA_CALIBRATE;
          ctx->regs.init1.bits.ra = CJ125_LA_RA_CALIBRATE;
          ctx->data.ampfactor = ctx->regs.init1.bits.vl;
          ctx->request.bytes[0] = CJ125_REG_WR_INIT1;
          ctx->request.bytes[1] = ctx->regs.init1.data;
          err = E_AGAIN;
          continue;
        } else if(err != E_AGAIN) {
          ctx->reset_fsm = CJ125_RESET_CONDITION;
          ctx->reset_errcode = err;
        }
        break;
      case CJ125_RESET_CALIB_INIT_WRITE:
        err = cj125_serial_operation(ctx, ctx->request, &ctx->response);
        if(err == E_OK) {
          ctx->reset_fsm = CJ125_RESET_CALIB_SAMPLE;
          ctx->calib_timestamp = now;
          ctx->calib_ignored = 0;
          ctx->calib_samples = 0;
          ctx->calib_ur_voltage = 0;
          ctx->calib_ua_voltage = 0;
          ctx->calib_ref_voltage = 0;
          ctx->voltages_updated = false;
          err = E_AGAIN;
          continue;
        } else if(err != E_AGAIN) {
          ctx->reset_fsm = CJ125_RESET_CONDITION;
          ctx->reset_errcode = err;
        }
        break;
      case CJ125_RESET_CALIB_SAMPLE:
        if(ctx->calib_samples >= CJ125_CALIBRATION_MIN_SAMPLES &&
            time_diff(now, ctx->calib_timestamp) >= CJ125_CALIBRATION_MIN_PERIOD_US) {
          ctx->reset_fsm = CJ125_RESET_CALIB_INIT_RESTORE;
          ctx->regs.init1.data = ctx->calib_init1_byte;
          ctx->regs.init1.bits.la = CJ125_LA_RA_NORMAL;
          ctx->regs.init1.bits.ra = CJ125_LA_RA_NORMAL;
          ctx->request.bytes[0] = CJ125_REG_WR_INIT1;
          ctx->request.bytes[1] = ctx->regs.init1.data;
          err = E_AGAIN;
          continue;
        } else if(time_diff(now, ctx->calib_timestamp) <= CJ125_CALIBRATION_TIMEOUT_US) {
          if(ctx->voltages_updated) {
            if(ctx->calib_ignored >= CJ125_CALIBRATION_SAMPLES_TO_IGNORE) {
              if(ctx->calib_samples < USHRT_MAX) {
                if(ctx->data.ur_voltage > CJ125_CALIBRATION_UR_THRESHOLD_L && ctx->data.ur_voltage < CJ125_CALIBRATION_UR_THRESHOLD_H &&
                    ctx->data.ua_voltage > CJ125_CALIBRATION_UA_THRESHOLD_L && ctx->data.ua_voltage < CJ125_CALIBRATION_UA_THRESHOLD_H &&
                    ctx->data.ref_voltage > CJ125_CALIBRATION_REF_THRESHOLD_L && ctx->data.ref_voltage < CJ125_CALIBRATION_REF_THRESHOLD_H) {
                  ctx->calib_ua_voltage += ctx->data.ua_voltage;
                  ctx->calib_ur_voltage += ctx->data.ur_voltage;
                  ctx->calib_ref_voltage += ctx->data.ref_voltage;
                  ctx->calib_samples++;
                } else {
                  err = E_BADVALUE;
                  ctx->reset_fsm = CJ125_RESET_CONDITION;
                  ctx->reset_errcode = err;
                }
              }
            } else {
              ctx->calib_ignored++;
            }
            ctx->voltages_updated = false;
          }
          err = E_AGAIN;
        } else {
          err = E_TIMEOUT;
          ctx->reset_fsm = CJ125_RESET_CONDITION;
          ctx->reset_errcode = err;
        }
        break;
      case CJ125_RESET_CALIB_INIT_RESTORE:
        err = cj125_serial_operation(ctx, ctx->request, &ctx->response);
        if(err == E_OK) {
          ctx->reset_fsm = CJ125_RESET_CALIB_CALCULATE;
          err = E_AGAIN;
          continue;
        } else if(err != E_AGAIN) {
          ctx->initialized = false;
          ctx->reset_fsm = CJ125_RESET_CONDITION;
          ctx->reset_errcode = err;
        }
        break;
      case CJ125_RESET_CALIB_CALCULATE:
        prim = EnterCritical();
        ctx->calib_ua_voltage /= ctx->calib_samples;
        ctx->calib_ur_voltage /= ctx->calib_samples;
        ctx->calib_ref_voltage /= ctx->calib_samples;

        ctx->data.ua_voltage = ctx->calib_ua_voltage;
        ctx->data.ur_voltage = ctx->calib_ur_voltage;
        ctx->data.ref_voltage = ctx->calib_ref_voltage;

        ctx->data.lambda_radj = CJ125_CALIBRATION_RADJ_DEFAULT;

        err = cj125_update_data(ctx, true);
        if(err == E_OK) {
          ctx->data.heat_ref_voltage = ctx->calib_ur_voltage;
          ctx->data.heat_ref_temp = ctx->data.temp_value;
          ctx->data.heat_ref_resistance = ctx->data.heat_resistance;

          ctx->data.lambda_ref_voltage = ctx->calib_ua_voltage;
          ctx->data.lambda_radj = ctx->calib_ua_voltage / ctx->calib_ref_voltage;

          err = E_OK;
          ctx->reset_fsm = CJ125_RESET_CONDITION;
          ctx->reset_errcode = err;
          ctx->initialized = true;
        } else {
          ctx->reset_fsm = CJ125_RESET_CONDITION;
          ctx->reset_errcode = err;
          ctx->initialized = false;
        }

        ctx->voltages_updated = false;

        ExitCritical(prim);
        break;
      default:
        break;
    }
    break;
  }

  return err;
}

ITCM_FUNC static error_t cj125_fsm_diag(cj125_ctx_t *ctx)
{
  error_t err;
  time_us_t now;

  while(true) {
    err = E_AGAIN;
    now = time_now_us();

    switch(ctx->diag_fsm) {
      case CJ125_DIAG_CONDITION:
        if(ctx->ready == true && ctx->initialized == true && time_diff(now, ctx->diag_timestamp) >= CJ125_DIAG_POLL_PERIOD_US) {
          ctx->request.bytes[0] = CJ125_REG_RD_DIAG;
          ctx->request.bytes[1] = 0;
          ctx->diag_fsm = CJ125_DIAG_REQUEST;
          err = E_AGAIN;
          continue;
        } else {
          err = E_OK;
        }
        break;
      case CJ125_DIAG_REQUEST:
        err = cj125_serial_operation(ctx, ctx->request, &ctx->response);
        if(err == E_OK) {
          ctx->regs.diag.data = ctx->response.bytes[1];
          ctx->diag.byte = ctx->regs.diag.data ^ 0xFF;
          ctx->diag_fsm = CJ125_DIAG_CONDITION;
          ctx->diag_timestamp = now;
          err = E_OK;
        } else if(err != E_AGAIN) {
          ctx->diag_fsm = CJ125_DIAG_CONDITION;
        }
        break;
      default:
        break;
    }
    break;
  }

  return err;
}

ITCM_FUNC static error_t cj125_fsm_configure(cj125_ctx_t *ctx)
{
  error_t err;

  while(true) {
    err = E_AGAIN;

    switch(ctx->config_fsm) {
      case CJ125_CONFIG_CONDITION:
        if(ctx->ready == true && ctx->initialized == true && ctx->config_errcode == E_AGAIN && ctx->config_request == true) {
          ctx->configured = false;

          ctx->regs.init1.data = 0;
          ctx->regs.init1.bits.vl = ctx->config.ampfactor;
          ctx->regs.init1.bits.la = CJ125_LA_RA_NORMAL;
          ctx->regs.init1.bits.en_f3k = 1;
          ctx->regs.init1.bits.ra = CJ125_LA_RA_NORMAL;
          ctx->regs.init1.bits.pa = 0;
          ctx->regs.init1.bits.en_hold = 1;

          ctx->regs.init2.data = 0;
          ctx->regs.init2.bits.pr = ctx->config.pump_ref_current;
          ctx->regs.init2.bits.enscun = ctx->config.reg_enscun != false;
          ctx->regs.init2.bits.set_dia_q = ctx->config.reg_set_dia_q != false;
          ctx->regs.init2.bits.sreset = 0;

          ctx->request.bytes[0] = CJ125_REG_WR_INIT1;
          ctx->request.bytes[1] = ctx->regs.init1.data;
          ctx->config_fsm = CJ125_CONFIG_REQUEST_INIT1;
          err = E_AGAIN;
          continue;
        } else {
          err = E_OK;
        }
        break;
      case CJ125_CONFIG_REQUEST_INIT1:
        err = cj125_serial_operation(ctx, ctx->request, &ctx->response);
        if(err == E_OK) {
          ctx->request.bytes[0] = CJ125_REG_WR_INIT2;
          ctx->request.bytes[1] = ctx->regs.init2.data;
          ctx->config_fsm = CJ125_CONFIG_REQUEST_INIT2;
          err = E_AGAIN;
          continue;
        } else if(err != E_AGAIN) {
          ctx->config_fsm = CJ125_CONFIG_CONDITION;
          ctx->config_errcode = err;
        }
        break;
      case CJ125_CONFIG_REQUEST_INIT2:
        err = cj125_serial_operation(ctx, ctx->request, &ctx->response);
        if(err == E_OK) {
          err = E_OK;
          ctx->config_errcode = err;
          ctx->configured = true;
          ctx->config_fsm = CJ125_CONFIG_CONDITION;
        } else if(err != E_AGAIN) {
          ctx->config_fsm = CJ125_CONFIG_CONDITION;
          ctx->config_errcode = err;
        }
        break;
      default:
        break;
    }
    break;
  }

  return err;
}

ITCM_FUNC error_t cj125_fsm(cj125_ctx_t *ctx)
{
  error_t err;

  do {
    err = E_OK;

    switch(ctx->process_fsm) {
      case CJ125_PROCESS_RESET:
        err = cj125_fsm_reset(ctx);
        break;
      case CJ125_PROCESS_DIAG:
        err = cj125_fsm_diag(ctx);
        break;
      case CJ125_PROCESS_CONFIGURE:
        err = cj125_fsm_configure(ctx);
        break;
      default:
        break;
    }

    if(err == E_OK) {
      if(++ctx->process_fsm >= CJ125_PROCESS_MAX) {
        ctx->process_fsm = 0;
        break;
      }
    }
  } while(0);

  return err;
}
