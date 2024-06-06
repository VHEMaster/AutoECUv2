/*
 * middlelayer_modules.c
 *
 *  Created on: May 16, 2024
 *      Author: VHEMaster
 */

#include "middlelayer_modules.h"
#include "config_modules.h"
#include "config_cylinders.h"
#include "config_timing.h"
#include "config_etc.h"
#include "compiler.h"

void middlelayer_modules_loop_main(void)
{
  ecu_modules_loop_main();
}

void middlelayer_modules_loop_slow(void)
{
  ecu_modules_loop_slow();
}

ITCM_FUNC void middlelayer_modules_loop_fast(void)
{
  ecu_modules_loop_fast();
}

void middlelayer_modules_init(void)
{
  error_t err = E_OK;
  void *module_ctx;

  //TODO: Create DTC of modules init
  (void)err;

  do {
    err = ecu_modules_init();
    BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

    for(int i = 0; i < ECU_MODULE_CYLINDERS_MAX; i++) {
      err = ecu_modules_get_module_ctx(ECU_MODULE_TYPE_CYLINDERS, i, &module_ctx);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

      err = ecu_modules_cylinders_init(i, (cylinders_ctx_t *)module_ctx);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

      err = ecu_modules_set_module_initialized(ECU_MODULE_TYPE_CYLINDERS, i, true);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));
    }
    BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

    for(int i = 0; i < ECU_MODULE_TIMING_MAX; i++) {
      err = ecu_modules_get_module_ctx(ECU_MODULE_TYPE_TIMING, i, &module_ctx);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

      err = ecu_modules_timing_init(i, (timing_ctx_t *)module_ctx);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

      err = ecu_modules_set_module_initialized(ECU_MODULE_TYPE_TIMING, i, true);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));
    }
    BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

    for(int i = 0; i < ECU_MODULE_ETC_MAX; i++) {
      err = ecu_modules_get_module_ctx(ECU_MODULE_TYPE_ETC, i, &module_ctx);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

      err = ecu_modules_etc_init(i, (etc_ctx_t *)module_ctx);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

      err = ecu_modules_set_module_initialized(ECU_MODULE_TYPE_ETC, i, true);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));
    }
    BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

  } while(0);
}
