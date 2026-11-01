/*
 * core_powermoding_fsm.c
 *
 *  Created on: Aug 17, 2025
 *      Author: VHEMaster
 */

#include "core_powermoding_types.h"
#include "core_powermoding_private.h"
#include "common.h"
#include "types.h"

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

INLINE bool powermoding_get_state_flag(ecu_core_ctx_t *ctx, ecu_core_powermoding_state_flag_t mask)
{
  ecu_core_powermoding_ctx_t *pm_ctx;
  bool retval = false;

  pm_ctx = ctx->powermoding;

  if((pm_ctx->state_flags & mask) == mask) {
    retval = true;
  }

  return retval;
}

INLINE void powermoding_set_state_flag(ecu_core_ctx_t *ctx, ecu_core_powermoding_state_flag_t mask)
{
  ecu_core_powermoding_ctx_t *pm_ctx;

  pm_ctx = ctx->powermoding;

  pm_ctx->state_flags |= mask;
}

INLINE void powermoding_clear_state_flag(ecu_core_ctx_t *ctx, ecu_core_powermoding_state_flag_t mask)
{
  ecu_core_powermoding_ctx_t *pm_ctx;

  pm_ctx = ctx->powermoding;

  pm_ctx->state_flags &= ~mask;
}

INLINE bool powermoding_is_runup_allowed(ecu_core_ctx_t *ctx)
{
  ecu_core_powermoding_ctx_t *pm_ctx;
  bool retval = false;

  pm_ctx = ctx->powermoding;

  switch(pm_ctx->mode_current) {
    case POWERMODING_MODE_IGNITION:
    case POWERMODING_MODE_STARTING:
    case POWERMODING_MODE_RUNNING:
      retval = true;
      break;
    default:
      break;
  }

  return retval;
}

