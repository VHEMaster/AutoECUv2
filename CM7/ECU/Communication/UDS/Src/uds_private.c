/*
 * uds_private.c
 *
 *  Created on: Nov 2, 2025
 *      Author: VHEMaster
 */

#include "uds_private.h"

static void uds_sid_diagnostic_session_control(uds_ctx_t *ctx);

void uds_loop_handler(uds_ctx_t *ctx)
{
  time_us_t now;

  do {
    now = time_now_us();

    if(ctx->reset_trigger) {
      ctx->downstream_available = false;
      ctx->upstream_available = false;
      ctx->error_code = UDS_OK;
      ctx->reset_trigger = false;
      break;
    }

    if(!ctx->downstream_available) {
      if(ctx->upstream_available) {

        ctx->downstream_data_len = 0;
        ctx->downstream_data[ctx->downstream_data_len++] = UDS_RESPONSE_POSITIVE_OFFSET + ctx->upstream_data[0];

        switch(ctx->upstream_data[0]) {
          case UDS_SID_DIAGNOSTIC_SESSION_CONTROL:
            uds_sid_diagnostic_session_control(ctx);
            break;
          default:
            ctx->downstream_data_len = 0;
            ctx->downstream_data[ctx->downstream_data_len++] = UDS_RESPONSE_NEGATIVE_CODE;
            ctx->downstream_data[ctx->downstream_data_len++] = ctx->upstream_data[0];
            ctx->downstream_data[ctx->downstream_data_len++] = UDS_RESPONSE_SERVICE_NOT_SUPPORTED;
            ctx->downstream_available = true;
            break;
        }

        ctx->upstream_available = false;
        ctx->downstream_time = now;
      }
    } else if(time_diff(now, ctx->downstream_time) >= UDS_DOWNSTREAM_TIMEOUT) {
      ctx->reset_trigger = true;
      continue;
    }
  } while(0);
}

static void uds_sid_diagnostic_session_control(uds_ctx_t *ctx)
{
  do {
    if(ctx->downstream_data_len <= 1) {
      ctx->downstream_data_len = 0;
      ctx->downstream_data[ctx->downstream_data_len++] = UDS_RESPONSE_NEGATIVE_CODE;
      ctx->downstream_data[ctx->downstream_data_len++] = ctx->upstream_data[0];
      ctx->downstream_data[ctx->downstream_data_len++] = UDS_RESPONSE_SERVICE_NOT_SUPPORTED;
    }
    ctx->downstream_available = true;
  } while(0);
}
