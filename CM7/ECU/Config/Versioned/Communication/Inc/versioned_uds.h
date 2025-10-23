/*
 * versioned_uds.h
 *
 *  Created on: Oct 23, 2025
 *      Author: VHEMaster
 */

#ifndef CONFIG_VERSIONED_COMMUNICATION_INC_VERSIONED_UDS_H_
#define CONFIG_VERSIONED_COMMUNICATION_INC_VERSIONED_UDS_H_

#include "common.h"

typedef enum {
  UDS_CONFIG_VERSION_V1 = 0,
  UDS_CONFIG_VERSION_MAX
}uds_config_versions_t;

typedef struct {

    uint32_t align ALIGNED_CACHE;
}uds_config_v1_t ALIGNED_CACHE;

typedef uds_config_v1_t uds_config_t;

#endif /* CONFIG_VERSIONED_COMMUNICATION_INC_VERSIONED_UDS_H_ */
