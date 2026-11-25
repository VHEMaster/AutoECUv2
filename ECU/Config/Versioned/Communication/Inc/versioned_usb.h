#ifndef CONFIG_VERSIONED_COMMUNICATION_INC_VERSIONED_USB_H_
#define CONFIG_VERSIONED_COMMUNICATION_INC_VERSIONED_USB_H_

#include "common.h"

typedef enum {
  USB_CONFIG_VERSION_V1 = 0,
  USB_CONFIG_VERSION_MAX
}usb_config_versions_t;

typedef struct {
    bool enabled;
    uint16_t vid;
    uint16_t pid;

    uint32_t align ALIGNED_CACHE;
}usb_config_v1_t ALIGNED_CACHE;

typedef usb_config_v1_t usb_config_t;

#endif /* CONFIG_VERSIONED_COMMUNICATION_INC_VERSIONED_USB_H_ */
