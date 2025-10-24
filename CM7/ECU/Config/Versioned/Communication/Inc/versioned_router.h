/*
 * versioned_router.h
 *
 *  Created on: Oct 23, 2025
 *      Author: VHEMaster
 */

#ifndef CONFIG_VERSIONED_COMMUNICATION_INC_VERSIONED_ROUTER_H_
#define CONFIG_VERSIONED_COMMUNICATION_INC_VERSIONED_ROUTER_H_

#include "common.h"

typedef enum {
  ROUTER_CONFIG_VERSION_V1 = 0,
  ROUTER_CONFIG_VERSION_MAX
}router_config_versions_t;

typedef struct {

    uint32_t align ALIGNED_CACHE;
}router_config_v1_t ALIGNED_CACHE;

typedef router_config_v1_t router_config_t;

#endif /* CONFIG_VERSIONED_COMMUNICATION_INC_VERSIONED_ROUTER_H_ */
