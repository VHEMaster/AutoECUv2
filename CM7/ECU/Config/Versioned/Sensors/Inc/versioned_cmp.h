/*
 * versioned_cmp.h
 *
 *  Created on: May 14, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_VERSIONED_SENSORS_INC_VERSIONED_CMP_H_
#define CONFIG_VERSIONED_SENSORS_INC_VERSIONED_CMP_H_

#include "compiler.h"
#include <stdint.h>

typedef enum {
  CMP_CONFIG_VERSION_V1 = 0,
  CMP_CONFIG_VERSION_MAX
}cmp_config_versions_t;

typedef struct {

    uint32_t align ALIGNED_CACHE;
}cmp_config_v1_t ALIGNED_CACHE;

typedef cmp_config_v1_t cmp_config_t;

#endif /* CONFIG_VERSIONED_SENSORS_INC_VERSIONED_CMP_H_ */
