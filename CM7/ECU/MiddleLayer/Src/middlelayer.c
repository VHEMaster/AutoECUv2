
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
#include "config_loop.h"
#include "config_global.h"
#include "core.h"
#include "spi.h"
#include "core.h"
#include "time.h"

typedef struct {
    time_us_t last;
    time_delta_us_t period;

    time_delta_us_t load_last;
    time_delta_us_t load_min;
    time_delta_us_t load_max;
}ml_loop_time_msmnt_item_t;

typedef struct {
    ml_loop_time_msmnt_item_t main;
    ml_loop_time_msmnt_item_t slow;
    ml_loop_time_msmnt_item_t fast;
}ml_loop_time_msmt_t;

static ml_loop_time_msmt_t ml_loop_time_msmt = {0};

ITCM_FUNC static void middlelayer_time_msmt_start(ml_loop_time_msmnt_item_t *item)
{
  time_us_t now = time_get_current_us();

  item->period = time_diff(now, item->last);
  item->last = now;
}

ITCM_FUNC static void middlelayer_time_msmt_stop(ml_loop_time_msmnt_item_t *item)
{
  time_us_t now = time_get_current_us();
  time_delta_us_t load = time_diff(now, item->last);

  if(item->load_max < load || item->load_max == 0) {
    item->load_max = load;
  }
  if(item->load_min > load || item->load_min == 0) {
    item->load_min = load;
  }
  item->load_last = load;
}

static void middlelayer_tim_slow_irq(TIM_HandleTypeDef *htim)
{
  middlelayer_time_msmt_start(&ml_loop_time_msmt.slow);

  middlelayer_gpio_loop_slow();
  middlelayer_spi_loop_slow();
  middlelayer_devices_loop_slow();

  ecu_loop_slow();

  ecu_config_global_loop_slow();

  core_loop_slow();

  middlelayer_time_msmt_stop(&ml_loop_time_msmt.slow);
}

ITCM_FUNC static void middlelayer_tim_fast_irq(TIM_HandleTypeDef *htim)
{
  middlelayer_time_msmt_start(&ml_loop_time_msmt.fast);

  middlelayer_gpio_loop_fast();
  middlelayer_spi_loop_fast();
  middlelayer_devices_loop_fast();

  ecu_loop_fast();

  ecu_config_global_loop_fast();

  core_loop_fast();

  middlelayer_time_msmt_stop(&ml_loop_time_msmt.fast);
}

void middlelayer_loop(void)
{
  middlelayer_time_msmt_start(&ml_loop_time_msmt.main);

  ecu_config_iwgd_refresh();
  middlelayer_gpio_loop_main();

  middlelayer_spi_loop_main();
  middlelayer_devices_loop_main();

  ecu_loop_main();

  ecu_config_global_loop_main();

  core_loop_main();

  middlelayer_time_msmt_stop(&ml_loop_time_msmt.main);
}

void middlelayer_init(void)
{
  ecu_config_ll_init();

  middlelayer_spi_init();

  middlelayer_gpio_init();

  middlelayer_devices_init();

  ecu_config_global_init();

  core_init();

  ecu_config_start_counter();

  ecu_config_start_periodic_timers(middlelayer_tim_slow_irq, middlelayer_tim_fast_irq);

}
