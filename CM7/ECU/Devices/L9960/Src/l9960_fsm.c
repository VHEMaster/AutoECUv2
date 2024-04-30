/*
 * l9960_fsm.c
 *
 *  Created on: Apr 17, 2024
 *      Author: VHEMaster
 */

#include "l9960_internal.h"
#include "l9960_fsm.h"
#include "l9960_reg.h"
#include "l9960.h"
#include "l9960_cfg_reg.h"
#include "compiler.h"
#include "errors.h"
#include "time.h"

static error_t l9960_fsm_check_status(l9960_ctx_t *ctx)
{
  error_t err;
  time_us_t now;


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
        } else if(err != E_AGAIN) {
          ctx->status_valid = false;
          ctx->status_fsm_state = L9960_STATUS_CONDITION;
          ctx->status_timestamp = now;
        }
        break;
      case L9960_STATUS_REQUEST1:
        err = l9960_serial_operation(ctx, ctx->request, &ctx->response);
        if(err == E_OK) {
          ctx->status.overcurrent.data = ctx->response.bits.data;

          ctx->status_fsm_state = L9960_STATUS_REQUEST2;
          ctx->request.bits.addr = L9960_REG_ADDR_STATUS_REQ;
          ctx->request.bits.data = L9960_REG_DATA_NONE;
          err = E_AGAIN;
          continue;
        } else if(err != E_AGAIN) {
          ctx->status_valid = false;
          ctx->status_fsm_state = L9960_STATUS_CONDITION;
          ctx->status_timestamp = now;
        }
        break;
      case L9960_STATUS_REQUEST2:
        err = l9960_serial_operation(ctx, ctx->request, &ctx->response);
        if(err == E_OK) {
          ctx->status.config5.data = ctx->response.bits.data;

          ctx->status_fsm_state = L9960_STATUS_REQUEST3;
          ctx->request.bits.addr = L9960_REG_ADDR_STATUS_REQ;
          ctx->request.bits.data = L9960_REG_DATA_0;
          err = E_AGAIN;
          continue;
        } else if(err != E_AGAIN) {
          ctx->status_valid = false;
          ctx->status_fsm_state = L9960_STATUS_CONDITION;
          ctx->status_timestamp = now;
        }
        break;
      case L9960_STATUS_REQUEST3:
        err = l9960_serial_operation(ctx, ctx->request, &ctx->response);
        if(err == E_OK) {
          ctx->status.states1.data = ctx->response.bits.data;
          ctx->status_hwsc_update = true;

          ctx->status_fsm_state = L9960_STATUS_REQUEST4;
          ctx->request.bits.addr = L9960_REG_ADDR_STATUS_REQ;
          ctx->request.bits.data = L9960_REG_DATA_1;
          err = E_AGAIN;
          continue;
        } else if(err != E_AGAIN) {
          ctx->status_valid = false;
          ctx->status_fsm_state = L9960_STATUS_CONDITION;
          ctx->status_timestamp = now;
        }
        break;
      case L9960_STATUS_REQUEST4:
        err = l9960_serial_operation(ctx, ctx->request, &ctx->response);
        if(err == E_OK) {
          ctx->status.states2.data = ctx->response.bits.data;

          ctx->status_fsm_state = L9960_STATUS_REQUEST5;
          ctx->request.bits.addr = L9960_REG_ADDR_STATUS_REQ;
          ctx->request.bits.data = L9960_REG_DATA_NONE;
          err = E_AGAIN;
          continue;
        } else if(err != E_AGAIN) {
          ctx->status_valid = false;
          ctx->status_fsm_state = L9960_STATUS_CONDITION;
          ctx->status_timestamp = now;
        }
        break;
      case L9960_STATUS_REQUEST5:
        err = l9960_serial_operation(ctx, ctx->request, &ctx->response);
        if(err == E_OK) {
          ctx->status.states3.data = ctx->response.bits.data;

          ctx->diag.bits.ol = ctx->status.states2.bits.ol_on_status ^ 3;
          ctx->diag.bits.ocl0 = ctx->status.overcurrent.bits.ocl0 ^ 2;
          ctx->diag.bits.och0 = ctx->status.overcurrent.bits.och0 ^ 2;
          ctx->diag.bits.ocl1 = ctx->status.overcurrent.bits.ocl1 ^ 2;
          ctx->diag.bits.och1 = ctx->status.overcurrent.bits.och1 ^ 2;
          ctx->diag.bits.cc = ctx->status.config5.bits.cc_latch_state ^ 1;
          ctx->diag.bits.ov_5v = ctx->status.states1.bits.vdd_ov_reg;
          ctx->diag.bits.uv_5v = ctx->status.states1.bits.vdd_uv_reg;
          ctx->diag.bits.uv_12v = ctx->status.states1.bits.vps_uv_reg;
          ctx->diag.bits.ot_warn = ctx->status.states2.bits.otwarn_reg;
          ctx->diag.bits.ot_shutdown = ctx->status.states2.bits.notsd_reg ^ 1;

          ctx->status_fsm_state = L9960_STATUS_CONDITION;
          ctx->status_timestamp = now;
          ctx->status_valid = true;
          ctx->diag_valid = true;
          err = E_OK;
        } else if(err != E_AGAIN) {
          ctx->status_valid = false;
          ctx->status_fsm_state = L9960_STATUS_CONDITION;
          ctx->status_timestamp = now;
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
  time_us_t now;

  while(true) {
    err = E_AGAIN;
    now = time_get_current_us();

    switch(ctx->reset_fsm_state) {
      case L9960_RESET_CONDITION:
        if(ctx->reset_request == true && ctx->reset_errcode == E_AGAIN) {
          l9960_internal_set_enabled(ctx, false);
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
        } else if(err != E_AGAIN) {
          ctx->reset_errcode = err;
          ctx->reset_fsm_state = L9960_RESET_CONDITION;
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
        } else if(err != E_AGAIN) {
          ctx->reset_errcode = err;
          ctx->reset_fsm_state = L9960_RESET_CONDITION;
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
        } else if(err != E_AGAIN) {
          ctx->reset_errcode = err;
          ctx->reset_fsm_state = L9960_RESET_CONDITION;
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
        } else if(err != E_AGAIN) {
          ctx->reset_errcode = err;
          ctx->reset_fsm_state = L9960_RESET_CONDITION;
        }
        break;
      case L9960_RESET_VERSION_REQUEST5:
        err = l9960_serial_operation(ctx, ctx->request, &ctx->response);
        if(err == E_OK) {
          trace1.data = ctx->response.bits.data;
          ctx->version.wafer_x = trace1.bits.wafer_x;
          ctx->version.wafer_y = trace1.bits.wafer_y;

          ctx->reset_fsm_state = L9960_RESET_TRIGGER;
          restart_trigger.data = 0;
          restart_trigger.bits.config_cc = ctx->config.comm_check;
          restart_trigger.bits.sw_reset = 1;
          ctx->request.bits.addr = L9960_REG_ADDR_RESTART_TRIGGER;
          ctx->request.bits.data = restart_trigger.data;
          err = E_AGAIN;
          continue;
        } else if(err != E_AGAIN) {
          ctx->reset_errcode = err;
          ctx->reset_fsm_state = L9960_RESET_CONDITION;
        }
        break;
      case L9960_RESET_TRIGGER:
        err = l9960_serial_operation(ctx, ctx->request, &ctx->response);
        if(err == E_OK) {
          trace2.data = ctx->response.bits.data;
          ctx->version.wafer_number = trace2.bits.wafer_number;

          ctx->reset_fsm_state = L9960_RESET_VERSION_VERIFY;
          err = E_AGAIN;
          continue;
        } else if(err != E_AGAIN) {
          ctx->reset_errcode = err;
          ctx->reset_fsm_state = L9960_RESET_CONDITION;
        }
        break;
      case L9960_RESET_VERSION_VERIFY:
        if(ctx->version.electronic_id == L9966_ELECTRONIC_ID) {
          err = E_OK;
          ctx->version_valid = true;
          ctx->initialized = true;
          ctx->status_timestamp = now;
          ctx->reset_fsm_state = L9960_RESET_CONDITION;
          ctx->reset_errcode = err;
        } else {
          err = E_BADRESP;
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

static error_t l9960_fsm_diagoff(l9960_ctx_t *ctx)
{
  error_t err;
  l9960_cmd_off_state_diag_t diag_req;
  l9960_resp_osd_t diag_resp;
  time_us_t now;

  while(true) {
    err = E_AGAIN;
    now = time_get_current_us();

    switch(ctx->diagoff_fsm_state) {
      case L9960_DIAGOFF_CONDITION:
        if(ctx->initialized == true && ctx->diagoff_request == true && ctx->diagoff_errcode == E_AGAIN) {
          l9960_internal_set_enabled(ctx, false);
          ctx->diagoff_fsm_state = L9960_DIAGOFF_INITIAL;
          diag_req.data = 0;
          diag_req.bits.trig = 1;
          ctx->request.bits.addr = L9960_REG_ADDR_OFF_STATE_DIAG;
          ctx->request.bits.data = diag_req.data;
          err = E_AGAIN;
          continue;
        } else {
          err = E_OK;
        }
        break;
      case L9960_DIAGOFF_INITIAL:
        err = l9960_serial_operation(ctx, ctx->request, NULL);
        if(err == E_OK) {
          ctx->diagoff_fsm_state = L9960_DIAGOFF_WAIT;
          ctx->diagoff_timestamp = now;
          ctx->diagoff_started = now;
        } else if(err != E_AGAIN) {
          ctx->diagoff_errcode = err;
          ctx->diagoff_errcode = L9960_DIAGOFF_CONDITION;
        }
        err = E_AGAIN;
        break;
      case L9960_DIAGOFF_WAIT:
        err = E_AGAIN;
        if(time_diff(now, ctx->diagoff_timestamp) >= L9966_DIAGOFF_WAIT_US) {
          ctx->diagoff_fsm_state = L9960_DIAGOFF_REQUEST;

          diag_req.data = 0;
          diag_req.bits.trig = 1;
          ctx->request.bits.addr = L9960_REG_ADDR_OFF_STATE_DIAG;
          ctx->request.bits.data = diag_req.data;
          continue;
        }
        break;
      case L9960_DIAGOFF_REQUEST:
        err = l9960_serial_operation(ctx, ctx->request, &ctx->response);
        if(err == E_OK) {
          diag_resp.data = ctx->response.bits.data;
          ctx->diagoff_status = diag_resp.bits.diag_off;
          switch(ctx->diagoff_status) {
            case L9960_REG_DIAGOFF_OPENLOAD:
            case L9960_REG_DIAGOFF_SHORTBATT:
            case L9960_REG_DIAGOFF_SHORTGND:
              err = E_FAULT;
              break;
            case L9960_REG_DIAGOFF_NOFAILURE:
              err = E_OK;
              break;
            case L9960_REG_DIAGOFF_NOTRIGGER_INCORRECT:
            case L9960_REG_DIAGOFF_NOTRIGGER_THZNOTEXPIRED:
            case L9960_REG_DIAGOFF_NOTDONE:
            case L9960_REG_DIAGOFF_NOTUSED:
              err = E_AGAIN;
              break;
            default:
              break;
          }

          if(err == E_OK) {
            ctx->diagoff_errcode = err;
            ctx->diagoff_fsm_state = L9960_DIAGOFF_DISABLE;
            err = E_AGAIN;
            continue;
          } else if(err == E_AGAIN) {
            if(time_diff(now, ctx->diagoff_started) >= L9966_DIAGOFF_TIMEOUT) {
              err = E_TIMEOUT;
              ctx->diagoff_errcode = err;
              ctx->diagoff_fsm_state = L9960_DIAGOFF_DISABLE;
              continue;
            } else {
              ctx->diagoff_timestamp = now;
              ctx->diagoff_fsm_state = L9960_DIAGOFF_WAIT;
            }
          } else {
            ctx->diagoff_errcode = err;
            ctx->diagoff_fsm_state = L9960_DIAGOFF_DISABLE;
            continue;
          }
        } else if(err != E_AGAIN) {
          ctx->diagoff_errcode = err;
          ctx->diagoff_errcode = L9960_DIAGOFF_CONDITION;
        }
        err = E_AGAIN;
        break;
      case L9960_DIAGOFF_DISABLE:
        diag_req.data = 0;
        ctx->diagoff_fsm_state = L9960_DIAGOFF_DISABLE_REQUEST;
        ctx->request.bits.addr = L9960_REG_ADDR_OFF_STATE_DIAG;
        ctx->request.bits.data = diag_req.data;
        err = E_AGAIN;
        continue;
      case L9960_DIAGOFF_DISABLE_REQUEST:
        err = l9960_serial_operation(ctx, ctx->request, NULL);
        if(err == E_OK) {
          ctx->diagoff_fsm_state = L9960_DIAGOFF_CONDITION;
        } else if(err != E_AGAIN) {
          ctx->diagoff_errcode = L9960_DIAGOFF_CONDITION;
        }
        break;
      default:
        break;
    }
    break;
  }

  return err;
}

static error_t l9960_fsm_hwsc(l9960_ctx_t *ctx)
{
  error_t err;
  l9960_cmd_restart_trigger_t hwsc_req;
  time_us_t now;

  while(true) {
    err = E_AGAIN;
    now = time_get_current_us();

    switch(ctx->hwsc_fsm_state) {
      case L9960_HWSC_CONDITION:
        if(ctx->initialized == true && ctx->hwsc_request == true && ctx->hwsc_errcode == E_AGAIN) {
          l9960_internal_set_enabled(ctx, false);
          ctx->hwsc_fsm_state = L9960_HWSC_INITIAL;

          hwsc_req.data = 0;
          hwsc_req.bits.config_cc = ctx->config.comm_check;
          hwsc_req.bits.hwsc_libst_trig = 1;
          ctx->request.bits.addr = L9960_REG_ADDR_RESTART_TRIGGER;
          ctx->request.bits.data = hwsc_req.data;
          err = E_AGAIN;
          continue;
        } else {
          err = E_OK;
        }
        break;
      case L9960_HWSC_INITIAL:
        err = l9960_serial_operation(ctx, ctx->request, NULL);
        if(err == E_OK) {
          ctx->hwsc_fsm_state = L9960_HWSC_STATUS;
          ctx->status_hwsc_update = false;
          ctx->hwsc_started = now;
        } else if(err != E_AGAIN) {
          ctx->hwsc_errcode = err;
          ctx->hwsc_fsm_state = L9960_HWSC_CONDITION;
        }
        break;
      case L9960_HWSC_STATUS:
        if(ctx->status_hwsc_update) {
          ctx->status_hwsc_update = false;
          ctx->hwsc_status = ctx->status.states1.bits.hwsc_lbist_status;
          switch(ctx->hwsc_status) {
            case L9960_REG_HWSC_FAIL:
            case L9960_REG_HWSC_FAIL_LBIST_PASS:
              err = E_FAULT;
              break;
            case L9960_REG_HWSC_PASS:
              err = E_OK;
              break;
            case L9960_REG_HWSC_NOTDONE0:
            case L9960_REG_HWSC_NOTDONE1:
            case L9960_REG_HWSC_NOTDONE2:
            case L9960_REG_HWSC_NOTDONE3:
            case L9960_REG_HWSC_RUN_LBIST_PASS:
              err = E_AGAIN;
              break;
            default:
              break;
          }

          if(err == E_OK) {
            ctx->hwsc_errcode = err;
            ctx->hwsc_fsm_state = L9960_HWSC_DISABLE;
            err = E_AGAIN;
            continue;
          } else if(err == E_AGAIN) {
            if(time_diff(now, ctx->hwsc_started) >= L9966_HWSC_TIMEOUT) {
              err = E_TIMEOUT;
              ctx->hwsc_errcode = err;
              ctx->hwsc_fsm_state = L9960_HWSC_DISABLE;
              continue;
            } else {
              ctx->hwsc_fsm_state = L9960_HWSC_STATUS;
            }
          } else {
            ctx->hwsc_errcode = err;
            ctx->hwsc_fsm_state = L9960_HWSC_DISABLE;
            continue;
          }
        }
        err = E_OK;
        break;
      case L9960_HWSC_DISABLE:
        hwsc_req.data = 0;
        hwsc_req.bits.config_cc = ctx->config.comm_check;
        ctx->hwsc_fsm_state = L9960_HWSC_DISABLE_REQUEST;
        ctx->request.bits.addr = L9960_REG_ADDR_RESTART_TRIGGER;
        ctx->request.bits.data = hwsc_req.data;
        err = E_AGAIN;
        continue;
      case L9960_HWSC_DISABLE_REQUEST:
        err = l9960_serial_operation(ctx, ctx->request, NULL);
        if(err == E_OK) {
          ctx->hwsc_fsm_state = L9960_HWSC_CONDITION;
        } else if(err != E_AGAIN) {
          ctx->hwsc_fsm_state = L9960_HWSC_CONDITION;
        }
        break;
      default:
        break;
    }
    break;
  }

  return err;
}

static error_t l9960_fsm_configure(l9960_ctx_t *ctx)
{
  error_t err;

  while(true) {
    err = E_AGAIN;

    switch(ctx->config_fsm_state) {
      case L9960_CONFIG_CONDITION:
        if(ctx->initialized == true && ctx->config_request == true && ctx->config_errcode == E_AGAIN) {
          ctx->config_fsm_state = L9960_CONFIG_TRANSLATE;
          err = E_AGAIN;
          continue;
        } else {
          err = E_OK;
        }
        break;
      case L9960_CONFIG_TRANSLATE:
        err = l9960_cfg_reg_translate(ctx);
        if(err == E_OK) {
          ctx->config_fsm_state = L9960_CONFIG_REQUEST1;
          ctx->request.bits.addr = L9960_REG_ADDR_CONFIG_1;
          ctx->request.bits.data = ctx->reg_cfg.config1.data;
          err = E_AGAIN;
          continue;
        } else {
          ctx->config_errcode = err;
          ctx->config_fsm_state = L9960_CONFIG_CONDITION;
        }
        break;
      case L9960_CONFIG_REQUEST1:
        err = l9960_serial_operation(ctx, ctx->request, NULL);
        if(err == E_OK) {
          ctx->config_fsm_state = L9960_CONFIG_REQUEST2;
          ctx->request.bits.addr = L9960_REG_ADDR_CONFIG_2;
          ctx->request.bits.data =  ctx->reg_cfg.config2.data;
          err = E_AGAIN;
          continue;
        } else if(err != E_AGAIN) {
          ctx->config_errcode = err;
          ctx->config_fsm_state = L9960_CONFIG_CONDITION;
        }
        break;
      case L9960_CONFIG_REQUEST2:
        err = l9960_serial_operation(ctx, ctx->request, &ctx->response);
        if(err == E_OK) {
          if(ctx->response.bits.addr == L9960_REG_ADDR_CONFIG_1 && ctx->response.bits.data == 0) {
            ctx->config_fsm_state = L9960_CONFIG_REQUEST3;
            ctx->request.bits.addr = L9960_REG_ADDR_CONFIG_3;
            ctx->request.bits.data =  ctx->reg_cfg.config3.data;
            err = E_AGAIN;
            continue;
          } else {
            err = E_BADRESP;
            ctx->config_errcode = err;
            ctx->config_fsm_state = L9960_CONFIG_CONDITION;
          }
        } else if(err != E_AGAIN) {
          ctx->config_errcode = err;
          ctx->config_fsm_state = L9960_CONFIG_CONDITION;
        }
        break;
      case L9960_CONFIG_REQUEST3:
        err = l9960_serial_operation(ctx, ctx->request, &ctx->response);
        if(err == E_OK) {
          if(ctx->response.bits.addr == L9960_REG_ADDR_CONFIG_2 && ctx->response.bits.data == 0) {
            ctx->config_fsm_state = L9960_CONFIG_REQUEST4;
            ctx->request.bits.addr = L9960_REG_ADDR_CONFIG_4;
            ctx->request.bits.data =  ctx->reg_cfg.config4.data;
            err = E_AGAIN;
            continue;
          } else {
            err = E_BADRESP;
            ctx->config_errcode = err;
            ctx->config_fsm_state = L9960_CONFIG_CONDITION;
          }
        } else if(err != E_AGAIN) {
          ctx->config_errcode = err;
          ctx->config_fsm_state = L9960_CONFIG_CONDITION;
        }
        break;
      case L9960_CONFIG_REQUEST4:
        err = l9960_serial_operation(ctx, ctx->request, &ctx->response);
        if(err == E_OK) {
            if(ctx->response.bits.addr == L9960_REG_ADDR_CONFIG_3 && ctx->response.bits.data == 0) {
            ctx->config_errcode = err;
            ctx->config_fsm_state = L9960_CONFIG_CONDITION;
            ctx->configured = true;
          } else {
            err = E_BADRESP;
            ctx->config_errcode = err;
            ctx->config_fsm_state = L9960_CONFIG_CONDITION;
          }
        } else if(err != E_AGAIN) {
          ctx->config_errcode = err;
          ctx->config_fsm_state = L9960_CONFIG_CONDITION;
        }
        break;
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
      case L9960_PROCESS_DIAGOFF:
        err = l9960_fsm_diagoff(ctx);
        break;
      case L9960_PROCESS_HWSC:
        err = l9960_fsm_hwsc(ctx);
        break;
      case L9960_PROCESS_CONFIGURE:
        err = l9960_fsm_configure(ctx);
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
