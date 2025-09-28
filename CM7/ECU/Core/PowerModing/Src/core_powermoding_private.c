/*
 * core_powermoding_fsm.c
 *
 *  Created on: Aug 17, 2025
 *      Author: VHEMaster
 */

#include "core_powermoding_types.h"
#include "core_powermoding_private.h"
#include "common.h"

void powermoding_switch_standby(ecu_core_ctx_t *ctx)
{
  //TODO: IMPLEMENT
}

void powermoding_switch_sleep(ecu_core_ctx_t *ctx)
{
  //TODO: IMPLEMENT
}

void powermoding_get_mode_requested(ecu_core_ctx_t *ctx)
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
