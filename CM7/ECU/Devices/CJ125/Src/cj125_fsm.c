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

static error_t cj125_fsm_reset(cj125_ctx_t *ctx)
{
  error_t err;
  time_us_t now;

  while(true) {
    err = E_AGAIN;
    now = time_get_current_us();

    switch(ctx->reset_fsm) {
      case CJ125_RESET_CONDITION:
        if(ctx->ready == true && ctx->reset_request == true && ctx->reset_errcode == E_AGAIN) {
          if(gpio_valid(&ctx->init.nrst_pin)) {
            gpio_set(&ctx->init.nrst_pin);
          }
          ctx->reset_fsm = CJ125_RESET_IDENT;
          ctx->request.bytes[0] = CJ125_REG_RD_IDENT;
          ctx->request.bytes[1] = 0;
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
