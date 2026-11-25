#include "usbd_vendor.h"
#include "usbd_ctlreq.h"

static uint8_t config_desc[] = {
  9,                              /* bLength: configuration header size in bytes. */
  USB_DESC_TYPE_CONFIGURATION,    /* bDescriptorType: Configuration. */
  32, 0,                          /* wTotalLength: 9-byte configuration + 9-byte interface + two 7-byte endpoints. */
  1,                              /* bNumInterfaces: one interface. */
  1,                              /* bConfigurationValue: value used in SET_CONFIGURATION. */
  0,                              /* iConfiguration: no configuration string index. */
  0xC0,                           /* bmAttributes: mandatory bit 7 and self-powered bit 6. */
  0,                              /* bMaxPower: zero bus power, in 2 mA units. */

  9,                              /* bLength: interface descriptor size in bytes. */
  USB_DESC_TYPE_INTERFACE,        /* bDescriptorType: Interface. */
  0,                              /* bInterfaceNumber: interface 0. */
  0,                              /* bAlternateSetting: default setting. */
  2,                              /* bNumEndpoints: two endpoints beyond EP0. */
  0xFF,                           /* bInterfaceClass: vendor-specific. */
  0,                              /* bInterfaceSubClass: zero. */
  0,                              /* bInterfaceProtocol: zero. */
  0,                              /* iInterface: no interface string index. */

  7,                              /* bLength: endpoint descriptor size in bytes. */
  USB_DESC_TYPE_ENDPOINT,         /* bDescriptorType: Endpoint. */
  USBD_VENDOR_EP_OUT,             /* bEndpointAddress: endpoint 1 OUT. */
  2,                              /* bmAttributes: bulk transfer. */
  USBD_VENDOR_MPS, 0,             /* wMaxPacketSize: full-speed bulk maximum in bytes. */
  0,                              /* bInterval: unused for full-speed bulk. */

  7,                              /* bLength: endpoint descriptor size in bytes. */
  USB_DESC_TYPE_ENDPOINT,         /* bDescriptorType: Endpoint. */
  USBD_VENDOR_EP_IN,              /* bEndpointAddress: endpoint 1 IN. */
  2,                              /* bmAttributes: bulk transfer. */
  USBD_VENDOR_MPS, 0,             /* wMaxPacketSize: full-speed bulk maximum in bytes. */
  0                               /* bInterval: unused for full-speed bulk. */
};

static uint8_t *vendor_get_config(uint16_t *length)
{
  *length = sizeof(config_desc);
  return config_desc;
}

static uint8_t vendor_init(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
  (void)cfgidx;
  if(USBD_LL_OpenEP(pdev, USBD_VENDOR_EP_OUT, USBD_EP_TYPE_BULK, USBD_VENDOR_MPS) != USBD_OK)
    return USBD_FAIL;
  if(USBD_LL_OpenEP(pdev, USBD_VENDOR_EP_IN, USBD_EP_TYPE_BULK, USBD_VENDOR_MPS) != USBD_OK) {
    USBD_LL_CloseEP(pdev, USBD_VENDOR_EP_OUT);
    return USBD_FAIL;
  }
  pdev->ep_out[1].is_used = 1u;
  pdev->ep_in[1].is_used = 1u;
  USBD_VENDOR_Opened(pdev);
  return USBD_OK;
}

static uint8_t vendor_deinit(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
  (void)cfgidx;
  if(pdev->ep_out[1].is_used) USBD_LL_CloseEP(pdev, USBD_VENDOR_EP_OUT);
  if(pdev->ep_in[1].is_used) USBD_LL_CloseEP(pdev, USBD_VENDOR_EP_IN);
  pdev->ep_out[1].is_used = 0u;
  pdev->ep_in[1].is_used = 0u;
  USBD_VENDOR_Closed(pdev);
  return USBD_OK;
}

static uint8_t vendor_setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{
  static uint8_t zero[2];
  if((req->bmRequest & USB_REQ_TYPE_MASK) == USB_REQ_TYPE_VENDOR) {
    if(USBD_VENDOR_Request(pdev, req) == USBD_OK) return USBD_OK;
  } else if(req->bmRequest == 0x81u && req->wIndex == 0u && req->wValue == 0u) {
    if(req->bRequest == USB_REQ_GET_INTERFACE && req->wLength == 1u)
      return USBD_CtlSendData(pdev, zero, 1u);
    if(req->bRequest == USB_REQ_GET_STATUS && req->wLength == 2u)
      return USBD_CtlSendData(pdev, zero, 2u);
  } else if(req->bmRequest == 0x01u && req->bRequest == USB_REQ_SET_INTERFACE &&
      req->wIndex == 0u && req->wValue == 0u && req->wLength == 0u) {
    return USBD_OK;
  }

  /* Core already clears the halt and sends the status stage. */
  if(req->bmRequest == 0x02u && req->bRequest == USB_REQ_CLEAR_FEATURE &&
      req->wValue == USB_FEATURE_EP_HALT && req->wLength == 0u &&
      (req->wIndex == USBD_VENDOR_EP_IN || req->wIndex == USBD_VENDOR_EP_OUT)) {
    return USBD_OK;
  }

  USBD_CtlError(pdev, req);
  return USBD_FAIL;
}

static uint8_t vendor_data_in(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
  if(epnum != 1u) return USBD_FAIL;
  USBD_VENDOR_Transmitted(pdev);
  return USBD_OK;
}

static uint8_t vendor_data_out(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
  if(epnum != 1u) return USBD_FAIL;
  USBD_VENDOR_Received(pdev, USBD_LL_GetRxDataSize(pdev, epnum));
  return USBD_OK;
}

USBD_ClassTypeDef USBD_VENDOR = {
  .Init = vendor_init,
  .DeInit = vendor_deinit,
  .Setup = vendor_setup,
  .DataIn = vendor_data_in,
  .DataOut = vendor_data_out,
  .GetFSConfigDescriptor = vendor_get_config,
};

USBD_StatusTypeDef USBD_VENDOR_Receive(USBD_HandleTypeDef *pdev, uint8_t *data, uint16_t len)
{
  if(pdev->dev_state != USBD_STATE_CONFIGURED || !pdev->ep_out[1].is_used ||
      data == NULL || len != USBD_VENDOR_MPS) return USBD_FAIL;
  return USBD_LL_PrepareReceive(pdev, USBD_VENDOR_EP_OUT, data, len);
}

USBD_StatusTypeDef USBD_VENDOR_Transmit(USBD_HandleTypeDef *pdev, uint8_t *data, uint16_t len)
{
  if(pdev->dev_state != USBD_STATE_CONFIGURED || !pdev->ep_in[1].is_used ||
      (len && data == NULL)) return USBD_FAIL;
  return USBD_LL_Transmit(pdev, USBD_VENDOR_EP_IN, data, len);
}

__weak void USBD_VENDOR_Opened(USBD_HandleTypeDef *pdev) { (void)pdev; }
__weak void USBD_VENDOR_Closed(USBD_HandleTypeDef *pdev) { (void)pdev; }
__weak void USBD_VENDOR_Received(USBD_HandleTypeDef *pdev, uint32_t len) { (void)pdev; (void)len; }
__weak void USBD_VENDOR_Transmitted(USBD_HandleTypeDef *pdev) { (void)pdev; }
__weak uint8_t USBD_VENDOR_Request(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{ (void)pdev; (void)req; return USBD_FAIL; }
