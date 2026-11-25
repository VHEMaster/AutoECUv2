#include "usbd_vendor.h"
#include "usbd_vendor_requests.h"
#include "usbd_ioreq.h"

/* Set header (10) + Compatible ID (20) + Registry Property (132). */
#define USBD_MS_OS_20_DESC_SIZE 162u

static uint8_t bos_desc[] = {
  0x05,                                                             /* bLength: BOS header size in bytes. */
  0x0f,                                                             /* bDescriptorType: BOS. */
  0x21, 0x00,                                                       /* wTotalLength: 5-byte header + 28-byte capability. */
  0x01,                                                             /* bNumDeviceCaps: one capability follows. */

  0x1c,                                                             /* bLength: platform capability size in bytes. */
  0x10,                                                             /* bDescriptorType: Device Capability. */
  0x05,                                                             /* bDevCapabilityType: Platform. */
  0x00,                                                             /* bReserved: must be zero. */
  0xdf, 0x60, 0xdd, 0xd8, 0x89, 0x45, 0xc7, 0x4c,                   /* PlatformCapabilityUUID: MS OS 2.0 UUID, first 8 bytes. */
  0x9c, 0xd2, 0x65, 0x9d, 0x9e, 0x64, 0x8a, 0x9f,                   /* PlatformCapabilityUUID: remaining 8 bytes. */
  0x00, 0x00, 0x03, 0x06,                                           /* dwWindowsVersion: 0x06030000 (Windows 8.1). */
  LOBYTE(USBD_MS_OS_20_DESC_SIZE), HIBYTE(USBD_MS_OS_20_DESC_SIZE), /* wMSOSDescriptorSetTotalLength: descriptor set size. */
  USBD_VENDOR_MS_OS_20,                                             /* bMS_VendorCode: bRequest used to fetch the set. */
  0x00                                                              /* bAltEnumCode: alternate enumeration is unsupported. */
};

/* WinUSB descriptors are at device scope for the single-interface device. */
static uint8_t ms_os_20_desc[] = {
  0x0a, 0x00,                                                       /* wLength: 10-byte descriptor set header. */
  0x00, 0x00,                                                       /* wDescriptorType: MS OS 2.0 Set Header. */
  0x00, 0x00, 0x03, 0x06,                                           /* dwWindowsVersion: must match the BOS capability. */
  LOBYTE(USBD_MS_OS_20_DESC_SIZE), HIBYTE(USBD_MS_OS_20_DESC_SIZE), /* wTotalLength: header and both feature descriptors. */

  0x14, 0x00,                                                       /* wLength: 20-byte Compatible ID feature descriptor. */
  0x03, 0x00,                                                       /* wDescriptorType: Compatible ID. */
  0x57, 0x49, 0x4e, 0x55, 0x53, 0x42, 0x00, 0x00,                   /* CompatibleID: "WINUSB" in ASCII, padded to 8 bytes. */
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,                   /* SubCompatibleID: unused 8-byte field. */

  0x84, 0x00,                                                       /* wLength: 10 bytes of fields + name and data byte counts. */
  0x04, 0x00,                                                       /* wDescriptorType: Registry Property. */
  0x07, 0x00,                                                       /* wPropertyDataType: REG_MULTI_SZ (UTF-16LE string list). */
  0x2a, 0x00,                                                       /* wPropertyNameLength: 42 bytes, including the NUL. */
  0x44, 0x00, 0x65, 0x00, 0x76, 0x00, 0x69, 0x00,                   /* PropertyName: "Devi" in UTF-16LE. */
  0x63, 0x00, 0x65, 0x00,                                           /* PropertyName: "ce" in UTF-16LE. */
  0x49, 0x00, 0x6e, 0x00, 0x74, 0x00, 0x65, 0x00, 0x72, 0x00,       /* PropertyName: "Inter" in UTF-16LE. */
  0x66, 0x00, 0x61, 0x00, 0x63, 0x00, 0x65, 0x00,                   /* PropertyName: "face" in UTF-16LE. */
  0x47, 0x00, 0x55, 0x00, 0x49, 0x00, 0x44, 0x00, 0x73, 0x00,       /* PropertyName: "GUIDs" in UTF-16LE. */
  0x00, 0x00,                                                       /* PropertyName: UTF-16LE NUL terminator. */
  0x50, 0x00,                                                       /* wPropertyDataLength: 80 bytes, including both NULs. */
  0x7b, 0x00, 0x44, 0x00, 0x39, 0x00, 0x38, 0x00, 0x46, 0x00,       /* PropertyData: "{D98F" in UTF-16LE. */
  0x38, 0x00, 0x42, 0x00, 0x37, 0x00, 0x44, 0x00,                   /* PropertyData: "8B7D" in UTF-16LE. */
  0x2d, 0x00, 0x33, 0x00, 0x36, 0x00, 0x33, 0x00, 0x38, 0x00,       /* PropertyData: "-3638" in UTF-16LE. */
  0x2d, 0x00, 0x34, 0x00, 0x41, 0x00, 0x36, 0x00, 0x35, 0x00,       /* PropertyData: "-4A65" in UTF-16LE. */
  0x2d, 0x00, 0x39, 0x00, 0x38, 0x00, 0x35, 0x00, 0x42, 0x00,       /* PropertyData: "-985B" in UTF-16LE. */
  0x2d, 0x00, 0x36, 0x00, 0x32, 0x00, 0x44, 0x00, 0x34, 0x00,       /* PropertyData: "-62D4" in UTF-16LE. */
  0x43, 0x00, 0x33, 0x00,                                           /* PropertyData: "C3" in UTF-16LE. */
  0x31, 0x00, 0x37, 0x00, 0x36, 0x00, 0x46, 0x00,                   /* PropertyData: "176F" in UTF-16LE. */
  0x32, 0x00, 0x41, 0x00, 0x7d, 0x00,                               /* PropertyData: "2A}" in UTF-16LE. */
  0x00, 0x00,                                                       /* PropertyData: NUL terminates the GUID string. */
  0x00, 0x00                                                        /* PropertyData: second NUL terminates the string list. */
};

