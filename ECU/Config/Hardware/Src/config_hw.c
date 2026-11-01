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

#define ECU_CONFIG_HW_SWI_COMM_PERIOD_US    (100)

static void ecu_config_trigger_comm_irq(void);
static void ecu_config_trigger_cm4_comm_irq(void);
static void ecu_config_trigger_cm4_slow_irq(void);

#ifdef CORE_CM7
void (*ecu_config_tim_slow_irq_cb_func)(void);

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
    .Mode = EXTI_MODE_CORE1_INTERRUPT,
    .Trigger = EXTI_TRIGGER_RISING,
  });

  HAL_EXTI_SetConfigLine(&hexti2, &(EXTI_ConfigTypeDef){
    .Line = EXTI_LINE_2,
    .Mode = EXTI_MODE_CORE2_INTERRUPT,
    .Trigger = EXTI_TRIGGER_RISING,
  });

  HAL_EXTI_SetConfigLine(&hexti3, &(EXTI_ConfigTypeDef){
    .Line = EXTI_LINE_3,
    .Mode = EXTI_MODE_CORE2_INTERRUPT,
    .Trigger = EXTI_TRIGGER_RISING,
  });
#endif /* CORE_CM7 */

#ifdef CORE_CM4
  HAL_EXTI_GetHandle(&hexti2, EXTI_LINE_2);
  HAL_EXTI_GetHandle(&hexti3, EXTI_LINE_3);
#endif /* CORE_CM4 */
}

#ifdef CORE_CM7
void ecu_config_init_counter(void)
{
  htim2.Init.Period = TIME_TICKBASE_MASK;
  htim5.Init.Period = TIME_TIMEBASE_MASK;

  HAL_TIM_Base_Init(&htim5);
  HAL_TIM_Base_Init(&htim2);

  time_init_tickbase(&htim2.Instance->CNT);
  time_init_timebase(&htim5.Instance->CNT);
}

void ecu_config_start_counter(void)
{
  HAL_TIM_Base_Start(&htim2);
  HAL_TIM_Base_Start(&htim5);
}

static void ecu_config_tim_slow_irq_cb(TIM_HandleTypeDef *htim)
{
  ecu_config_trigger_cm4_slow_irq();
  if(ecu_config_tim_slow_irq_cb_func != NULL) {
    ecu_config_tim_slow_irq_cb_func();
  }
}
#endif /* CORE_CM7 */

void ecu_config_start_periodic_timers(void (*func_tim_slow_irq_cb)(void), void (*func_tim_comm_irq_cb)(void), pTIM_CallbackTypeDef func_tim_fast_irq_cb)
{
#ifdef CORE_CM7
  HAL_EXTI_RegisterCallback(&hexti1, HAL_EXTI_COMMON_CB_ID, func_tim_comm_irq_cb);
  HAL_EXTI_ClearPending(&hexti1, EXTI_TRIGGER_RISING);

  HAL_NVIC_SetPriority(EXTI1_IRQn, NVIC_PRIO_EXTI1_COMM, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);

  ecu_config_tim_slow_irq_cb_func = func_tim_slow_irq_cb;
  HAL_TIM_RegisterCallback(&htim6, HAL_TIM_PERIOD_ELAPSED_CB_ID, ecu_config_tim_slow_irq_cb);
  HAL_TIM_RegisterCallback(&htim7, HAL_TIM_PERIOD_ELAPSED_CB_ID, func_tim_fast_irq_cb);

  HAL_TIM_Base_Start_IT(&htim6);
  HAL_TIM_Base_Start_IT(&htim7);
#endif /* CORE_CM7 */

#ifdef CORE_CM4
  HAL_EXTI_RegisterCallback(&hexti2, HAL_EXTI_COMMON_CB_ID, func_tim_comm_irq_cb);
  HAL_EXTI_RegisterCallback(&hexti3, HAL_EXTI_COMMON_CB_ID, func_tim_slow_irq_cb);
  HAL_EXTI_ClearPending(&hexti2, EXTI_TRIGGER_RISING);
  HAL_EXTI_ClearPending(&hexti3, EXTI_TRIGGER_RISING);

  HAL_NVIC_SetPriority(EXTI2_IRQn, NVIC_PRIO_EXTI2_COMM, 0);
  HAL_NVIC_EnableIRQ(EXTI2_IRQn);

  HAL_NVIC_SetPriority(EXTI3_IRQn, NVIC_PRIO_EXTI3_SLOW, 0);
  HAL_NVIC_EnableIRQ(EXTI3_IRQn);
#endif /* CORE_CM4 */

}

#ifdef CORE_CM7
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
    ecu_config_trigger_cm4_comm_irq();
    ecu_config_trigger_comm_irq();
  }
}

STATIC_INLINE void ecu_config_trigger_comm_irq(void)
{
  HAL_EXTI_GenerateSWI(&hexti1);
}

STATIC_INLINE void ecu_config_trigger_cm4_comm_irq(void)
{
  HAL_EXTI_GenerateSWI(&hexti2);
}

STATIC_INLINE void ecu_config_trigger_cm4_slow_irq(void)
{
  HAL_EXTI_GenerateSWI(&hexti3);
}
#endif /* CORE_CM7 */
