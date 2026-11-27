/*
 * versioned_tunerstudio.h
 *
 *  Created on: Nov 26, 2026
 *      Author: VHEMaster
 */

#ifndef CONFIG_VERSIONED_COMMUNICATION_INC_VERSIONED_TUNERSTUDIO_H_
#define CONFIG_VERSIONED_COMMUNICATION_INC_VERSIONED_TUNERSTUDIO_H_

#include "common.h"
#include "config_comm_types.h"

typedef enum {
  TUNERSTUDIO_CONFIG_VERSION_V1 = 0,
  TUNERSTUDIO_CONFIG_VERSION_MAX
}tunerstudio_config_versions_t;

typedef struct {
    bool enabled;
    bool lwip_enabled;
    bool rs422_enabled;
    uint16_t port;
    ecu_comm_rs422_t rs422_instance;

    uint32_t align ALIGNED_CACHE;
}tunerstudio_config_v1_t ALIGNED_CACHE;

typedef tunerstudio_config_v1_t tunerstudio_config_t;

#endif /* CONFIG_VERSIONED_COMMUNICATION_INC_VERSIONED_TUNERSTUDIO_H_ */
