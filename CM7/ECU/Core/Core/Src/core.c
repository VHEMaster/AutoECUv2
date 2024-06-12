/*
 * core.c
 *
 *  Created on: Apr 3, 2024
 *      Author: VHEMaster
 */

#include "core.h"
#include "core_timing.h"
#include "common.h"

static ecu_core_ctx_t ecu_core_ctx = {0};

void core_init(void)
{
  error_t err;

  do {
    err = ecu_config_global_get_global_engine_config(&ecu_core_ctx.engine_config);
    BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

    err = ecu_config_global_get_engine_calibration_config(&ecu_core_ctx.calibration);
    BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

    err = ecu_config_global_get_engine_runtime_config(&ecu_core_ctx.runtime_config);
    BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

    err = ecu_modules_timing_register_cb(ECU_MODULE_TIMING_1, core_timing_signal_update_cb, &ecu_core_ctx);
    BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

  } while(0);

}

void core_loop_main(void)
{

}

void core_loop_slow(void)
{

}

ITCM_FUNC void core_loop_fast(void)
{

}
