
/*
 * middlelayer.c
 *
 *  Created on: Apr 3, 2024
 *      Author: VHEMaster
 */

#include "common.h"
#include "middlelayer.h"
#include "middlelayer_ipc.h"
#include "config_hw.h"

#ifdef CORE_CM7
#include "middlelayer_spi.h"
#include "middlelayer_gpio.h"
#include "middlelayer_devices.h"
#include "middlelayer_sensors.h"
#include "middlelayer_modules.h"
#include "middlelayer_timings.h"
#include "config_loop.h"
#include "config_global.h"
#include "core.h"
#include "spi.h"
#endif /* CORE_CM7 */

#ifdef CORE_CM4
#include "middlelayer_comm.h"
#endif /* CORE_CM4 */

#include "time.h"

typedef struct {
    TIME_MSMT_TYPE main;
    TIME_MSMT_TYPE comm;
    TIME_MSMT_TYPE slow;

#ifdef CORE_CM7
    TIME_MSMT_TYPE fast;
#endif /* CORE_CM7 */

}ml_loop_time_msmt_t;

static ml_loop_time_msmt_t ml_loop_time_msmt = {0};

static void middlelayer_tim_slow_irq(void)
{
  TIME_MSMT_START(&ml_loop_time_msmt.slow);

#ifdef CORE_CM7
  middlelayer_gpio_loop_slow();
  middlelayer_spi_loop_slow();
  middlelayer_devices_loop_slow();
  middlelayer_sensors_loop_slow();
  middlelayer_modules_loop_slow();
  middlelayer_timings_loop_slow();

  ecu_loop_slow();

  ecu_config_global_loop_slow();

  core_loop_slow();
#endif /* CORE_CM7 */

#ifdef CORE_CM4
  middlelayer_comm_loop_slow();
#endif /* CORE_CM4 */

  TIME_MSMT_STOP(&ml_loop_time_msmt.slow);
}

#ifdef CORE_CM7
ITCM_FUNC static void middlelayer_tim_fast_irq(void *)
{
  TIME_MSMT_START(&ml_loop_time_msmt.fast);

  middlelayer_gpio_loop_fast();
  middlelayer_spi_loop_fast();
  middlelayer_devices_loop_fast();
  middlelayer_sensors_loop_fast();
  middlelayer_modules_loop_fast();
  middlelayer_timings_loop_fast();

  ecu_loop_fast();

  ecu_config_global_loop_fast();

  core_loop_fast();

  TIME_MSMT_STOP(&ml_loop_time_msmt.fast);
}
#endif /* CORE_CM7 */

void middlelayer_loop(void)
{
  TIME_MSMT_START(&ml_loop_time_msmt.main);

  ecu_config_iwgd_refresh();

#ifdef CORE_CM7
  middlelayer_gpio_loop_main();

  middlelayer_spi_loop_main();
  middlelayer_devices_loop_main();
  middlelayer_sensors_loop_main();
  middlelayer_modules_loop_main();
  middlelayer_timings_loop_main();

  ecu_loop_main();

  ecu_config_global_loop_main();

  core_loop_main();
#endif /* CORE_CM7 */

#ifdef CORE_CM4
  middlelayer_comm_loop_main();
#endif /* CORE_CM4 */

  TIME_MSMT_STOP(&ml_loop_time_msmt.main);
}

static void middlelayer_tim_comm_irq(void)
{
  TIME_MSMT_START(&ml_loop_time_msmt.comm);

  middlelayer_ipc_loop_comm();

#ifdef CORE_CM4
  middlelayer_comm_loop_comm();
#endif /* CORE_CM4 */

#ifdef CORE_CM7
  core_loop_comm();
#endif /* CORE_CM7 */

  TIME_MSMT_STOP(&ml_loop_time_msmt.comm);
}

void middlelayer_init(void)
{
  ecu_config_ll_init();

  ecu_config_init_swi();

  ecu_config_init_counter();

  middlelayer_ipc_init();

#ifdef CORE_CM7

  middlelayer_spi_init();

  middlelayer_gpio_init();

  middlelayer_devices_init();

  middlelayer_sensors_init();

  middlelayer_modules_init();

  middlelayer_timings_init();

  ecu_config_global_init();

  core_init();

  ecu_config_start_counter();

#endif /* CORE_CM7 */

#ifdef CORE_CM4
  middlelayer_comm_init();
#endif /* CORE_CM4 */
}

void middlelayer_start(void)
{
  middlelayer_ipc_start();

#ifdef CORE_CM7
  ecu_config_start_periodic_timers(middlelayer_tim_slow_irq, middlelayer_tim_comm_irq, (pTIM_CallbackTypeDef)middlelayer_tim_fast_irq);
#endif /* CORE_CM7 */

#ifdef CORE_CM4
  ecu_config_start_periodic_timers(middlelayer_tim_slow_irq, middlelayer_tim_comm_irq, NULL);
#endif /* CORE_CM4 */
}

INLINE void middlelayer_fault(middlelayer_fault_type_t fault_type)
{
#ifdef CORE_CM7
  IGN_NEN_GPIO_Port->BSRR = IGN_NEN_Pin;
  LAMBDA_HEATER_NEN_GPIO_Port->BSRR = LAMBDA_HEATER_NEN_Pin;
  OUTS_RESET_GPIO_Port->BSRR = OUTS_RESET_Pin << 16;
  LAMBDA_NRST_GPIO_Port->BSRR = LAMBDA_NRST_Pin << 16;
#endif /* CORE_CM7 */

#ifdef CORE_CM4

#endif /* CORE_CM4 */

  (void)fault_type;
}
