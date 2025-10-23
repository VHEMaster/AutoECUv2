/*
 * versioned_kwp.h
 *
 *  Created on: Oct 23, 2025
 *      Author: VHEMaster
 */

#ifndef CONFIG_VERSIONED_COMMUNICATION_INC_VERSIONED_KWP_H_
#define CONFIG_VERSIONED_COMMUNICATION_INC_VERSIONED_KWP_H_

#include "common.h"

typedef enum {
  KWP_CONFIG_VERSION_V1 = 0,
  KWP_CONFIG_VERSION_MAX
}kwp_config_versions_t;

typedef struct {

    uint32_t align ALIGNED_CACHE;
}kwp_config_v1_t ALIGNED_CACHE;

typedef kwp_config_v1_t kwp_config_t;

#endif /* CONFIG_VERSIONED_COMMUNICATION_INC_VERSIONED_KWP_H_ */
