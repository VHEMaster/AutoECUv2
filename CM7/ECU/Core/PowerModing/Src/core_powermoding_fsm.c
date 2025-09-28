/*
 * core_powermoding_fsm.c
 *
 *  Created on: Aug 17, 2025
 *      Author: VHEMaster
 */

#include "core_powermoding_types.h"
#include "core_powermoding_fsm.h"
#include "core_powermoding_private.h"
#include "common.h"

static void powermoding_fsm_mode_standby(ecu_core_ctx_t *ctx);
static void powermoding_fsm_mode_sleep(ecu_core_ctx_t *ctx);
static void powermoding_fsm_mode_coldboot(ecu_core_ctx_t *ctx);
static void powermoding_fsm_mode_rundown(ecu_core_ctx_t *ctx);
static void powermoding_fsm_mode_turbotimer(ecu_core_ctx_t *ctx);
static void powermoding_fsm_mode_ignition(ecu_core_ctx_t *ctx);
static void powermoding_fsm_mode_starting(ecu_core_ctx_t *ctx);
static void powermoding_fsm_mode_running(ecu_core_ctx_t *ctx);

void core_powermoding_fsm(ecu_core_ctx_t *ctx)
{
  ecu_core_powermoding_ctx_t *pm_ctx;
  ecu_core_powermoding_mode_t mode_prev;

  pm_ctx = ctx->powermoding;
  mode_prev = pm_ctx->mode_current;

  switch(mode_prev) {
    case POWERMODING_MODE_STANDBY:
      powermoding_fsm_mode_standby(ctx);
      break;
    case POWERMODING_MODE_SLEEP:
      powermoding_fsm_mode_sleep(ctx);
      break;
    case POWERMODING_MODE_COLDBOOT:
      powermoding_fsm_mode_coldboot(ctx);
      break;
    case POWERMODING_MODE_RUNDOWN:
      powermoding_fsm_mode_rundown(ctx);
      break;
    case POWERMODING_MODE_TURBOTIMER:
      powermoding_fsm_mode_turbotimer(ctx);
      break;
    case POWERMODING_MODE_IGNITION:
      powermoding_fsm_mode_ignition(ctx);
      break;
    case POWERMODING_MODE_STARTING:
      powermoding_fsm_mode_starting(ctx);
      break;
    case POWERMODING_MODE_RUNNING:
      powermoding_fsm_mode_running(ctx);
      break;
    default:
      pm_ctx->mode_current = POWERMODING_MODE_STANDBY;
      pm_ctx->mode_switch_timestamp = time_now_us();
      break;
  }

  if(pm_ctx->mode_current != mode_prev) {
    pm_ctx->mode_switch_timestamp = time_now_us();
  }
}

static void powermoding_fsm_mode_standby(ecu_core_ctx_t *ctx)
{
  time_us_t now, mode_switch_timestamp;
  ecu_core_powermoding_ctx_t *pm_ctx;
  const ecu_config_powermoding_t *config;

  pm_ctx = ctx->powermoding;
  config = &ctx->calibration->powermoding;

  mode_switch_timestamp = pm_ctx->mode_switch_timestamp;
  now = time_now_us();

  switch(pm_ctx->mode_requested) {
    case POWERMODING_MODE_REQ_STANDBY:
      break;
    case POWERMODING_MODE_REQ_SLEEP:
      pm_ctx->mode_current = POWERMODING_MODE_SLEEP;
      break;
    default:
      pm_ctx->mode_current = POWERMODING_MODE_COLDBOOT;
      break;
  }

  if(pm_ctx->mode_current == POWERMODING_MODE_STANDBY) {
    if(time_diff(now, mode_switch_timestamp) > config->standby_state_phy_accept_time) {
      powermoding_switch_standby(ctx);
    }
  }
}

static void powermoding_fsm_mode_sleep(ecu_core_ctx_t *ctx)
{
  time_us_t now, mode_switch_timestamp;
  ecu_core_powermoding_ctx_t *pm_ctx;
  const ecu_config_powermoding_t *config;

  pm_ctx = ctx->powermoding;
  config = &ctx->calibration->powermoding;

  mode_switch_timestamp = pm_ctx->mode_switch_timestamp;
  now = time_now_us();

  switch(pm_ctx->mode_requested) {
    case POWERMODING_MODE_REQ_STANDBY:
      pm_ctx->mode_current = POWERMODING_MODE_STANDBY;
      break;
    case POWERMODING_MODE_REQ_SLEEP:
      break;
    default:
      pm_ctx->mode_current = POWERMODING_MODE_COLDBOOT;
      break;
  }

  if(pm_ctx->mode_current == POWERMODING_MODE_SLEEP) {
    if(time_diff(now, mode_switch_timestamp) > config->sleep_state_phy_accept_time) {
      powermoding_switch_sleep(ctx);
    }
  }

}

static void powermoding_fsm_mode_coldboot(ecu_core_ctx_t *ctx)
{
  ecu_core_powermoding_ctx_t *pm_ctx;

  pm_ctx = ctx->powermoding;

  switch(pm_ctx->mode_requested) {
    case POWERMODING_MODE_REQ_STANDBY:
      pm_ctx->mode_current = POWERMODING_MODE_STANDBY;
      break;
    case POWERMODING_MODE_REQ_SLEEP:
      pm_ctx->mode_current = POWERMODING_MODE_SLEEP;
      break;
    case POWERMODING_MODE_REQ_RUNDOWN:
      pm_ctx->mode_current = POWERMODING_MODE_RUNDOWN;
      break;
    case POWERMODING_MODE_REQ_RUNNING:
      pm_ctx->mode_current = POWERMODING_MODE_IGNITION;
      break;
    default:
      pm_ctx->mode_current = POWERMODING_MODE_COLDBOOT;
      break;
  }


}

