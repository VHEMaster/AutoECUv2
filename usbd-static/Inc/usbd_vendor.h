#ifndef USBD_VENDOR_H_
#define USBD_VENDOR_H_

#include "usbd_core.h"

#define USBD_VENDOR_EP_IN  0x81u
#define USBD_VENDOR_EP_OUT 0x01u
#define USBD_VENDOR_MPS    64u

extern USBD_ClassTypeDef USBD_VENDOR;

USBD_StatusTypeDef USBD_VENDOR_Receive(USBD_HandleTypeDef *pdev, uint8_t *data, uint16_t len);
USBD_StatusTypeDef USBD_VENDOR_Transmit(USBD_HandleTypeDef *pdev, uint8_t *data, uint16_t len);

/* Application hooks execute in USB IRQ context. */
void USBD_VENDOR_Opened(USBD_HandleTypeDef *pdev);
void USBD_VENDOR_Closed(USBD_HandleTypeDef *pdev);
void USBD_VENDOR_Received(USBD_HandleTypeDef *pdev, uint32_t len);
void USBD_VENDOR_Transmitted(USBD_HandleTypeDef *pdev);
uint8_t USBD_VENDOR_Request(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);

#endif
