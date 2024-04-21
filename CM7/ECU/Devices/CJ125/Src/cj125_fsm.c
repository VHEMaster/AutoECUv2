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

static error_t cj125_fsm_reset(cj125_ctx_t *ctx)
{
  error_t err;

  while(true) {
    err = E_AGAIN;

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
          ctx->regs.init2.bits.sreset = 1;
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
          ctx->initialized = true;
          ctx->reset_fsm = CJ125_RESET_CONDITION;
          ctx->reset_errcode = err;
          continue;
        } else if(err != E_AGAIN) {
          ctx->initialized = false;
          ctx->reset_fsm = CJ125_RESET_CONDITION;
          ctx->reset_errcode = err;
        }
        break;
      default:
        break;
    }
    break;
  }

  return err;
}

static error_t cj125_fsm_calibrate(cj125_ctx_t *ctx)
{
  error_t err;
  time_us_t now;
  cj125_reg_init1_t reg_init1;
  uint32_t prim;

  while(true) {
    err = E_AGAIN;
    now = time_get_current_us();

    switch(ctx->calib_fsm) {
      case CJ125_CALIB_CONDITION:
        if(ctx->configured == true && ctx->calib_request == true && ctx->calib_errcode == E_AGAIN) {
          ctx->calib_fsm = CJ125_CALIB_INIT_READ;
          ctx->request.bytes[0] = CJ125_REG_RD_INIT1;
          ctx->data_lambda_valid = 0;
          ctx->data_temp_valid = 0;
          err = E_AGAIN;
          continue;
        } else {
          err = E_OK;
        }
        break;
      case CJ125_CALIB_INIT_READ:
        err = cj125_serial_operation(ctx, ctx->request, &ctx->response);
        if(err == E_OK) {
          ctx->calib_init1_byte = ctx->response.bytes[1];
          ctx->calib_fsm = CJ125_CALIB_INIT_WRITE;
          reg_init1.data = ctx->calib_init1_byte;
          reg_init1.bits.la = 1;
          reg_init1.bits.ra = 1;
          ctx->data.ampfactor = reg_init1.bits.vl;
          ctx->request.bytes[0] = CJ125_REG_WR_INIT1;
          ctx->request.bytes[1] = reg_init1.data;
          err = E_AGAIN;
          continue;
        } else if(err != E_AGAIN) {
          ctx->calib_fsm = CJ125_CALIB_CONDITION;
          ctx->calib_errcode = err;
        }
        break;
      case CJ125_CALIB_INIT_WRITE:
        err = cj125_serial_operation(ctx, ctx->request, &ctx->response);
        if(err == E_OK) {
          ctx->calib_fsm = CJ125_CALIB_SAMPLE;
          ctx->diag_timestamp = now;
          ctx->calib_ignored = 0;
          ctx->calib_samples = 0;
          ctx->calib_ur_voltage = 0;
          ctx->calib_ua_voltage = 0;
          ctx->calib_ref_voltage = 0;
          ctx->ur_updated = false;
          ctx->ua_updated = false;
          ctx->ref_ua_updated = false;
          ctx->ref_ur_updated = false;
          err = E_AGAIN;
          continue;
        } else if(err != E_AGAIN) {
          ctx->calib_fsm = CJ125_CALIB_CONDITION;
          ctx->calib_errcode = err;
        }
        break;
      case CJ125_CALIB_SAMPLE:
        if(ctx->calib_samples >= CJ125_CALIBRATION_MIN_SAMPLES &&
            time_diff(now, ctx->calib_timestamp) >= CJ125_CALIBRATION_MIN_PERIOD_US) {
          ctx->calib_fsm = CJ125_CALIB_INIT_RESTORE;
          reg_init1.data = ctx->calib_init1_byte;
          reg_init1.bits.la = 0;
          reg_init1.bits.ra = 0;
          ctx->request.bytes[0] = CJ125_REG_WR_INIT1;
          ctx->request.bytes[1] = reg_init1.data;
          err = E_AGAIN;
          continue;
        } else if(time_diff(now, ctx->calib_timestamp) <= CJ125_CALIBRATION_TIMEOUT_US) {
          if(ctx->ua_updated && ctx->ur_updated && ctx->ref_ua_updated && ctx->ref_ur_updated) {
            if(ctx->calib_ignored >= CJ125_CALIBRATION_SAMPLES_TO_IGNORE) {
              if(ctx->calib_samples < USHRT_MAX) {
                ctx->calib_ua_voltage += ctx->data.ua_voltage;
                ctx->calib_ur_voltage += ctx->data.ur_voltage;
                ctx->calib_ref_voltage += ctx->data.ref_voltage;
                ctx->calib_samples++;
              }
            } else {
              ctx->calib_ignored++;
            }
            ctx->ua_updated = false;
            ctx->ur_updated = false;
            ctx->ref_ua_updated = false;
            ctx->ref_ur_updated = false;
          }
          err = E_AGAIN;
        } else {
          err = E_TIMEOUT;
          ctx->calib_fsm = CJ125_CALIB_CONDITION;
          ctx->calib_errcode = err;
        }
        break;
      case CJ125_CALIB_INIT_RESTORE:
        err = cj125_serial_operation(ctx, ctx->request, &ctx->response);
        if(err == E_OK) {
          ctx->calib_fsm = CJ125_CALIB_CALCULATE;
          err = E_AGAIN;
          continue;
        } else if(err != E_AGAIN) {
          ctx->calib_fsm = CJ125_CALIB_CONDITION;
          ctx->calib_errcode = err;
        }
        break;
      case CJ125_CALIB_CALCULATE:
        prim = EnterCritical();
        ctx->calib_ua_voltage /= ctx->calib_samples;
        ctx->calib_ur_voltage /= ctx->calib_samples;
        ctx->calib_ref_voltage /= ctx->calib_samples;

        ctx->data.ua_voltage = ctx->calib_ua_voltage;
        ctx->data.ur_voltage = ctx->calib_ur_voltage;
        ctx->data.ref_voltage = ctx->calib_ref_voltage;

        ctx->data.lambda_radj = 0.3f;

        err = cj125_update_data(ctx, true);
        if(err == E_OK) {
          ctx->data.heat_ref_voltage = ctx->calib_ur_voltage;
          ctx->data.heat_ref_temp = ctx->data.temp_value;
          ctx->data.heat_ref_resistance = ctx->data.heat_resistance;

          ctx->data.lambda_ref_voltage = ctx->calib_ua_voltage;
          ctx->data.lambda_radj = ctx->calib_ua_voltage / ctx->calib_ref_voltage;

          err = E_OK;
          ctx->calib_fsm = CJ125_CALIB_CONDITION;
          ctx->calib_errcode = err;
          ctx->calibrated = true;
        } else {
          ctx->calib_fsm = CJ125_CALIB_CONDITION;
          ctx->calib_errcode = err;
        }

        ctx->ua_updated = false;
        ctx->ur_updated = false;
        ctx->ref_ua_updated = false;
        ctx->ref_ur_updated = false;

        ExitCritical(prim);
        break;
      default:
        break;
    }
    break;
  }

  return err;
}

error_t cj125_fsm(cj125_ctx_t *ctx)
{
  error_t err;

  do {
    err = E_OK;

    switch(ctx->process_fsm) {
      case CJ125_PROCESS_RESET:
        err = cj125_fsm_reset(ctx);
        break;
      case CJ125_PROCESS_DIAG:
        //err = cj125_fsm_diagoff(ctx);
        break;
      case CJ125_PROCESS_CONFIGURE:
        //err = cj125_fsm_configure(ctx);
        break;
      case CJ125_PROCESS_CALIBRATE:
        err = cj125_fsm_calibrate(ctx);
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