static void powermoding_fsm_mode_rundown(ecu_core_ctx_t *ctx)
{
  ecu_core_powermoding_ctx_t *pm_ctx;

  pm_ctx = ctx->powermoding;

  switch(pm_ctx->mode_requested) {
    case POWERMODING_MODE_REQ_STANDBY:
      pm_ctx->mode_current = POWERMODING_MODE_STANDBY;
      break;
    case POWERMODING_MODE_REQ_SLEEP:
      pm_ctx->mode_current = POWERMODING_MODE_SLEEP;
      break;
    case POWERMODING_MODE_REQ_COLDBOOT:
      pm_ctx->mode_current = POWERMODING_MODE_COLDBOOT;
      break;
    case POWERMODING_MODE_REQ_RUNNING:
      pm_ctx->mode_current = POWERMODING_MODE_IGNITION;
      break;
    default:
      pm_ctx->mode_current = POWERMODING_MODE_RUNDOWN;
      break;
  }
}

static void powermoding_fsm_mode_turbotimer(ecu_core_ctx_t *ctx)
{
  time_us_t now, mode_switch_timestamp;
  ecu_core_powermoding_ctx_t *pm_ctx;

  pm_ctx = ctx->powermoding;

  mode_switch_timestamp = pm_ctx->mode_switch_timestamp;
  now = time_now_us();

  pm_ctx->turbotimer_passed_time = time_diff(now, mode_switch_timestamp);

  switch(pm_ctx->mode_requested) {
    case POWERMODING_MODE_REQ_STANDBY:
    case POWERMODING_MODE_REQ_SLEEP:
    case POWERMODING_MODE_REQ_COLDBOOT:
      pm_ctx->mode_current = POWERMODING_MODE_REQ_RUNDOWN;
      break;
    case POWERMODING_MODE_REQ_RUNDOWN:
      pm_ctx->mode_current = POWERMODING_MODE_TURBOTIMER;
      break;
    case POWERMODING_MODE_REQ_RUNNING:
      pm_ctx->mode_current = POWERMODING_MODE_IGNITION;
      break;
    default:
      break;
  }

  if(pm_ctx->mode_current == POWERMODING_MODE_TURBOTIMER) {
    if(pm_ctx->turbotimer_passed_time > pm_ctx->turbotimer_requested_time) {
      pm_ctx->mode_current = POWERMODING_MODE_REQ_RUNDOWN;
    }
    //TODO: RUNNING HANDLING
  }
}

static void powermoding_fsm_mode_ignition(ecu_core_ctx_t *ctx)
{
  ecu_core_powermoding_ctx_t *pm_ctx;

  pm_ctx = ctx->powermoding;

  switch(pm_ctx->mode_requested) {
    case POWERMODING_MODE_REQ_STANDBY:
    case POWERMODING_MODE_REQ_SLEEP:
    case POWERMODING_MODE_REQ_COLDBOOT:
    case POWERMODING_MODE_REQ_RUNDOWN:
      pm_ctx->mode_current = POWERMODING_MODE_REQ_RUNDOWN;
      break;
    default:
      pm_ctx->mode_current = POWERMODING_MODE_IGNITION;
      break;
  }

  if(pm_ctx->mode_current == POWERMODING_MODE_IGNITION) {
    //TODO: IGNITION HANDLING
    //pm_ctx->mode_current = POWERMODING_MODE_STARTING;
    //pm_ctx->mode_current = POWERMODING_MODE_RUNNING;
  }
}

static void powermoding_fsm_mode_starting(ecu_core_ctx_t *ctx)
{
  ecu_core_powermoding_ctx_t *pm_ctx;

  pm_ctx = ctx->powermoding;

  switch(pm_ctx->mode_requested) {
    case POWERMODING_MODE_REQ_STANDBY:
    case POWERMODING_MODE_REQ_SLEEP:
    case POWERMODING_MODE_REQ_COLDBOOT:
    case POWERMODING_MODE_REQ_RUNDOWN:
      pm_ctx->mode_current = POWERMODING_MODE_IGNITION;
      break;
    default:
      pm_ctx->mode_current = POWERMODING_MODE_STARTING;
      break;
  }

  if(pm_ctx->mode_current == POWERMODING_MODE_STARTING) {
    //TODO: STARTING HANDLING
    //pm_ctx->mode_current = POWERMODING_MODE_IGNITION;
    //pm_ctx->mode_current = POWERMODING_MODE_RUNNING;
  }
}

static void powermoding_fsm_mode_running(ecu_core_ctx_t *ctx)
{
  ecu_core_powermoding_ctx_t *pm_ctx;

  pm_ctx = ctx->powermoding;

  switch(pm_ctx->mode_requested) {
    case POWERMODING_MODE_REQ_STANDBY:
    case POWERMODING_MODE_REQ_SLEEP:
    case POWERMODING_MODE_REQ_COLDBOOT:
    case POWERMODING_MODE_REQ_RUNDOWN:
      pm_ctx->mode_current = POWERMODING_MODE_TURBOTIMER;
      break;
    default:
      pm_ctx->mode_current = POWERMODING_MODE_RUNNING;
      break;
  }

  if(pm_ctx->mode_current == POWERMODING_MODE_RUNNING) {
    //TODO: RUNNING HANDLING
    //pm_ctx->mode_current = POWERMODING_MODE_IGNITION;
  }
}
