/*
 * versioned_etc.h
 *
 *  Created on: May 15, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_VERSIONED_SENSORS_INC_VERSIONED_ETC_H_
#define CONFIG_VERSIONED_SENSORS_INC_VERSIONED_ETC_H_

#include "compiler.h"
#include "bool.h"
#include <stdint.h>

#define ETC_CONFIG_SIGNALS_MAX    (2)

typedef enum {
  ETC_CONFIG_VERSION_V1 = 0,
  ETC_CONFIG_VERSION_MAX
}etc_config_versions_t;

typedef struct {
    bool enabled;

    uint32_t align ALIGNED_CACHE;
}etc_config_v1_t ALIGNED_CACHE;

typedef etc_config_v1_t etc_config_t;

#endif /* CONFIG_VERSIONED_SENSORS_INC_VERSIONED_ETC_H_ */
