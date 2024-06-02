/*
 * config.c
 *
 *  Created on: Apr 5, 2024
 *      Author: VHEMaster
 */

#include <config_hw.h>
#include "main.h"
#include "config_gpio.h"
#include "config_extern.h"
#include "compiler.h"
#include "time.h"

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  ecu_config_gpio_exti_call(GPIO_Pin);
}

INLINE void ecu_config_set_ignition_enabled(bool enabled) {
  HAL_GPIO_WritePin(IGN_NEN_GPIO_Port, IGN_NEN_Pin, enabled ? GPIO_PIN_RESET : GPIO_PIN_SET);
}

INLINE void ecu_config_iwgd_refresh(void) {
  HAL_IWDG_Refresh(&hiwdg1);
}

void ecu_config_ll_init(void)
{
  __HAL_DBGMCU_FREEZE_TIM5();
  __HAL_DBGMCU_FREEZE_TIM6();
  __HAL_DBGMCU_FREEZE_TIM7();
  __HAL_DBGMCU_FREEZE_IWDG1();
  __HAL_DBGMCU_FREEZE2_IWDG2();
}

void ecu_config_init_counter(void)
{
  htim2.Init.Period = ECU_TICKBASE_MASK;
  htim5.Init.Period = ECU_TIMEBASE_MASK;

  HAL_TIM_Base_Init(&htim5);
  HAL_TIM_Base_Init(&htim2);

  time_init_tickbase(&htim2.Instance->CNT, ECU_TICKBASE_MASK);
  time_init_timebase(&htim5.Instance->CNT, ECU_TIMEBASE_MASK);
}

void ecu_config_start_counter(void)
{
  HAL_TIM_Base_Start(&htim2);
  HAL_TIM_Base_Start(&htim5);
}

void ecu_config_start_periodic_timers(pTIM_CallbackTypeDef func_tim_slow_irq_cb, pTIM_CallbackTypeDef func_tim_fast_irq_cb)
{
  HAL_TIM_RegisterCallback(&htim6, HAL_TIM_PERIOD_ELAPSED_CB_ID, func_tim_slow_irq_cb);
  HAL_TIM_RegisterCallback(&htim7, HAL_TIM_PERIOD_ELAPSED_CB_ID, func_tim_fast_irq_cb);

  HAL_TIM_Base_Start_IT(&htim6);
  HAL_TIM_Base_Start_IT(&htim7);
}
