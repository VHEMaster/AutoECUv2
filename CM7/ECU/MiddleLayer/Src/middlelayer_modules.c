/*
 * middlelayer_modules.c
 *
 *  Created on: May 16, 2024
 *      Author: VHEMaster
 */

#include "middlelayer_modules.h"
#include "config_modules.h"

#include "config_timing.h"
#include "config_etc.h"
#include "config_vvt.h"
#include "config_fuelpump.h"
#include "config_coolingfan.h"
#include "config_ignpower.h"

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

    for(int i = 0; i < ECU_MODULE_VVT_MAX; i++) {
      err = ecu_modules_get_module_ctx(ECU_MODULE_TYPE_VVT, i, &module_ctx);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

      err = ecu_modules_vvt_init(i, (vvt_ctx_t *)module_ctx);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

      err = ecu_modules_set_module_initialized(ECU_MODULE_TYPE_VVT, i, true);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));
    }
    BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

    for(int i = 0; i < ECU_MODULE_FUELPUMP_MAX; i++) {
      err = ecu_modules_get_module_ctx(ECU_MODULE_TYPE_FUELPUMP, i, &module_ctx);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

      err = ecu_modules_fuelpump_init(i, (fuelpump_ctx_t *)module_ctx);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

      err = ecu_modules_set_module_initialized(ECU_MODULE_TYPE_FUELPUMP, i, true);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));
    }
    BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

    for(int i = 0; i < ECU_MODULE_COOLINGFAN_MAX; i++) {
      err = ecu_modules_get_module_ctx(ECU_MODULE_TYPE_COOLINGFAN, i, &module_ctx);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

      err = ecu_modules_coolingfan_init(i, (coolingfan_ctx_t *)module_ctx);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

      err = ecu_modules_set_module_initialized(ECU_MODULE_TYPE_COOLINGFAN, i, true);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));
    }
    BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

    for(int i = 0; i < ECU_MODULE_IGNPOWER_MAX; i++) {
      err = ecu_modules_get_module_ctx(ECU_MODULE_TYPE_IGNPOWER, i, &module_ctx);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

      err = ecu_modules_ignpower_init(i, (ignpower_ctx_t *)module_ctx);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

      err = ecu_modules_set_module_initialized(ECU_MODULE_TYPE_IGNPOWER, i, true);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));
    }
    BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

  } while(0);
}
