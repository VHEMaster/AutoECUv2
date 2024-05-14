/*
 * versioned_tps.h
 *
 *  Created on: May 14, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_VERSIONED_SENSORS_INC_VERSIONED_TPS_H_
#define CONFIG_VERSIONED_SENSORS_INC_VERSIONED_TPS_H_

#include "compiler.h"
#include <stdint.h>

typedef enum {
  TPS_CONFIG_VERSION_V1 = 0,
  TPS_CONFIG_VERSION_MAX
}tps_config_versions_t;

typedef struct {

    uint32_t align ALIGNED_CACHE;
}tps_config_v1_t ALIGNED_CACHE;

typedef tps_config_v1_t tps_config_t;

#endif /* CONFIG_VERSIONED_SENSORS_INC_VERSIONED_TPS_H_ */
