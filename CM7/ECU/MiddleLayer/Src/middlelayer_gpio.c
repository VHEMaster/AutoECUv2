/*
 * middlelayer_gpio.c
 *
 *  Created on: Apr 4, 2024
 *      Author: VHEMaster
 */

#include "middlelayer_gpio.h"
#include "config_queuedpulses.h"
#include "compiler.h"
#include "config_gpio.h"

#include "config_extern.h"

void middlelayer_gpio_loop_main(void)
{
  inputs_loop_main();
  outputs_loop_main();
  queuedpulses_loop_main();
}

void middlelayer_gpio_loop_slow(void)
{
  inputs_loop_slow();
  outputs_loop_slow();
  queuedpulses_loop_slow();
}

void middlelayer_gpio_loop_fast(void)
{
  inputs_loop_fast();
  outputs_loop_fast();
  queuedpulses_loop_fast();
}

void middlelayer_gpio_init(void)
{
  error_t err = E_OK;

  //TODO: Create DTC of GPIO init
  (void)err;

  err = ecu_config_gpio_output_init();

  err = ecu_config_gpio_input_init();

  err = ecu_config_queuedpulses_init();
}
