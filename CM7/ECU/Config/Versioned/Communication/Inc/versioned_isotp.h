/*
 * versioned_isotp.h
 *
 *  Created on: Oct 23, 2025
 *      Author: VHEMaster
 */

#ifndef CONFIG_VERSIONED_COMMUNICATION_INC_VERSIONED_ISOTP_H_
#define CONFIG_VERSIONED_COMMUNICATION_INC_VERSIONED_ISOTP_H_

#include "common.h"
#include "time.h"

typedef enum {
  ISOTP_CONFIG_VERSION_V1 = 0,
  ISOTP_CONFIG_VERSION_MAX
}isotp_config_versions_t;

typedef struct {
    time_us_t timeout;
    uint16_t upstream_block_size;
    time_delta_us_t upstream_min_separation_time;
    time_delta_us_t upstream_separation_time_gap;
    bool downstream_pass_early_ovf;
    uint32_t align ALIGNED_CACHE;
}isotp_config_v1_t ALIGNED_CACHE;

typedef isotp_config_v1_t isotp_config_t;

#endif /* CONFIG_VERSIONED_COMMUNICATION_INC_VERSIONED_ISOTP_H_ */
