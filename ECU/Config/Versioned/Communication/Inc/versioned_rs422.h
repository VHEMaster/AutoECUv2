/*
 * versioned_rs422.h
 *
 *  Created on: 16 нояб. 2026 г.
 *      Author: VHEMaster
 */

#ifndef CONFIG_VERSIONED_COMMUNICATION_INC_VERSIONED_RS422_H_
#define CONFIG_VERSIONED_COMMUNICATION_INC_VERSIONED_RS422_H_

#include "common.h"

typedef enum {
  RS422_CONFIG_VERSION_V1 = 0,
  RS422_CONFIG_VERSION_MAX
}rs422_config_versions_t;

typedef struct {
    bool enabled;
    uint32_t baudrate;

    uint32_t align ALIGNED_CACHE;
}rs422_config_v1_t ALIGNED_CACHE;

typedef rs422_config_v1_t rs422_config_t;

#endif /* CONFIG_VERSIONED_COMMUNICATION_INC_VERSIONED_RS422_H_ */
