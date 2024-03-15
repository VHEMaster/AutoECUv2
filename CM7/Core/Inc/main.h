/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LAMBDA_HEATER_NEN_Pin GPIO_PIN_4
#define LAMBDA_HEATER_NEN_GPIO_Port GPIOE
#define LAMBDA1_HEATER_Pin GPIO_PIN_5
#define LAMBDA1_HEATER_GPIO_Port GPIOE
#define LAMBDA2_HEATER_Pin GPIO_PIN_6
#define LAMBDA2_HEATER_GPIO_Port GPIOE
#define LOGIC_OE_Pin GPIO_PIN_13
#define LOGIC_OE_GPIO_Port GPIOC
#define IGN_CH5_Pin GPIO_PIN_0
#define IGN_CH5_GPIO_Port GPIOF
#define IGN_CH6_Pin GPIO_PIN_1
#define IGN_CH6_GPIO_Port GPIOF
#define IGN_CH7_Pin GPIO_PIN_2
#define IGN_CH7_GPIO_Port GPIOF
#define IGN_CH8_Pin GPIO_PIN_3
#define IGN_CH8_GPIO_Port GPIOF
#define MOTOR1_DIR_Pin GPIO_PIN_4
#define MOTOR1_DIR_GPIO_Port GPIOF
#define MOTOR2_DIR_Pin GPIO_PIN_5
#define MOTOR2_DIR_GPIO_Port GPIOF
#define FLEXIO1_AOX_Pin GPIO_PIN_0
#define FLEXIO1_AOX_GPIO_Port GPIOC
#define FLEXIO2_AOX_Pin GPIO_PIN_1
#define FLEXIO2_AOX_GPIO_Port GPIOC
#define FLEXIO1_SENT1_Pin GPIO_PIN_0
#define FLEXIO1_SENT1_GPIO_Port GPIOA
#define FLEXIO1_SENT2_Pin GPIO_PIN_1
#define FLEXIO1_SENT2_GPIO_Port GPIOA
#define FLEXIO1_SENT3_Pin GPIO_PIN_2
#define FLEXIO1_SENT3_GPIO_Port GPIOA
#define FLEXIO1_SENT4_Pin GPIO_PIN_3
#define FLEXIO1_SENT4_GPIO_Port GPIOA
#define OUTS1_IN9_Pin GPIO_PIN_6
#define OUTS1_IN9_GPIO_Port GPIOA
#define OUTS1_IN10_Pin GPIO_PIN_7
#define OUTS1_IN10_GPIO_Port GPIOA
#define SPI6_NSS_EGT1_Pin GPIO_PIN_4
#define SPI6_NSS_EGT1_GPIO_Port GPIOC
#define SPI6_NSS_EGT2_Pin GPIO_PIN_5
#define SPI6_NSS_EGT2_GPIO_Port GPIOC
#define OUTS2_CH3_Pin GPIO_PIN_0
#define OUTS2_CH3_GPIO_Port GPIOB
#define OUTS2_CH4_Pin GPIO_PIN_1
#define OUTS2_CH4_GPIO_Port GPIOB
#define IGN_NEN_Pin GPIO_PIN_2
#define IGN_NEN_GPIO_Port GPIOB
#define IGN_CH1_Pin GPIO_PIN_12
#define IGN_CH1_GPIO_Port GPIOF
#define IGN_CH2_Pin GPIO_PIN_13
#define IGN_CH2_GPIO_Port GPIOF
#define IGN_CH3_Pin GPIO_PIN_14
#define IGN_CH3_GPIO_Port GPIOF
#define IGN_CH4_Pin GPIO_PIN_15
#define IGN_CH4_GPIO_Port GPIOF
#define SPI5_NSS_OUTSM1_Pin GPIO_PIN_0
#define SPI5_NSS_OUTSM1_GPIO_Port GPIOG
#define SPI5_NSS_OUTSM2_Pin GPIO_PIN_1
#define SPI5_NSS_OUTSM2_GPIO_Port GPIOG
#define OUTS1_CH2_Pin GPIO_PIN_11
#define OUTS1_CH2_GPIO_Port GPIOE
#define FLEXIO1_INT_Pin GPIO_PIN_12
#define FLEXIO1_INT_GPIO_Port GPIOE
#define FLEXIO2_INT_Pin GPIO_PIN_13
#define FLEXIO2_INT_GPIO_Port GPIOE
#define OUTS1_CH4_Pin GPIO_PIN_14
#define OUTS1_CH4_GPIO_Port GPIOE
#define MOTOR1_DIS_Pin GPIO_PIN_11
#define MOTOR1_DIS_GPIO_Port GPIOB
#define MOTOR2_DIS_Pin GPIO_PIN_12
#define MOTOR2_DIS_GPIO_Port GPIOB
#define FLEXIO1_VRSO_Pin GPIO_PIN_14
#define FLEXIO1_VRSO_GPIO_Port GPIOB
#define FLEXIO2_VRSO_Pin GPIO_PIN_15
#define FLEXIO2_VRSO_GPIO_Port GPIOB
#define USART3_LO_Pin GPIO_PIN_10
#define USART3_LO_GPIO_Port GPIOD
#define OUTS2_CH9_Pin GPIO_PIN_12
#define OUTS2_CH9_GPIO_Port GPIOD
#define OUTS2_CH10_Pin GPIO_PIN_13
#define OUTS2_CH10_GPIO_Port GPIOD
#define OUTS2_CH11_Pin GPIO_PIN_14
#define OUTS2_CH11_GPIO_Port GPIOD
#define OUTS2_CH12_Pin GPIO_PIN_15
#define OUTS2_CH12_GPIO_Port GPIOD
#define FLEXIO1_WAKE_Pin GPIO_PIN_8
#define FLEXIO1_WAKE_GPIO_Port GPIOJ
#define OUTS1_CH3_Pin GPIO_PIN_9
#define OUTS1_CH3_GPIO_Port GPIOJ
#define FLEXIO2_WAKE_Pin GPIO_PIN_10
#define FLEXIO2_WAKE_GPIO_Port GPIOJ
#define OUTS1_CH1_Pin GPIO_PIN_1
#define OUTS1_CH1_GPIO_Port GPIOK
#define KNOCK_INT_Pin GPIO_PIN_2
#define KNOCK_INT_GPIO_Port GPIOK
#define SPI5_NSS_OUTS1_Pin GPIO_PIN_2
#define SPI5_NSS_OUTS1_GPIO_Port GPIOG
#define SPI5_NSS_OUTS2_Pin GPIO_PIN_3
#define SPI5_NSS_OUTS2_GPIO_Port GPIOG
#define FLEXIO2_SENT1_Pin GPIO_PIN_4
#define FLEXIO2_SENT1_GPIO_Port GPIOG
#define FLEXIO2_SENT2_Pin GPIO_PIN_5
#define FLEXIO2_SENT2_GPIO_Port GPIOG
#define FLEXIO2_SENT3_Pin GPIO_PIN_6
#define FLEXIO2_SENT3_GPIO_Port GPIOG
#define FLEXIO2_SENT4_Pin GPIO_PIN_7
#define FLEXIO2_SENT4_GPIO_Port GPIOG
#define LAMBDA_NRST_Pin GPIO_PIN_8
#define LAMBDA_NRST_GPIO_Port GPIOG
#define OUTS2_CH1_Pin GPIO_PIN_6
#define OUTS2_CH1_GPIO_Port GPIOC
#define OUTS2_CH2_Pin GPIO_PIN_7
#define OUTS2_CH2_GPIO_Port GPIOC
#define MOTOR2_PWM_Pin GPIO_PIN_8
#define MOTOR2_PWM_GPIO_Port GPIOC
#define MOTOR1_PWM_Pin GPIO_PIN_9
#define MOTOR1_PWM_GPIO_Port GPIOC
#define FLEXIO_NRST_Pin GPIO_PIN_15
#define FLEXIO_NRST_GPIO_Port GPIOA
#define STEP_ERR_Pin GPIO_PIN_12
#define STEP_ERR_GPIO_Port GPIOC
#define FDCAN1_LBK_Pin GPIO_PIN_2
#define FDCAN1_LBK_GPIO_Port GPIOD
#define STEP_I0_Pin GPIO_PIN_3
#define STEP_I0_GPIO_Port GPIOD
#define STEP_I1_Pin GPIO_PIN_4
#define STEP_I1_GPIO_Port GPIOD
#define STEP_PH1_Pin GPIO_PIN_5
#define STEP_PH1_GPIO_Port GPIOD
#define STEP_PH2_Pin GPIO_PIN_6
#define STEP_PH2_GPIO_Port GPIOD
#define SPI2_NSS_FLEXIO1_Pin GPIO_PIN_7
#define SPI2_NSS_FLEXIO1_GPIO_Port GPIOD
#define SPI6_NSS_LAMBDA1_Pin GPIO_PIN_10
#define SPI6_NSS_LAMBDA1_GPIO_Port GPIOG
#define OUTS_RESET_Pin GPIO_PIN_11
#define OUTS_RESET_GPIO_Port GPIOG
#define SPI6_NSS_LAMBDA2_Pin GPIO_PIN_15
#define SPI6_NSS_LAMBDA2_GPIO_Port GPIOG
#define SPI3_NSS_FLEXIO2_Pin GPIO_PIN_6
#define SPI3_NSS_FLEXIO2_GPIO_Port GPIOB
#define SPI6_NSS_KNOCK_Pin GPIO_PIN_7
#define SPI6_NSS_KNOCK_GPIO_Port GPIOB
#define OUTS1_IN11_Pin GPIO_PIN_8
#define OUTS1_IN11_GPIO_Port GPIOB
#define OUTS1_IN12_Pin GPIO_PIN_9
#define OUTS1_IN12_GPIO_Port GPIOB
#define FLEXIO1_SYNC_Pin GPIO_PIN_0
#define FLEXIO1_SYNC_GPIO_Port GPIOE
#define FLEXIO2_SYNC_Pin GPIO_PIN_1
#define FLEXIO2_SYNC_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */
#define LD_GPIO_Port GPIOI

#define LD1_Pin GPIO_PIN_12
#define LD1_GPIO_Port LD_GPIO_Port
#define LD2_Pin GPIO_PIN_13
#define LD2_GPIO_Port LD_GPIO_Port
#define LD3_Pin GPIO_PIN_14
#define LD3_GPIO_Port LD_GPIO_Port
#define LD4_Pin GPIO_PIN_15
#define LD4_GPIO_Port LD_GPIO_Port
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