_Static_assert(sizeof(ms_os_20_desc) == USBD_MS_OS_20_DESC_SIZE,
    "MS OS 2.0 descriptor set size mismatch");

/* A single atomic snapshot is shared between the CM4 main loop and USB IRQ. */
static uint32_t link_status;
static uint8_t requests;
static uint8_t response[4];

uint8_t *USBD_VENDOR_BOS(USBD_SpeedTypeDef speed, uint16_t *length)
{
  (void)speed;
  *length = sizeof(bos_desc);
  return bos_desc;
}

void USBD_VENDOR_SetLinkStatus(uint8_t owner, uint8_t link_up, uint8_t configured)
{
  __atomic_store_n(&link_status, (uint32_t)owner | ((uint32_t)link_up << 8u) |
      ((uint32_t)configured << 16u), __ATOMIC_RELEASE);
}

uint8_t USBD_VENDOR_TakeRequests(void)
{
  return __atomic_exchange_n(&requests, 0u, __ATOMIC_ACQ_REL);
}

static uint8_t send_response(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req, uint8_t *data, uint16_t len)
{
  if(req->wLength == 0u) return USBD_FAIL;
  return USBD_CtlSendData(pdev, data, req->wLength < len ? req->wLength : len);
}

uint8_t USBD_VENDOR_Request(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{
  uint32_t status = __atomic_load_n(&link_status, __ATOMIC_ACQUIRE);
  if(req->bmRequest == 0xC0u && req->bRequest == USBD_VENDOR_MS_OS_20 &&
      req->wIndex == 7u && req->wValue == 0u)
    return send_response(pdev, req, ms_os_20_desc, sizeof(ms_os_20_desc));

  if(req->wIndex != 0u || req->wValue != 0u || pdev->dev_state != USBD_STATE_CONFIGURED)
    return USBD_FAIL;
  if(req->bmRequest == 0xC0u) {
    switch(req->bRequest) {
      case USBD_VENDOR_GET_DEVICE_INFO:
        response[0] = 1u; /* protocol version */
        response[1] = USBD_VENDOR_MPS;
        response[2] = USBD_VENDOR_EP_IN;
        response[3] = USBD_VENDOR_EP_OUT;
        return send_response(pdev, req, response, 4u);
      case USBD_VENDOR_GET_LINK_STATUS:
        response[0] = (uint8_t)(status >> 8u);
        response[1] = (uint8_t)status;
        response[2] = (uint8_t)(status >> 16u);
        return send_response(pdev, req, response, 3u);
      case USBD_VENDOR_GET_ACTIVE_TRANSPORT:
        response[0] = (uint8_t)status;
        return send_response(pdev, req, response, 1u);
      default: return USBD_FAIL;
    }
  }
  if(req->bmRequest == 0x40u && req->wLength == 0u) {
    if(req->bRequest == USBD_VENDOR_REQUEST_CONNECTION) {
      /* This is a readiness check, not ownership acquisition before link-up. */
      if(((status >> 16u) & 1u) && (!((status >> 8u) & 1u) || (uint8_t)status == 2u)) {
        return USBD_CtlSendStatus(pdev);
      }
    } else if(req->bRequest == USBD_VENDOR_RELEASE_CONNECTION && (uint8_t)status == 2u) {
      __atomic_fetch_or(&requests, 1u, __ATOMIC_RELEASE);
      return USBD_CtlSendStatus(pdev);
    }
  }
  return USBD_FAIL;
}
