/*
 * versioned_lwip.h
 *
 *  Created on: 17 нояб. 2026 г.
 *      Author: VHEMaster
 */

#ifndef CONFIG_VERSIONED_COMMUNICATION_INC_VERSIONED_LWIP_H_
#define CONFIG_VERSIONED_COMMUNICATION_INC_VERSIONED_LWIP_H_

#include "common.h"

typedef enum {
  LWIP_CONFIG_VERSION_V1 = 0,
  LWIP_CONFIG_VERSION_MAX
}lwip_config_versions_t;

typedef struct {
    bool enabled;

    uint32_t align ALIGNED_CACHE;
}lwip_config_v1_t ALIGNED_CACHE;

typedef lwip_config_v1_t lwip_config_t;

#endif /* CONFIG_VERSIONED_COMMUNICATION_INC_VERSIONED_LWIP_H_ */
