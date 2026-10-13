/*
 * versioned_signals.h
 *
 *  Created on: 13 окт. 2026 г.
 *      Author: VHEMaster
 */

#ifndef CONFIG_VERSIONED_COMMUNICATION_INC_VERSIONED_SIGNALS_H_
#define CONFIG_VERSIONED_COMMUNICATION_INC_VERSIONED_SIGNALS_H_

#include "common.h"
#include "config_comm_types.h"

typedef enum {
  SIGNALS_CONFIG_VERSION_V1 = 0,
  SIGNALS_CONFIG_VERSION_MAX
}signals_config_versions_t;

typedef struct {
    bool enabled;

    uint32_t align ALIGNED_CACHE;
}signals_config_v1_t ALIGNED_CACHE;

typedef signals_config_v1_t signals_config_t;

#endif /* CONFIG_VERSIONED_COMMUNICATION_INC_VERSIONED_SIGNALS_H_ */
