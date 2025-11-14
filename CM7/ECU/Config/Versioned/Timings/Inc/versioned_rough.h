/*
 * versioned_rough.h
 *
 *  Created on: Nov 13, 2025
 *      Author: VHEMaster
 */

#ifndef CONFIG_VERSIONED_CORE_INC_VERSIONED_ROUGH_H_
#define CONFIG_VERSIONED_CORE_INC_VERSIONED_ROUGH_H_

#include "common.h"

typedef enum {
  ROUGH_CONFIG_VERSION_V1 = 0,
  ROUGH_CONFIG_VERSION_MAX
}ecu_config_rough_versions_t;

typedef struct {
    bool enabled;
    float rough_measure_range;

    uint32_t align ALIGNED_CACHE;
}rough_config_v1_t ALIGNED_CACHE;

typedef rough_config_v1_t rough_config_t;

#endif /* CONFIG_VERSIONED_CORE_INC_VERSIONED_ROUGH_H_ */
