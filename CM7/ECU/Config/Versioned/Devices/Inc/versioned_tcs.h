/*
 * versioned_tcs.h
 *
 *  Created on: Oct 2, 2026
 *      Author: VHEMaster
 */

#ifndef CONFIG_PROJECT_INC_VERSIONED_TCS_H_
#define CONFIG_PROJECT_INC_VERSIONED_TCS_H_

#include "bool.h"
#include "compiler.h"

typedef enum {
  TCS_CONFIG_VERSION_V1 = 0,
  TCS_CONFIG_VERSION_MAX
}tcs_config_versions_t;

typedef struct {
    bool enabled;
    time_delta_us_t poll_period;

    uint32_t align ALIGNED_CACHE;
}max31855_config_v1_t ALIGNED_CACHE;

typedef max31855_config_v1_t max31855_config_t;

#endif /* CONFIG_PROJECT_INC_VERSIONED_TCS_H_ */
