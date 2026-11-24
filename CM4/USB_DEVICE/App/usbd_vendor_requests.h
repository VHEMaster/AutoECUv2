#ifndef USBD_VENDOR_REQUESTS_H_
#define USBD_VENDOR_REQUESTS_H_
#include "usbd_core.h"

#define USBD_VENDOR_MS_OS_20 0x20u
#define USBD_VENDOR_GET_DEVICE_INFO 0x30u
#define USBD_VENDOR_GET_LINK_STATUS 0x31u
#define USBD_VENDOR_GET_ACTIVE_TRANSPORT 0x32u
#define USBD_VENDOR_REQUEST_CONNECTION 0x33u
#define USBD_VENDOR_RELEASE_CONNECTION 0x34u

uint8_t *USBD_VENDOR_BOS(USBD_SpeedTypeDef speed, uint16_t *length);
void USBD_VENDOR_SetLinkStatus(uint8_t owner, uint8_t link_up, uint8_t configured);
uint8_t USBD_VENDOR_TakeRequests(void);

#endif
