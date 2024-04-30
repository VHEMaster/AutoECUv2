/*
 * config_extern.h
 *
 *  Created on: Apr 3, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_EXTERN_H_
#define CONFIG_EXTERN_H_

#include "main.h"

extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;
extern DMA_HandleTypeDef hdma_adc1;
extern DMA_HandleTypeDef hdma_adc2;

extern CRC_HandleTypeDef hcrc;

extern DAC_HandleTypeDef hdac1;

extern FDCAN_HandleTypeDef hfdcan1;

extern HRTIM_HandleTypeDef hhrtim;

extern IWDG_HandleTypeDef hiwdg1;

extern QSPI_HandleTypeDef hqspi;
extern MDMA_HandleTypeDef hmdma_quadspi_fifo_th;

extern RTC_HandleTypeDef hrtc;

extern SPI_HandleTypeDef hspi2;
extern SPI_HandleTypeDef hspi3;
extern SPI_HandleTypeDef hspi5;
extern SPI_HandleTypeDef hspi6;

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim5;
extern TIM_HandleTypeDef htim6;
extern TIM_HandleTypeDef htim7;
extern TIM_HandleTypeDef htim8;
extern TIM_HandleTypeDef htim12;
extern TIM_HandleTypeDef htim13;
extern TIM_HandleTypeDef htim14;
extern TIM_HandleTypeDef htim15;
extern TIM_HandleTypeDef htim16;
extern TIM_HandleTypeDef htim17;

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart3;
extern DMA_HandleTypeDef hdma_usart1_rx;
extern DMA_HandleTypeDef hdma_usart1_tx;
extern DMA_HandleTypeDef hdma_usart3_rx;
extern DMA_HandleTypeDef hdma_usart3_tx;

#endif /* CONFIG_EXTERN_H_ */
