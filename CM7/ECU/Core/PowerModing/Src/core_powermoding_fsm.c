/*
 * core_powermoding_fsm.c
 *
 *  Created on: Aug 17, 2025
 *      Author: VHEMaster
 */

#include "core_powermoding_types.h"
#include "core_powermoding_fsm.h"
#include "common.h"

static void powermoding_fsm_mode_undefined(ecu_core_ctx_t *ctx);
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
  time_us_t now;
  ecu_core_powermoding_ctx_t *pm_ctx;

  pm_ctx = ctx->powermoding;

  switch(pm_ctx->mode_current) {
    case POWERMODING_MODE_UNDEFINED:
      powermoding_fsm_mode_undefined(ctx);
      break;
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
      now = time_now_us();
      pm_ctx->mode_current = POWERMODING_MODE_UNDEFINED;
      pm_ctx->mode_switch_timestamp = now;
      break;
  }
}

static void powermoding_fsm_mode_undefined(ecu_core_ctx_t *ctx)
{
  time_us_t now, mode_switch_timestamp;
  ecu_core_powermoding_ctx_t *pm_ctx;

  pm_ctx = ctx->powermoding;

  mode_switch_timestamp = pm_ctx->mode_switch_timestamp;
  now = time_now_us();


}

static void powermoding_fsm_mode_standby(ecu_core_ctx_t *ctx)
{
  time_us_t now, mode_switch_timestamp;
  ecu_core_powermoding_ctx_t *pm_ctx;

  pm_ctx = ctx->powermoding;

  mode_switch_timestamp = pm_ctx->mode_switch_timestamp;
  now = time_now_us();


}

static void powermoding_fsm_mode_sleep(ecu_core_ctx_t *ctx)
{
  time_us_t now, mode_switch_timestamp;
  ecu_core_powermoding_ctx_t *pm_ctx;

  pm_ctx = ctx->powermoding;

  mode_switch_timestamp = pm_ctx->mode_switch_timestamp;
  now = time_now_us();


}

static void powermoding_fsm_mode_coldboot(ecu_core_ctx_t *ctx)
{
  time_us_t now, mode_switch_timestamp;
  ecu_core_powermoding_ctx_t *pm_ctx;

  pm_ctx = ctx->powermoding;

  mode_switch_timestamp = pm_ctx->mode_switch_timestamp;
  now = time_now_us();


}

static void powermoding_fsm_mode_rundown(ecu_core_ctx_t *ctx)
{
  time_us_t now, mode_switch_timestamp;
  ecu_core_powermoding_ctx_t *pm_ctx;

  pm_ctx = ctx->powermoding;

  mode_switch_timestamp = pm_ctx->mode_switch_timestamp;
  now = time_now_us();


}

static void powermoding_fsm_mode_turbotimer(ecu_core_ctx_t *ctx)
{
  time_us_t now, mode_switch_timestamp;
  ecu_core_powermoding_ctx_t *pm_ctx;

  pm_ctx = ctx->powermoding;

  mode_switch_timestamp = pm_ctx->mode_switch_timestamp;
  now = time_now_us();


}

static void powermoding_fsm_mode_ignition(ecu_core_ctx_t *ctx)
{
  time_us_t now, mode_switch_timestamp;
  ecu_core_powermoding_ctx_t *pm_ctx;

  pm_ctx = ctx->powermoding;

  mode_switch_timestamp = pm_ctx->mode_switch_timestamp;
  now = time_now_us();


}

static void powermoding_fsm_mode_starting(ecu_core_ctx_t *ctx)
{
  time_us_t now, mode_switch_timestamp;
  ecu_core_powermoding_ctx_t *pm_ctx;

  pm_ctx = ctx->powermoding;

  mode_switch_timestamp = pm_ctx->mode_switch_timestamp;
  now = time_now_us();


}

static void powermoding_fsm_mode_running(ecu_core_ctx_t *ctx)
{
  time_us_t now, mode_switch_timestamp;
  ecu_core_powermoding_ctx_t *pm_ctx;

  pm_ctx = ctx->powermoding;

  mode_switch_timestamp = pm_ctx->mode_switch_timestamp;
  now = time_now_us();


}
