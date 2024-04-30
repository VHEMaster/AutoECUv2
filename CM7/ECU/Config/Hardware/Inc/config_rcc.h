/*
 * config_rcc.h
 *
 *  Created on: Apr 4, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_INC_CONFIG_RCC_H_
#define CONFIG_INC_CONFIG_RCC_H_

#include "main.h"
#include "errors.h"

error_t ecu_config_get_tim_base_frequency(TIM_HandleTypeDef *htim, uint32_t *tim_base_freq);

#endif /* CONFIG_INC_CONFIG_RCC_H_ */
