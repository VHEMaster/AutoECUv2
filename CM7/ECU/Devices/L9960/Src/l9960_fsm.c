/*
 * l9960_fsm.c
 *
 *  Created on: Apr 17, 2024
 *      Author: VHEMaster
 */

#include "l9960_internal.h"
#include "l9960_fsm.h"
#include "l9960_reg.h"
#include "l9960_config.h"
#include "compiler.h"
#include "errors.h"
#include "time.h"

static error_t l9960_fsm_check_status(l9960_ctx_t *ctx)
{
  error_t err;
  time_us_t now;
  l9960_resp_oc_t resp_oc;


  while(true) {
    err = E_AGAIN;
    now = time_get_current_us();

    switch(ctx->status_fsm_state) {
      case L9960_STATUS_CONDITION:
        if(ctx->initialized == true && time_diff(now, ctx->status_timestamp) >= L9960_STATUS_POLL_PERIOD_UD) {
          ctx->status_fsm_state = L9960_STATUS_REQUEST_OC;
          ctx->request.bits.addr = L9960_REG_ADDR_OC_MONITOR;
          ctx->request.bits.data = L9960_REG_DATA_NONE;
          err = E_AGAIN;
          continue;
        } else {
          err = E_OK;
        }
        break;
      case L9960_STATUS_REQUEST_OC:
        err = l9960_serial_operation(ctx, ctx->request, NULL);
        if(err == E_OK) {
          ctx->status_fsm_state = L9960_STATUS_REQUEST1;
          ctx->request.bits.addr = L9960_REG_ADDR_CONFIG_REQ;
          ctx->request.bits.data = L9960_REG_DATA_4;
          err = E_AGAIN;
          continue;
        }
        break;
      case L9960_STATUS_REQUEST1:
        err = l9960_serial_operation(ctx, ctx->request, &ctx->response);
        if(err == E_OK) {
          resp_oc.data = ctx->response.bits.data;
          ctx->status.overcurrent.bits.ocl0 = resp_oc.bits.ocl0;
          ctx->status.overcurrent.bits.och0 = resp_oc.bits.och0;
          ctx->status.overcurrent.bits.ocl1 = resp_oc.bits.ocl1;
          ctx->status.overcurrent.bits.och1 = resp_oc.bits.och1;

          ctx->status_fsm_state = L9960_STATUS_REQUEST2;
          ctx->request.bits.addr = L9960_REG_ADDR_STATUS_REQ;
          ctx->request.bits.data = L9960_REG_DATA_NONE;
          err = E_AGAIN;
          continue;
        }
        break;
      case L9960_STATUS_REQUEST2:
        err = l9960_serial_operation(ctx, ctx->request, &ctx->response);
        if(err == E_OK) {
          ctx->status.regs.config5.data = ctx->response.bits.data;

          ctx->status_fsm_state = L9960_STATUS_REQUEST3;
          ctx->request.bits.addr = L9960_REG_ADDR_STATUS_REQ;
          ctx->request.bits.data = L9960_REG_DATA_0;
          err = E_AGAIN;
          continue;
        }
        break;
      case L9960_STATUS_REQUEST3:
        err = l9960_serial_operation(ctx, ctx->request, &ctx->response);
        if(err == E_OK) {
          ctx->status.regs.states1.data = ctx->response.bits.data;

          ctx->status_fsm_state = L9960_STATUS_REQUEST4;
          ctx->request.bits.addr = L9960_REG_ADDR_STATUS_REQ;
          ctx->request.bits.data = L9960_REG_DATA_1;
          err = E_AGAIN;
          continue;
        }
        break;
      case L9960_STATUS_REQUEST4:
        err = l9960_serial_operation(ctx, ctx->request, &ctx->response);
        if(err == E_OK) {
          ctx->status.regs.states2.data = ctx->response.bits.data;

          ctx->status_fsm_state = L9960_STATUS_REQUEST5;
          ctx->request.bits.addr = L9960_REG_ADDR_STATUS_REQ;
          ctx->request.bits.data = L9960_REG_DATA_NONE;
          err = E_AGAIN;
          continue;
        }
        break;
      case L9960_STATUS_REQUEST5:
        err = l9960_serial_operation(ctx, ctx->request, &ctx->response);
        if(err == E_OK) {
          ctx->status.regs.states3.data = ctx->response.bits.data;

          ctx->status_fsm_state = L9960_STATUS_CONDITION;
          ctx->status_timestamp = now;
          ctx->status_valid = true;
          err = E_OK;
        }
        break;
      default:
        break;
    }
    break;
  }

  return err;
}

