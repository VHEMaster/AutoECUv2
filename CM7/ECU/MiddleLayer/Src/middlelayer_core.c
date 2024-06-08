/*
 * middlelayer_core.c
 *
 *  Created on: June 8, 2024
 *      Author: VHEMaster
 */

#include "middlelayer_core.h"
#include "config_core.h"

#include "config_cylinders.h"

#include "compiler.h"

void middlelayer_core_components_loop_main(void)
{
  ecu_core_components_loop_main();
}

void middlelayer_core_components_loop_slow(void)
{
  ecu_core_components_loop_slow();
}

ITCM_FUNC void middlelayer_core_components_loop_fast(void)
{
  ecu_core_components_loop_fast();
}

void middlelayer_core_components_init(void)
{
  error_t err = E_OK;
  void *module_ctx;

  //TODO: Create DTC of core_components init
  (void)err;

  do {
    err = ecu_core_components_init();

    for(int i = 0; i < ECU_CORE_COMPONENT_CYLINDERS_MAX; i++) {
      err = ecu_core_components_get_ctx(ECU_CORE_COMPONENT_TYPE_CYLINDERS, i, &module_ctx);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

      err = ecu_core_component_cylinders_init(i, (cylinders_ctx_t *)module_ctx);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

      err = ecu_core_components_set_initialized(ECU_CORE_COMPONENT_TYPE_CYLINDERS, i, true);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));
    }
    BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

  } while(0);
}
