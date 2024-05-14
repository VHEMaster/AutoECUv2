/*
 * versioned_iat.h
 *
 *  Created on: May 14, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_VERSIONED_SENSORS_INC_VERSIONED_IAT_H_
#define CONFIG_VERSIONED_SENSORS_INC_VERSIONED_IAT_H_

#include "compiler.h"
#include <stdint.h>

typedef enum {
  IAT_CONFIG_VERSION_V1 = 0,
  IAT_CONFIG_VERSION_MAX
}iat_config_versions_t;

typedef struct {

    uint32_t align ALIGNED_CACHE;
}iat_config_v1_t ALIGNED_CACHE;

typedef iat_config_v1_t iat_config_t;

#endif /* CONFIG_VERSIONED_SENSORS_INC_VERSIONED_IAT_H_ */
