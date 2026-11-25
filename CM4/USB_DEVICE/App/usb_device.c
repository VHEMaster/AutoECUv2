/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : usb_device.c
  * @version        : v1.0_Cube
  * @brief          : This file implements the USB Device
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

/* Includes ------------------------------------------------------------------*/

#include "main.h"
#include "usb_device.h"
#include "usbd_core.h"
#include "usbd_desc.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USB Device Core handle declaration. */
USBD_HandleTypeDef hUsbDeviceFS;

/*
 * -- Insert your variables declaration here --
 */
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*
 * -- Insert your external function declaration here --
 */
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

USBD_StatusTypeDef USB_DEVICE_Init(void)
{
  HAL_PWREx_EnableUSBVoltageDetector();
  return USBD_Init(&hUsbDeviceFS, &FS_Desc, DEVICE_FS);
}

USBD_StatusTypeDef USB_DEVICE_Start(void)
{
  return USBD_Start(&hUsbDeviceFS);
}

void USB_DEVICE_Stop(void)
{
  (void)USBD_Stop(&hUsbDeviceFS);
}

USBD_StatusTypeDef USB_DEVICE_DeInit(void)
{
  return USBD_DeInit(&hUsbDeviceFS);
}

/**
  * @}
  */

/**
  * @}
  */

