/*
 * core_powermoding.c
 *
 *  Created on: Aug 16, 2025
 *      Author: VHEMaster
 */

#include "core_powermoding.h"
#include "core_powermoding_fsm.h"
#include "common.h"

static ecu_core_powermoding_ctx_t powermoding_ctx = {0};

static void powermoding_get_mode_requested(ecu_core_ctx_t *ctx);

error_t core_powermoding_init(ecu_core_ctx_t *ctx)
{
  error_t ret = E_OK;
  time_us_t now;
  ecu_core_powermoding_ctx_t *pm_ctx;

  do {
    BREAK_IF_ACTION(ctx == NULL, ret = E_PARAM);

    memset(&powermoding_ctx, 0, sizeof(powermoding_ctx));
    pm_ctx = &powermoding_ctx;
    ctx->powermoding = pm_ctx;

    now = time_now_us();
    pm_ctx->mode_current = POWERMODING_MODE_UNDEFINED;
    pm_ctx->mode_switch_timestamp = now;

  } while(0);

  return ret;
}

void core_powermoding_loop_slow(ecu_core_ctx_t *ctx)
{
  do {
    BREAK_IF(ctx == NULL);
    BREAK_IF(ctx->powermoding == NULL);

    powermoding_get_mode_requested(ctx);

    core_powermoding_fsm(ctx);

  } while(0);
}

error_t core_powermoding_user_mode_request(ecu_core_ctx_t *ctx, ecu_core_powermoding_user_t user, ecu_core_powermoding_mode_request_t mode_request)
{
  error_t ret = E_OK;
  ecu_core_powermoding_ctx_t *pm_ctx;

  do {
    BREAK_IF_ACTION(ctx == NULL, ret = E_PARAM);
    BREAK_IF_ACTION(user <= POWERMODING_USER_RSVD, ret = E_PARAM);
    BREAK_IF_ACTION(user >= POWERMODING_USER_MAX, ret = E_PARAM);
    BREAK_IF_ACTION(mode_request >= POWERMODING_MODE_REQ_MAX, ret = E_PARAM);

    pm_ctx = ctx->powermoding;

    pm_ctx->modes_requested[user] = mode_request;

  } while(0);

  return ret;
}

error_t core_powermoding_user_mode_reset(ecu_core_ctx_t *ctx, ecu_core_powermoding_user_t user)
{
  error_t ret = E_OK;
  ecu_core_powermoding_ctx_t *pm_ctx;

  do {
    BREAK_IF_ACTION(ctx == NULL, ret = E_PARAM);
    BREAK_IF_ACTION(user <= POWERMODING_USER_RSVD, ret = E_PARAM);
    BREAK_IF_ACTION(user >= POWERMODING_USER_MAX, ret = E_PARAM);

    pm_ctx = ctx->powermoding;

    pm_ctx->modes_requested[user] = POWERMODING_MODE_REQ_STANDBY;

  } while(0);

  return ret;
}

error_t core_powermoding_mode_get(ecu_core_ctx_t *ctx, ecu_core_powermoding_mode_t *mode)
{
  error_t ret = E_OK;
  ecu_core_powermoding_ctx_t *pm_ctx;

  do {
    BREAK_IF_ACTION(ctx == NULL, ret = E_PARAM);
    BREAK_IF_ACTION(mode == NULL, ret = E_PARAM);

    pm_ctx = ctx->powermoding;

    *mode = pm_ctx->mode_current;

  } while(0);

  return ret;
}

STATIC_INLINE void powermoding_get_mode_requested(ecu_core_ctx_t *ctx)
{
  ecu_core_powermoding_ctx_t *pm_ctx;
  ecu_core_powermoding_mode_request_t mode_requested;

  pm_ctx = ctx->powermoding;

  mode_requested = POWERMODING_MODE_REQ_STANDBY;
  for(ecu_core_powermoding_user_t u = 0; u < POWERMODING_USER_MAX; u++) {
    if(pm_ctx->modes_requested[u] < POWERMODING_MODE_REQ_MAX && pm_ctx->modes_requested[u] > mode_requested) {
      mode_requested = pm_ctx->modes_requested[u];
    }
  }
  pm_ctx->mode_requested = mode_requested;
}
