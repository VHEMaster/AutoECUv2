
/*
 * middlelayer.c
 *
 *  Created on: Apr 3, 2024
 *      Author: VHEMaster
 */

#include "middlelayer.h"
#include "middlelayer_spi.h"
#include "middlelayer_gpio.h"
#include "config.h"
#include "core.h"

#include "inputs.h"
#include "outputs.h"
#include "queuedpulses.h"
#include "spi.h"
#include "core.h"

static void middlelayer_tim_slow_irq(TIM_HandleTypeDef *htim)
{
  inputs_loop_slow();
  outputs_loop_slow();
  queuedpulses_loop_slow();

  middlelayer_spi_loop_slow();

  core_loop_slow();
}

static void middlelayer_tim_fast_irq(TIM_HandleTypeDef *htim)
{
  inputs_loop_fast();
  outputs_loop_fast();
  queuedpulses_loop_fast();

  middlelayer_spi_loop_fast();

  core_loop_fast();
}

void middlelayer_init(void)
{
  ecu_config_ll_init();

  ecu_config_start_counter();

  middlelayer_spi_init();

  middlelayer_gpio_init();

  ecu_config_start_periodic_timers(middlelayer_tim_slow_irq, middlelayer_tim_fast_irq);
}

void middlelayer_loop(void)
{
  ecu_config_iwgd_refresh();

  inputs_loop_main();
  outputs_loop_main();
  queuedpulses_loop_main();

  middlelayer_spi_loop_main();

  core_loop_main();

}
