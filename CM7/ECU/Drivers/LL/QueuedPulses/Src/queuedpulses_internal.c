/*
 * queuedpulses_internal.c
 *
 *  Created on: Mar 26, 2024
 *      Author: VHEMaster
 */

#include "queuedpulses_internal.h"
#include "compiler.h"

ITCM_FUNC void queuedpulses_internal_tim_enable(queuedpulse_timer_t *timer, uint32_t prd, uint32_t psc)
{
  while(prd > 0x10000) {
    prd >>= 1;
    psc <<= 1;
  }

  timer->htim->Instance->PSC = psc - 1;
  timer->htim->Instance->ARR = prd - 1;
  timer->htim->Instance->EGR |= 1;

  __HAL_TIM_CLEAR_IT(timer->htim, TIM_IT_UPDATE);
  __HAL_TIM_CLEAR_FLAG(timer->htim, TIM_FLAG_UPDATE);

  __HAL_TIM_ENABLE_IT(timer->htim, TIM_IT_UPDATE);
  __HAL_TIM_ENABLE(timer->htim);
}


ITCM_FUNC void queuedpulses_internal_tim_disable(queuedpulse_timer_t *timer)
{
  __HAL_TIM_DISABLE_IT(timer->htim, TIM_IT_UPDATE);
  __HAL_TIM_DISABLE(timer->htim);
}


ITCM_FUNC time_delta_us_t queuedpulses_internal_calculate_pulse_cplt_time(queuedpulse_entry_t *entry, time_us_t now)
{
  time_us_t absolute_time = entry->time + entry->pulse;
  time_delta_us_t relative_time = time_diff(absolute_time, now);

  return relative_time;
}
