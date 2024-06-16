/*
 * tle6240_fsm.c
 *
 *  Created on: Apr 16, 2024
 *      Author: VHEMaster
 */

#include "tle6240_fsm.h"
#include "tle6240_internal.h"
#include "compiler.h"

ITCM_FUNC error_t tle6240_fsm(tle6240_ctx_t *ctx)
{
  error_t err;
  time_us_t now;
  uint32_t prim;
  time_delta_us_t poll_period;
  uint32_t data_xor;

  while(true) {
    err = E_OK;
    now = time_get_current_us();

    switch(ctx->process_fsm) {
      case TLE6240_PROCESS_RESET:
        if(gpio_valid(&ctx->init.reset_pin)) {
          gpio_set(&ctx->init.reset_pin);
        }
        ctx->output_updated = true;
        ctx->process_fsm = TLE6240_PROCESS_CONDITION;
        err = E_OK;
        break;
      case TLE6240_PROCESS_CONDITION:
        prim = EnterCritical();
        if(ctx->poll_slow && !ctx->output_updated) {
          poll_period = ctx->poll_period_slow;
        } else {
          poll_period = ctx->poll_period_fast;
        }

        if(ctx->output_updated || time_diff(now, ctx->poll_last) >= poll_period) {
          if(ctx->output_updated) {
            ctx->poll_slow = false;
            ctx->change_last = now;
          }
          data_xor = ctx->output_temp ^ ctx->output_state;
          if((data_xor & 0x00FF) != 0 || (data_xor & 0xFF00) == 0) {
            ctx->half_initial_first = true;
          } else {
            ctx->half_initial_first = false;
          }
          ctx->output_temp = ctx->output_state;
          ctx->output_updated = false;
          ExitCritical(prim);
          ctx->process_fsm = TLE6240_PROCESS_WRITE_1_TO_8;
          ctx->ctrl = TLE6240_CTRL_CH1_CH8_OR_WRITE_OR;
          ctx->data = ctx->output_temp & 0xFF;
          err = E_AGAIN;
          continue;
        } else {
          ExitCritical(prim);
        }
        break;
      case TLE6240_PROCESS_WRITE_1_TO_8:
        err = tle6240_serial_operation(ctx, ctx->ctrl, ctx->data, &ctx->diag_hword);
        if(err == E_OK) {
          if(ctx->half_initial_first) {
            ctx->process_fsm = TLE6240_PROCESS_WRITE_9_TO_16;
          } else {
            ctx->diag.diag.hword[1] = ctx->diag_hword ^ 0xFFFF;
            ctx->process_fsm = TLE6240_PROCESS_ECHO_REQUEST;
          }
          ctx->ctrl = TLE6240_CTRL_CH9_CH16_OR_WRITE_OR;
          ctx->data = (ctx->output_temp >> 8) & 0xFF;
          err = E_AGAIN;
          continue;
        } else if(err != E_AGAIN) {
          ctx->process_fsm = TLE6240_PROCESS_RESET;
          ctx->diag.comm_status = E_IO;
        }
        break;
      case TLE6240_PROCESS_WRITE_9_TO_16:
        err = tle6240_serial_operation(ctx, ctx->ctrl, ctx->data, &ctx->diag_hword);
        if(err == E_OK) {
          if(!ctx->half_initial_first) {
            ctx->process_fsm = TLE6240_PROCESS_WRITE_9_TO_16;
          } else {
            ctx->diag.diag.hword[0] = ctx->diag_hword ^ 0xFFFF;
            ctx->process_fsm = TLE6240_PROCESS_ECHO_REQUEST;
          }
          ctx->process_fsm = TLE6240_PROCESS_ECHO_REQUEST;
          ctx->ctrl = TLE6240_CTRL_CH1_CH8_ECHO;
          err = E_AGAIN;
          continue;
        } else if(err != E_AGAIN) {
          ctx->process_fsm = TLE6240_PROCESS_RESET;
          ctx->diag.comm_status = E_IO;
        }
        break;
      case TLE6240_PROCESS_ECHO_REQUEST:
        err = tle6240_serial_operation(ctx, ctx->ctrl, ctx->data, &ctx->diag_hword);
        if(err == E_OK) {
          if(ctx->half_initial_first) {
            ctx->diag.diag.hword[1] = ctx->diag_hword ^ 0xFFFF;
          } else {
            ctx->diag.diag.hword[0] = ctx->diag_hword ^ 0xFFFF;
          }
          ctx->process_fsm = TLE6240_PROCESS_ECHO_CHECK;
          ctx->ctrl = (ctx->echo_value >> 8) & 0xFF;
          ctx->data = ctx->echo_value & 0xFF;
          err = E_AGAIN;
          continue;
        } else if(err != E_AGAIN) {
          ctx->process_fsm = TLE6240_PROCESS_RESET;
          ctx->diag.comm_status = E_IO;
        }
        break;
      case TLE6240_PROCESS_ECHO_CHECK:
        err = tle6240_serial_operation(ctx, ctx->ctrl, ctx->data, &ctx->diag_hword);
        if(err == E_OK) {
          if(ctx->diag_hword == ctx->echo_value) {
            ctx->echo_value += TLE6240_ECHO_INCREMENT_VALUE;
            err = E_OK;
          } else {
            err = E_BADRESP;
          }
          ctx->diag.comm_status = err;
          ctx->poll_last = now;
          ctx->process_fsm = TLE6240_PROCESS_CONDITION;
          if(!ctx->poll_slow && time_diff(now, ctx->change_last) >= ctx->poll_fast_to_slow) {
            ctx->poll_slow = true;
          }
        } else if(err != E_AGAIN) {
          ctx->process_fsm = TLE6240_PROCESS_RESET;
          ctx->diag.comm_status = E_IO;
        }
        break;
      default:
        break;
    }
    break;
  }

  return err;
}
