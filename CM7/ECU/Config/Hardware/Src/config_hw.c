/*
 * config.c
 *
 *  Created on: Apr 5, 2024
 *      Author: VHEMaster
 */

#include "config_hw.h"
#include "main.h"
#include "config_gpio.h"
#include "config_extern.h"
#include "compiler.h"
#include "time.h"

#define ECU_CONFIG_HW_SWI_COMM_PERIOD_US    (100)

static void ecu_config_trigger_comm_irq(void);

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

void ecu_config_start_periodic_timers(pTIM_CallbackTypeDef func_tim_slow_irq_cb, void (*func_tim_comm_irq_cb)(void), pTIM_CallbackTypeDef func_tim_fast_irq_cb)
{
  HAL_TIM_RegisterCallback(&htim6, HAL_TIM_PERIOD_ELAPSED_CB_ID, func_tim_slow_irq_cb);
  HAL_TIM_RegisterCallback(&htim7, HAL_TIM_PERIOD_ELAPSED_CB_ID, func_tim_fast_irq_cb);

  HAL_EXTI_RegisterCallback(&hexti1, HAL_EXTI_COMMON_CB_ID, func_tim_comm_irq_cb);
  HAL_EXTI_SetConfigLine(&hexti1, &(EXTI_ConfigTypeDef){
    .Line = EXTI_LINE_1,
    .Mode = EXTI_MODE_INTERRUPT,
    .Trigger = EXTI_TRIGGER_RISING,
  });

  HAL_TIM_Base_Start_IT(&htim6);
  HAL_TIM_Base_Start_IT(&htim7);

}

void ecu_config_swi_poll(void)
{
  static time_us_t last_poll_time = 0;
  uint32_t now = time_now_us();

  if(time_diff(now, last_poll_time) >= ECU_CONFIG_HW_SWI_COMM_PERIOD_US) {
    if(time_diff(now, last_poll_time) >= ECU_CONFIG_HW_SWI_COMM_PERIOD_US * 3) {
      last_poll_time = now;
    } else {
      last_poll_time += ECU_CONFIG_HW_SWI_COMM_PERIOD_US;
    }
    ecu_config_trigger_comm_irq();
  }
}

STATIC_INLINE void ecu_config_trigger_comm_irq(void)
{
  if(hexti1.PendingCallback != NULL) {
    HAL_EXTI_GenerateSWI(&hexti1);
  }
}
