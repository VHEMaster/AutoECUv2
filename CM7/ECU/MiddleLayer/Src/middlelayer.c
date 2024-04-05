
/*
 * middlelayer.c
 *
 *  Created on: Apr 3, 2024
 *      Author: VHEMaster
 */

#include "middlelayer.h"
#include "middlelayer_spi.h"
#include "middlelayer_gpio.h"
#include "middlelayer_devices.h"
#include "config.h"
#include "core.h"

#include "spi.h"
#include "core.h"

static void middlelayer_tim_slow_irq(TIM_HandleTypeDef *htim)
{
  middlelayer_gpio_loop_slow();
  middlelayer_spi_loop_slow();
  middlelayer_devices_loop_slow();

  core_loop_slow();
}

static void middlelayer_tim_fast_irq(TIM_HandleTypeDef *htim)
{
  middlelayer_gpio_loop_fast();
  middlelayer_spi_loop_fast();
  middlelayer_devices_loop_fast();

  core_loop_fast();
}

void middlelayer_init(void)
{
  ecu_config_ll_init();

  ecu_config_start_counter();

  middlelayer_spi_init();

  middlelayer_gpio_init();

  middlelayer_devices_init();

  ecu_config_start_periodic_timers(middlelayer_tim_slow_irq, middlelayer_tim_fast_irq);

  core_init();
}

void middlelayer_loop(void)
{
  ecu_config_iwgd_refresh();
  middlelayer_gpio_loop_main();

  middlelayer_spi_loop_main();
  middlelayer_devices_loop_main();

  core_loop_main();

}
