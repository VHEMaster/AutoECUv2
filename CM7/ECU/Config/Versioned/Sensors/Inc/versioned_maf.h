/*
 * versioned_maf.h
 *
 *  Created on: May 14, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_VERSIONED_SENSORS_INC_VERSIONED_MAF_H_
#define CONFIG_VERSIONED_SENSORS_INC_VERSIONED_MAF_H_

#include "compiler.h"
#include <stdint.h>

typedef enum {
  MAF_CONFIG_VERSION_V1 = 0,
  MAF_CONFIG_VERSION_MAX
}maf_config_versions_t;

typedef struct {

    uint32_t align ALIGNED_CACHE;
}maf_config_v1_t ALIGNED_CACHE;

typedef maf_config_v1_t maf_config_t;

#endif /* CONFIG_VERSIONED_SENSORS_INC_VERSIONED_MAF_H_ */
