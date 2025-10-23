/*
 * versioned_obd2.h
 *
 *  Created on: Oct 23, 2025
 *      Author: VHEMaster
 */

#ifndef CONFIG_VERSIONED_COMMUNICATION_INC_VERSIONED_OBD2_H_
#define CONFIG_VERSIONED_COMMUNICATION_INC_VERSIONED_OBD2_H_

#include "common.h"

typedef enum {
  OBD2_CONFIG_VERSION_V1 = 0,
  OBD2_CONFIG_VERSION_MAX
}obd2_config_versions_t;

typedef struct {

    uint32_t align ALIGNED_CACHE;
}obd2_config_v1_t ALIGNED_CACHE;

typedef obd2_config_v1_t obd2_config_t;

#endif /* CONFIG_VERSIONED_COMMUNICATION_INC_VERSIONED_OBD2_H_ */
