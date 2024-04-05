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

void middlelayer_gpio_init(void)
{
  error_t err = E_OK;

  //TODO: Create DTC of GPIO init
  (void)err;

  err = ecu_config_gpio_output_init();

  err = ecu_config_gpio_input_init();

  err = ecu_config_queuedpulses_init();
}
