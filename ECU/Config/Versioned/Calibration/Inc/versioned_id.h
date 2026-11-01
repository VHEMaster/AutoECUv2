/*
 * versioned_id.h
 *
 *  Created on: May 3, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_VERSIONED_SOFTWARE_INC_VERSIONED_ID_H_
#define CONFIG_VERSIONED_SOFTWARE_INC_VERSIONED_ID_H_

#include "common.h"

typedef enum {
  ECU_CONFIG_DATA_ID_VERSION_V1 = 0,
  ECU_CONFIG_DATA_ID_VERSION_MAX
}ecu_config_data_identification_versions_t;

typedef struct {
    uint32_t reserved[128];
    uint32_t align ALIGNED_CACHE;
}ecu_config_data_identification_v1_t ALIGNED_CACHE;

typedef ecu_config_data_identification_v1_t ecu_config_data_identification_t;

#endif /* CONFIG_VERSIONED_SOFTWARE_INC_VERSIONED_ID_H_ */
