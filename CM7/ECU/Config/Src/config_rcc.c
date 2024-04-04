/*
 * config_rcc.c
 *
 *  Created on: Apr 4, 2024
 *      Author: VHEMaster
 */

#include "config_rcc.h"

error_t ecu_config_get_tim_base_frequency(TIM_HandleTypeDef *htim, uint32_t *tim_base_freq)
{
  RCC_ClkInitTypeDef clkConfig;
  uint32_t freq, latency, periph_reg;
  error_t err = E_OK;

  periph_reg = (uint32_t)htim->Instance;

  HAL_RCC_GetClockConfig(&clkConfig, &latency);

  if (periph_reg >= APB2PERIPH_BASE) {
    freq = HAL_RCC_GetPCLK2Freq();
    if(clkConfig.APB2CLKDivider != RCC_APB2_DIV1){
      freq *= 2;
    }
  } else if (periph_reg >= APB1PERIPH_BASE) {
    freq = HAL_RCC_GetPCLK1Freq();
    if (clkConfig.APB1CLKDivider != RCC_APB1_DIV1) {
      freq *= 2;
    }
  }

  if(tim_base_freq != NULL) {
    *tim_base_freq = freq;
  }

  return err;
}
