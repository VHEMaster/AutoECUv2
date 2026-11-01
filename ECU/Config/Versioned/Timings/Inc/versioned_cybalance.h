/*
 * versioned_cybalance.h
 *
 *  Created on: Nov 13, 2025
 *      Author: VHEMaster
 */

#ifndef CONFIG_VERSIONED_CORE_INC_VERSIONED_CYBALANCE_H_
#define CONFIG_VERSIONED_CORE_INC_VERSIONED_CYBALANCE_H_

#include "common.h"

typedef enum {
  CYBALANCE_CONFIG_VERSION_V1 = 0,
  CYBALANCE_CONFIG_VERSION_MAX
}ecu_config_cybalance_versions_t;

typedef struct {
    bool enabled;
    float measure_startpoint;
    float measure_midpoint;
    float measure_endpoint;

    uint32_t align ALIGNED_CACHE;
}cybalance_config_v1_t ALIGNED_CACHE;

typedef cybalance_config_v1_t cybalance_config_t;

#endif /* CONFIG_VERSIONED_CORE_INC_VERSIONED_CYBALANCE_H_ */
