/*
 * versioned_lwipdrv.h
 *
 *  Created on: 17 нояб. 2026 г.
 *      Author: VHEMaster
 */

#ifndef CONFIG_VERSIONED_COMMUNICATION_INC_VERSIONED_LWIPDRV_H_
#define CONFIG_VERSIONED_COMMUNICATION_INC_VERSIONED_LWIPDRV_H_

#include "common.h"

typedef enum {
  LWIPDRV_CONFIG_VERSION_V1 = 0,
  LWIPDRV_CONFIG_VERSION_MAX
}lwipdrv_config_versions_t;

typedef struct {
    bool enabled;

    uint32_t align ALIGNED_CACHE;
}lwipdrv_config_v1_t ALIGNED_CACHE;

typedef lwipdrv_config_v1_t lwipdrv_config_t;

#endif /* CONFIG_VERSIONED_COMMUNICATION_INC_VERSIONED_LWIPDRV_H_ */
