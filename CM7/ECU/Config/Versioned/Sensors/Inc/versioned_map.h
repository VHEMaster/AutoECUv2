/*
 * versioned_map.h
 *
 *  Created on: May 14, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_VERSIONED_SENSORS_INC_VERSIONED_MAP_H_
#define CONFIG_VERSIONED_SENSORS_INC_VERSIONED_MAP_H_

#include "compiler.h"
#include <stdint.h>

typedef enum {
  MAP_CONFIG_VERSION_V1 = 0,
  MAP_CONFIG_VERSION_MAX
}map_config_versions_t;

typedef struct {

    uint32_t align ALIGNED_CACHE;
}map_config_v1_t ALIGNED_CACHE;

typedef map_config_v1_t map_config_t;

#endif /* CONFIG_VERSIONED_SENSORS_INC_VERSIONED_MAP_H_ */
