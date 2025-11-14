/*
 * middlelayer_timings.c
 *
 *  Created on: May 16, 2024
 *      Author: VHEMaster
 */

#include "middlelayer_timings.h"
#include "config_timings.h"

#include "config_timing_base.h"
#include "config_ignition.h"
#include "config_injection.h"
#include "config_rough.h"

#include "compiler.h"

void middlelayer_timings_loop_main(void)
{
  ecu_timings_loop_main();
}

void middlelayer_timings_loop_slow(void)
{
  ecu_timings_loop_slow();
}

ITCM_FUNC void middlelayer_timings_loop_fast(void)
{
  ecu_timings_loop_fast();
}

void middlelayer_timings_init(void)
{
  error_t err = E_OK;
  void *timing_ctx;

  //TODO: Create DTC of timings init
  (void)err;

  do {
    err = ecu_timings_init();

    for(int i = 0; i < ECU_TIMING_BASE_MAX; i++) {
      err = ecu_timings_get_timing_ctx(ECU_TIMING_TYPE_BASE, i, &timing_ctx);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

      err = ecu_timings_base_init(i, (timing_base_ctx_t *)timing_ctx);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

      err = ecu_timings_set_timing_initialized(ECU_TIMING_TYPE_BASE, i, true);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));
    }
    BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

    for(int i = 0; i < ECU_TIMING_IGNITION_MAX; i++) {
      err = ecu_timings_get_timing_ctx(ECU_TIMING_TYPE_IGNITION, i, &timing_ctx);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

      err = ecu_timings_ignition_init(i, (ignition_ctx_t *)timing_ctx);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

      err = ecu_timings_set_timing_initialized(ECU_TIMING_TYPE_IGNITION, i, true);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));
    }
    BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

    for(int i = 0; i < ECU_TIMING_INJECTION_MAX; i++) {
      err = ecu_timings_get_timing_ctx(ECU_TIMING_TYPE_INJECTION, i, &timing_ctx);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

      err = ecu_timings_injection_init(i, (injection_ctx_t *)timing_ctx);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

      err = ecu_timings_set_timing_initialized(ECU_TIMING_TYPE_INJECTION, i, true);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));
    }
    BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

    for(int i = 0; i < ECU_TIMING_ROUGH_MAX; i++) {
      err = ecu_timings_get_timing_ctx(ECU_TIMING_TYPE_ROUGH, i, &timing_ctx);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

      err = ecu_timings_rough_init(i, (rough_ctx_t *)timing_ctx);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

      err = ecu_timings_set_timing_initialized(ECU_TIMING_TYPE_ROUGH, i, true);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));
    }
    BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

  } while(0);
}
