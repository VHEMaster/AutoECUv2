/*
 * middlelayer.c
 *
 *  Created on: Apr 3, 2024
 *      Author: VHEMaster
 */

#include "middlelayer.h"
#include "middlelayer_spi.h"
#include "config_extern.h"
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

static void middlelayer_start_timers(void)
{
  HAL_TIM_RegisterCallback(&htim6, HAL_TIM_PERIOD_ELAPSED_CB_ID, middlelayer_tim_slow_irq);
  HAL_TIM_RegisterCallback(&htim7, HAL_TIM_PERIOD_ELAPSED_CB_ID, middlelayer_tim_fast_irq);

  HAL_TIM_Base_Start_IT(&htim6);
  HAL_TIM_Base_Start_IT(&htim7);
}

static void middlelayer_start_time_counter(void)
{
  htim5.Init.Period = MIDDLELAYER_TIMEBASE_MASK;

  HAL_TIM_Base_Init(&htim5);

  time_init_timebase(&htim5.Instance->CNT, MIDDLELAYER_TIMEBASE_MASK);

  HAL_TIM_Base_Start(&htim5);
}

void middlelayer_ll_init(void)
{
  __HAL_DBGMCU_FREEZE_TIM5();
  __HAL_DBGMCU_FREEZE_IWDG1();
}

void middlelayer_init(void)
{
  middlelayer_start_time_counter();

  middlelayer_spi_init();

  middlelayer_start_timers();
}

void middlelayer_loop(void)
{
  HAL_IWDG_Refresh(&hiwdg1);

  inputs_loop_main();
  outputs_loop_main();
  queuedpulses_loop_main();

  middlelayer_spi_loop_main();

  core_loop_main();
}
