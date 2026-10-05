/*
 * versioned_knockwindow.h
 *
 *  Created on: 5 окт. 2026 г.
 *      Author: VHEMaster
 */

#ifndef CONFIG_VERSIONED_TIMINGS_INC_VERSIONED_KNOCKWINDOW_H_
#define CONFIG_VERSIONED_TIMINGS_INC_VERSIONED_KNOCKWINDOW_H_

#include "common.h"

typedef enum {
  KNOCKWINDOW_CONFIG_VERSION_V1 = 0,
  KNOCKWINDOW_CONFIG_VERSION_MAX
}ecu_config_knockwindow_versions_t;

typedef struct {
    bool enabled;

    uint32_t align ALIGNED_CACHE;
}knockwindow_config_v1_t ALIGNED_CACHE;

typedef knockwindow_config_v1_t knockwindow_config_t;




#endif /* CONFIG_VERSIONED_TIMINGS_INC_VERSIONED_KNOCKWINDOW_H_ */
