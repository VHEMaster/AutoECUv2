/*
 * config.c
 *
 *  Created on: Apr 5, 2024
 *      Author: VHEMaster
 */

#include "config_hw.h"
#include "main.h"
#include "config_extern.h"
#include "compiler.h"
#include "time.h"

#ifdef CORE_CM7
#include "config_gpio.h"
#endif /* CORE_CM7 */

#ifdef CORE_CM7
static void ecu_config_trigger_comm_irq(void);
static void ecu_config_trigger_slow_irq(void);
static void ecu_config_tim_100us_irq_cb(TIM_HandleTypeDef *htim);
#endif /* CORE_CM7 */

#ifdef CORE_CM7
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  ecu_config_gpio_exti_call(GPIO_Pin);
}

INLINE void ecu_config_set_ignition_enabled(bool enabled) {
  //HAL_GPIO_WritePin(IGN_NEN_GPIO_Port, IGN_NEN_Pin, enabled ? GPIO_PIN_RESET : GPIO_PIN_SET);
  if(enabled) {
    IGN_NEN_GPIO_Port->BSRR = IGN_NEN_Pin << 16;
  } else {
    IGN_NEN_GPIO_Port->BSRR = IGN_NEN_Pin;
  }
}
#endif /* CORE_CM7 */

INLINE void ecu_config_iwgd_refresh(void) {
  IWDG_HandleTypeDef *hiwdg = NULL;

#ifdef CORE_CM7
  hiwdg = &hiwdg1;
#endif /* CORE_CM7 */

#ifdef CORE_CM4
  hiwdg = &hiwdg2;
#endif /* CORE_CM4 */

  if(hiwdg != NULL && hiwdg->Instance != NULL) {
    HAL_IWDG_Refresh(hiwdg);
  }
}

void ecu_config_ll_init(void)
{
  __HAL_DBGMCU_FREEZE_TIM5();
  __HAL_DBGMCU_FREEZE_TIM6();
  __HAL_DBGMCU_FREEZE_TIM7();
  __HAL_DBGMCU_FREEZE_IWDG1();
  __HAL_DBGMCU_FREEZE2_IWDG2();

  __HAL_DBGMCU_FREEZE_FDCAN();
}

void ecu_config_init_swi(void)
{
#ifdef CORE_CM7
  HAL_EXTI_SetConfigLine(&hexti1, &(EXTI_ConfigTypeDef){
    .Line = EXTI_LINE_1,
    .Mode = EXTI_MODE_CORE1_INTERRUPT | EXTI_MODE_CORE2_INTERRUPT,
    .Trigger = EXTI_TRIGGER_RISING,
  });

  HAL_EXTI_SetConfigLine(&hexti2, &(EXTI_ConfigTypeDef){
    .Line = EXTI_LINE_2,
    .Mode = EXTI_MODE_CORE1_INTERRUPT | EXTI_MODE_CORE2_INTERRUPT,
    .Trigger = EXTI_TRIGGER_RISING,
  });
#endif /* CORE_CM7 */

#ifdef CORE_CM4
  HAL_EXTI_GetHandle(&hexti1, EXTI_LINE_1);
  HAL_EXTI_GetHandle(&hexti2, EXTI_LINE_2);
#endif /* CORE_CM4 */
}

void ecu_config_init_counter(void)
{
#ifdef CORE_CM7
  htim2.Init.Period = TIME_TICKBASE_MASK;
  htim5.Init.Period = TIME_TIMEBASE_MASK;

  HAL_TIM_Base_Init(&htim5);
  HAL_TIM_Base_Init(&htim2);

#endif /* CORE_CM7 */

  time_init_tickbase(&TIM2->CNT);
  time_init_timebase(&TIM5->CNT);
}

#ifdef CORE_CM7
void ecu_config_start_counter(void)
{
  HAL_TIM_Base_Start(&htim2);
  HAL_TIM_Base_Start(&htim5);
}

static void ecu_config_tim_100us_irq_cb(TIM_HandleTypeDef *htim)
{
  static time_us_t slow_last = 0;
  time_us_t now = time_now_us();

  if(time_diff(now, slow_last) >= 1 * TIME_US_IN_MS) {
    ecu_config_trigger_slow_irq();
    slow_last = now;
  }
  ecu_config_trigger_comm_irq();
}
#endif /* CORE_CM7 */

void ecu_config_start_periodic_timers(void (*func_tim_slow_irq_cb)(void), void (*func_tim_comm_irq_cb)(void), pTIM_CallbackTypeDef func_tim_fast_irq_cb)
{
  HAL_EXTI_RegisterCallback(&hexti1, HAL_EXTI_COMMON_CB_ID, func_tim_comm_irq_cb);
  HAL_EXTI_RegisterCallback(&hexti2, HAL_EXTI_COMMON_CB_ID, func_tim_slow_irq_cb);
  HAL_EXTI_ClearPending(&hexti1, EXTI_TRIGGER_RISING);
  HAL_EXTI_ClearPending(&hexti2, EXTI_TRIGGER_RISING);

  HAL_NVIC_SetPriority(EXTI1_IRQn, NVIC_PRIO_EXTI1_COMM, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);
  HAL_NVIC_SetPriority(EXTI2_IRQn, NVIC_PRIO_EXTI2_SLOW, 0);
  HAL_NVIC_EnableIRQ(EXTI2_IRQn);

#ifdef CORE_CM7
  HAL_TIM_RegisterCallback(&htim6, HAL_TIM_PERIOD_ELAPSED_CB_ID, ecu_config_tim_100us_irq_cb);
  HAL_TIM_RegisterCallback(&htim7, HAL_TIM_PERIOD_ELAPSED_CB_ID, func_tim_fast_irq_cb);

  HAL_TIM_Base_Start_IT(&htim6);
  HAL_TIM_Base_Start_IT(&htim7);
#endif /* CORE_CM7 */
}

#ifdef CORE_CM7
STATIC_INLINE void ecu_config_trigger_comm_irq(void)
{
  HAL_EXTI_GenerateSWI(&hexti1);
}

STATIC_INLINE void ecu_config_trigger_slow_irq(void)
{
  HAL_EXTI_GenerateSWI(&hexti2);
}
#endif /* CORE_CM7 */
