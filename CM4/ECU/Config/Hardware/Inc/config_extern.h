#ifndef CM4_CONFIG_EXTERN_H
#define CM4_CONFIG_EXTERN_H
#include "main.h"
extern FDCAN_HandleTypeDef hfdcan1;
extern UART_HandleTypeDef huart1, huart3;
extern DMA_HandleTypeDef hdma_usart1_rx, hdma_usart1_tx, hdma_usart3_rx, hdma_usart3_tx;
#endif
