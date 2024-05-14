/*
 * versioned_ect.h
 *
 *  Created on: May 14, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_VERSIONED_SENSORS_INC_VERSIONED_ECT_H_
#define CONFIG_VERSIONED_SENSORS_INC_VERSIONED_ECT_H_

#include "compiler.h"
#include <stdint.h>

typedef enum {
  ECT_CONFIG_VERSION_V1 = 0,
  ECT_CONFIG_VERSION_MAX
}ect_config_versions_t;

typedef struct {

    uint32_t align ALIGNED_CACHE;
}ect_config_v1_t ALIGNED_CACHE;

typedef ect_config_v1_t ect_config_t;

#endif /* CONFIG_VERSIONED_SENSORS_INC_VERSIONED_ECT_H_ */
