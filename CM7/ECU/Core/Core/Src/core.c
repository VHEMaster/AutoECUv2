/*
 * core.c
 *
 *  Created on: Apr 3, 2024
 *      Author: VHEMaster
 */

#include "core.h"
#include "core_timing.h"
#include "core_powermoding.h"
#include "calcdata.h"
#include "core_init_fsm.h"
#include "common.h"

static ecu_core_ctx_t ecu_core_ctx = {0};

static void core_init_request(ecu_core_ctx_t *ctx)
{
  ctx->core_init_errcode = E_AGAIN;
  ctx->core_init_request = true;
}

void core_init(void)
{
  error_t err;
  ecu_core_ctx_t *ctx = &ecu_core_ctx;

  do {
    err = ecu_config_global_get_global_engine_config(&ctx->engine_config);
    BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

    err = ecu_config_global_get_engine_calibration_config(&ctx->calibration);
    BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

    err = ecu_config_global_get_engine_runtime_config(&ctx->runtime_config);
    BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

    err = ecu_modules_timing_register_cb(ECU_MODULE_TIMING_1, core_timing_signal_update_cb, ctx);
    BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

    core_init_request(ctx);

  } while(0);

}

void core_loop_main(void)
{

}

void core_loop_slow(void)
{
  ecu_core_ctx_t *ctx = &ecu_core_ctx;

  core_init_fsm(ctx);

  if(ctx->initialized) {
    core_calcdata_loop_slow(ctx);
    core_powermoding_loop_slow(ctx);
  }
}

ITCM_FUNC void core_loop_fast(void)
{

}
