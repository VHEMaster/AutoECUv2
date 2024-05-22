
/*
 * middlelayer.c
 *
 *  Created on: Apr 3, 2024
 *      Author: VHEMaster
 */

#include <config_hw.h>
#include "middlelayer.h"
#include "middlelayer_spi.h"
#include "middlelayer_gpio.h"
#include "middlelayer_devices.h"
#include "middlelayer_sensors.h"
#include "middlelayer_modules.h"
#include "config_loop.h"
#include "config_global.h"
#include "core.h"
#include "spi.h"
#include "core.h"
#include "time.h"

typedef struct {
    time_msmnt_item_t main;
    time_msmnt_item_t slow;
    time_msmnt_item_t fast;
}ml_loop_time_msmt_t;

static ml_loop_time_msmt_t ml_loop_time_msmt = {0};

static void middlelayer_tim_slow_irq(void *)
{
  time_msmt_start(&ml_loop_time_msmt.slow);

  middlelayer_gpio_loop_slow();
  middlelayer_spi_loop_slow();
  middlelayer_devices_loop_slow();
  middlelayer_sensors_loop_slow();
  middlelayer_modules_loop_slow();

  ecu_loop_slow();

  ecu_config_global_loop_slow();

  core_loop_slow();

  time_msmt_stop(&ml_loop_time_msmt.slow);
}

ITCM_FUNC static void middlelayer_tim_fast_irq(void *)
{
  time_msmt_start(&ml_loop_time_msmt.fast);

  middlelayer_gpio_loop_fast();
  middlelayer_spi_loop_fast();
  middlelayer_devices_loop_fast();
  middlelayer_sensors_loop_fast();
  middlelayer_modules_loop_fast();

  ecu_loop_fast();

  ecu_config_global_loop_fast();

  core_loop_fast();

  time_msmt_stop(&ml_loop_time_msmt.fast);
}

void middlelayer_loop(void)
{
  time_msmt_start(&ml_loop_time_msmt.main);

  ecu_config_iwgd_refresh();
  middlelayer_gpio_loop_main();

  middlelayer_spi_loop_main();
  middlelayer_devices_loop_main();
  middlelayer_sensors_loop_main();
  middlelayer_modules_loop_main();

  ecu_loop_main();

  ecu_config_global_loop_main();

  core_loop_main();

  time_msmt_stop(&ml_loop_time_msmt.main);
}

void middlelayer_init(void)
{
  ecu_config_ll_init();

  middlelayer_spi_init();

  middlelayer_gpio_init();

  middlelayer_devices_init();

  middlelayer_sensors_init();

  middlelayer_modules_init();

  ecu_config_global_init();

  core_init();

  ecu_config_start_counter();

  ecu_config_start_periodic_timers((pTIM_CallbackTypeDef)middlelayer_tim_slow_irq, (pTIM_CallbackTypeDef)middlelayer_tim_fast_irq);

}