static error_t l9960_fsm_reset(l9960_ctx_t *ctx)
{
  error_t err;
  l9960_resp_eid_t eid;
  l9960_resp_sv_t sv;
  l9960_resp_lhwv_t lhwv;
  l9960_resp_trace1_t trace1;
  l9960_resp_trace2_t trace2;
  l9960_cmd_restart_trigger_t restart_trigger;
  l9960_cmd_off_state_diag_t diag_req;
  l9960_resp_osd_t diag_resp;
  time_us_t now;

  while(true) {
    err = E_AGAIN;
    now = time_get_current_us();

    switch(ctx->reset_fsm_state) {
      case L9960_RESET_CONDITION:
        if(ctx->reset_request == true && ctx->reset_errcode == E_AGAIN) {
          l9960_set_enabled(ctx, false);
          ctx->reset_fsm_state = L9960_RESET_VERSION_REQUEST1;
          ctx->request.bits.addr = L9960_REG_ADDR_ID_REQ;
          ctx->request.bits.data = L9960_REG_DATA_NONE;
          continue;
        } else {
          err = E_OK;
        }
        break;
      case L9960_RESET_VERSION_REQUEST1:
        err = l9960_serial_operation(ctx, ctx->request, NULL);
        if(err == E_OK) {

          ctx->reset_fsm_state = L9960_RESET_VERSION_REQUEST2;
          ctx->request.bits.addr = L9960_REG_ADDR_ID_REQ;
          ctx->request.bits.data = L9960_REG_DATA_0;
          err = E_AGAIN;
          continue;
        }
        break;
      case L9960_RESET_VERSION_REQUEST2:
        err = l9960_serial_operation(ctx, ctx->request, &ctx->response);
        if(err == E_OK) {
          eid.data = ctx->response.bits.data;
          ctx->version.electronic_id = eid.bits.asic_name;

          ctx->reset_fsm_state = L9960_RESET_VERSION_REQUEST3;
          ctx->request.bits.addr = L9960_REG_ADDR_ID_REQ;
          ctx->request.bits.data = L9960_REG_DATA_1;
          err = E_AGAIN;
          continue;
        }
        break;
      case L9960_RESET_VERSION_REQUEST3:
        err = l9960_serial_operation(ctx, ctx->request, &ctx->response);
        if(err == E_OK) {
          sv.data = ctx->response.bits.data;
          ctx->version.silicon_version = sv.bits.silicon_version;
          ctx->version.supplier = sv.bits.supplier;

          ctx->reset_fsm_state = L9960_RESET_VERSION_REQUEST4;
          ctx->request.bits.addr = L9960_REG_ADDR_COMP_TRACE_REQ;
          ctx->request.bits.data = L9960_REG_DATA_NONE;
          err = E_AGAIN;
          continue;
        }
        break;
      case L9960_RESET_VERSION_REQUEST4:
        err = l9960_serial_operation(ctx, ctx->request, &ctx->response);
        if(err == E_OK) {
          lhwv.data = ctx->response.bits.data;
          ctx->version.logic_hw_version = lhwv.bits.code_version;

          ctx->reset_fsm_state = L9960_RESET_VERSION_REQUEST5;
          ctx->request.bits.addr = L9960_REG_ADDR_COMP_TRACE_REQ;
          ctx->request.bits.data = L9960_REG_DATA_0;
          err = E_AGAIN;
          continue;
        }
        break;
      case L9960_RESET_VERSION_REQUEST5:
        err = l9960_serial_operation(ctx, ctx->request, &ctx->response);
        if(err == E_OK) {
          trace1.data = ctx->response.bits.data;
          ctx->version.wafer_x = trace1.bits.wafer_x;
          ctx->version.wafer_y = trace1.bits.wafer_y;

          ctx->reset_fsm_state = L9960_RESET_VERSION_DIAG_OFF_STATE;
          ctx->request.bits.addr = L9960_REG_ADDR_OFF_STATE_DIAG;
          ctx->request.bits.data = 0;
          err = E_AGAIN;
          continue;
        }
        break;
      case L9960_RESET_VERSION_DIAG_OFF_STATE:
        err = l9960_serial_operation(ctx, ctx->request, &ctx->response);
        if(err == E_OK) {
          trace2.data = ctx->response.bits.data;
          ctx->version.wafer_number = trace2.bits.wafer_number;

          ctx->reset_fsm_state = L9960_RESET_VERSION_VERIFY;
          err = E_AGAIN;
          continue;
        }
        break;
      case L9960_RESET_VERSION_VERIFY:
        if(ctx->version.electronic_id == L9966_ELECTRONIC_ID) {
          ctx->version_valid = true;
          err = E_AGAIN;

          ctx->reset_fsm_state = L9960_RESET_REQUEST;
          restart_trigger.data = 0;
          restart_trigger.bits.config_cc = ctx->config.comm_check;
          restart_trigger.bits.hwsc_libst_trig = 0;
          restart_trigger.bits.sw_reset = 1;
          ctx->request.bits.addr = L9960_REG_ADDR_RESTART_TRIGGER;
          ctx->request.bits.data = restart_trigger.data;
          continue;
        } else {
          err = E_BADRESP;
          ctx->reset_fsm_state = L9960_RESET_CONDITION;
          ctx->reset_errcode = err;
        }
        break;
      case L9960_RESET_REQUEST:
        err = l9960_serial_operation(ctx, ctx->request, NULL);
        if(err == E_OK) {
          ctx->reset_fsm_state = L9960_RESET_WAIT;
          ctx->reset_timestamp = now;
        }
        break;
      case L9960_RESET_WAIT:
        if(time_diff(now, ctx->reset_timestamp) >= L9966_RESET_WAIT_US) {
          ctx->reset_fsm_state = L9960_RESET_DIAG_OFF_STATE;

          diag_req.data = 0;
          diag_req.bits.trig = 1;

          ctx->request.bits.addr = L9960_REG_ADDR_OFF_STATE_DIAG;
          ctx->request.bits.data = diag_req.data;
          err = E_AGAIN;
          continue;
        }
        break;
      case L9960_RESET_DIAG_OFF_STATE:
        err = l9960_serial_operation(ctx, ctx->request, &ctx->response);
        if(err == E_OK) {
          diag_resp.data = ctx->response.bits.data;
          switch(diag_resp.bits.diag_off) {
            case L9960_REG_DIAG_OFF_OPENLOAD:
            case L9960_REG_DIAG_OFF_SHORTBATT:
            case L9960_REG_DIAG_OFF_SHORTGND:
              err = E_FAULT;
              break;
            case L9960_REG_DIAG_OFF_NOFAILURE:
              err = E_OK;
              break;
            case L9960_REG_DIAG_OFF_NOTRIGGER_INCORRECT:
            case L9960_REG_DIAG_OFF_NOTRIGGER_THZNOTEXPIRED:
            case L9960_REG_DIAG_OFF_NOTDONE:
            case L9960_REG_DIAG_OFF_NOTUSED:
              err = E_AGAIN;
              break;
            default:
              break;
          }

          if(err == E_OK) {
            ctx->reset_fsm_state = L9960_RESET_DIAG_OFF_DISABLE;

            diag_req.data = 0;
            ctx->request.bits.addr = L9960_REG_ADDR_OFF_STATE_DIAG;
            ctx->request.bits.data = diag_req.data;
            err = E_AGAIN;
            continue;
          } else if(err == E_AGAIN) {
            ctx->reset_timestamp = now;
            ctx->reset_fsm_state = L9960_RESET_WAIT;
            err = E_OK;
          } else {
            ctx->reset_errcode = err;
            ctx->reset_fsm_state = L9960_RESET_CONDITION;
          }
        }
        break;
      case L9960_RESET_DIAG_OFF_DISABLE:
        err = l9960_serial_operation(ctx, ctx->request, NULL);
        if(err == E_OK) {
          ctx->initialized = true;
          ctx->status_timestamp = now;
          ctx->reset_fsm_state = L9960_RESET_CONDITION;
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

error_t l9960_fsm(l9960_ctx_t *ctx)
{
  error_t err;

  do {
    err = E_OK;

    switch(ctx->process_fsm) {
      case L9960_PROCESS_RESET:
        err = l9960_fsm_reset(ctx);
        break;
      case L9960_PROCESS_CHECK_STATUS:
        err = l9960_fsm_check_status(ctx);
        break;
      case L9960_PROCESS_CONFIGURE:
        //err = l9960_fsm_configure(ctx);
        break;
      default:
        break;
    }

    if(err == E_OK) {
      if(++ctx->process_fsm >= L9960_PROCESS_MAX) {
        ctx->process_fsm = 0;
        break;
      }
    }
  } while(0);

  return err;
}
