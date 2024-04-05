/*
 * middlelayer_devices.c
 *
 *  Created on: Apr 4, 2024
 *      Author: VHEMaster
 */

#include "middlelayer_devices.h"
#include "config_devices.h"

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

  //TODO: Create DTC of GPIO init
  (void)err;

  err = ecu_devices_init();
}
