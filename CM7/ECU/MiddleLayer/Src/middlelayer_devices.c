/*
 * middlelayer_devices.c
 *
 *  Created on: Apr 4, 2024
 *      Author: VHEMaster
 */

#include "middlelayer_devices.h"
#include "config_devices.h"
#include "config_flexio.h"
#include "config_output.h"
#include "config_egt.h"
#include "compiler.h"

void middlelayer_devices_loop_main(void)
{
  ecu_devices_loop_main();
}

void middlelayer_devices_loop_slow(void)
{
  ecu_devices_loop_slow();
}

void middlelayer_devices_loop_fast(void)
{
  ecu_devices_loop_fast();
}

void middlelayer_devices_init(void)
{
  error_t err = E_OK;
  void *device_ctx;

  //TODO: Create DTC of GPIO init
  (void)err;

  do {
    err = ecu_devices_init();
    BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

    for(int i = 0; i < ECU_DEVICE_FLEXIO_MAX; i++) {
      err = ecu_devices_get_device_ctx(ECU_DEVICE_TYPE_FLEXIO, i, &device_ctx);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

      err = ecu_devices_flexio_init(i, (l9966_ctx_t *)device_ctx);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

      err = ecu_devices_set_device_initialized(ECU_DEVICE_TYPE_FLEXIO, i, true);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));
    }
    BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

    for(int i = 0; i < ECU_DEVICE_OUTPUT_MAX; i++) {
      err = ecu_devices_get_device_ctx(ECU_DEVICE_TYPE_OUTPUT, i, &device_ctx);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

      err = ecu_devices_output_init(i, (tle6240_ctx_t *)device_ctx);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

      err = ecu_devices_set_device_initialized(ECU_DEVICE_TYPE_OUTPUT, i, true);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));
    }
    BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

    for(int i = 0; i < ECU_DEVICE_EGT_MAX; i++) {
      err = ecu_devices_get_device_ctx(ECU_DEVICE_TYPE_EGT, i, &device_ctx);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

      err = ecu_devices_egt_init(i, (max31855_ctx_t *)device_ctx);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

      err = ecu_devices_set_device_initialized(ECU_DEVICE_TYPE_EGT, i, true);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));
    }
    BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));


  } while(0);
}
