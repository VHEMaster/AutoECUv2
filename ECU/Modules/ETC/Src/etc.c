/*
 * etc.c
 *
 *  Created on: May 16, 2024
 *      Author: VHEMaster
 */

#include "etc.h"
#include "etc_fsm.h"
#include "etc_internal.h"
#include "compiler.h"
#include <string.h>

error_t etc_init(etc_ctx_t *ctx, const etc_init_ctx_t *init_ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || init_ctx == NULL, err = E_PARAM);

    memset(ctx, 0u, sizeof(etc_ctx_t));
    memcpy(&ctx->init, init_ctx, sizeof(etc_init_ctx_t));

    math_pid_init(&ctx->pid_position);
    math_pid_init(&ctx->pid_speed);

    ctx->adaptation.apply_status = E_OK;

    ctx->ready = true;

  } while(0);

  return err;
}

error_t etc_configure(etc_ctx_t *ctx, const etc_config_t *config)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || config == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    if(etc_adaptation_cancel(ctx) == E_AGAIN) {
      ctx->configured = false;
      err = E_AGAIN;
      break;
    }

    if(ctx->reset_request == false) {
      ctx->configured = false;

      if(&ctx->config != config) {
        memcpy(&ctx->config, config, sizeof(etc_config_t));
      }
      if(!etc_internal_adaptation_config_valid(&ctx->config.adaptation)) {
        memset(&ctx->config.adaptation, 0u, sizeof(ctx->config.adaptation));
      }

      if(ctx->config.enabled == true) {
        err = ecu_config_gpio_input_get_id(ctx->config.power_voltage_pin, &ctx->power_voltage_pin);
        BREAK_IF(err != E_OK);

        err = E_AGAIN;
        ctx->reset_errcode = err;
        ctx->reset_request = true;
      }
    } else if(ctx->reset_errcode != E_AGAIN) {
      err = ctx->reset_errcode;
      ctx->reset_request = false;
      if(err == E_OK) {
        ctx->diag.bits.position_reach_failure = false;
        ctx->process_time = 0;
        ctx->configured = true;
      }
    } else {
      err = E_AGAIN;
    }

  } while(0);

  return err;
}

error_t etc_reset(etc_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    if(etc_adaptation_cancel(ctx) == E_AGAIN) {
      ctx->configured = false;
      err = E_AGAIN;
      break;
    }

    if(ctx->configured || ctx->data.active) {
      err = etc_internal_drive(ctx, false, 0.0f);
    }
    ctx->configured = false;

  } while(0);

  return err;
}

static error_t etc_apply_adaptation(etc_ctx_t *ctx)
{
  const etc_adaptation_ctx_t *adaptation = &ctx->adaptation;
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(adaptation->status != E_OK, err = adaptation->status);
    BREAK_IF_ACTION(!etc_internal_adaptation_config_valid(&adaptation->result), err = E_BADVALUE);

    memcpy(&ctx->config.adaptation, &adaptation->result, sizeof(ctx->config.adaptation));
    ctx->process_time = 0;
    etc_internal_reset_control(ctx, time_now_us());

  } while(0);

  return err;
}

void etc_loop_slow(etc_ctx_t *ctx)
{
  error_t err = E_OK;

  if(ctx->ready) {
    if(ctx->adaptation.apply_status == E_AGAIN && ctx->configured && !ctx->reset_request &&
        ctx->adaptation.status != E_AGAIN && (ctx->adaptation.result.valid || ctx->adaptation.status != E_OK)) {
      ctx->adaptation.apply_status = etc_apply_adaptation(ctx);
    }

    if(ctx->adaptation.status == E_AGAIN) {
      etc_adaptation_loop(ctx);
    } else if(ctx->configured) {
      err = etc_internal_process(ctx);
      if(err != E_OK && err != E_AGAIN) {
        //TODO: set error in future
      }
    }
  }
}

ITCM_FUNC void etc_loop_fast(etc_ctx_t *ctx)
{
  error_t err = E_OK;

  if(ctx->ready) {
    err = etc_fsm(ctx);
    if(err != E_OK && err != E_AGAIN) {
      //TODO: set error in future
    }

  }
}

error_t etc_get_data(etc_ctx_t *ctx, etc_data_t *data)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(data == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    *data = ctx->data;

  } while(0);

  return err;
}

error_t etc_get_diag(etc_ctx_t *ctx, etc_diag_t *diag)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(diag == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    *diag = ctx->diag;

  } while(0);

  return err;
}
