/*
 * versioned_obd2.h
 *
 *  Created on: Oct 23, 2025
 *      Author: VHEMaster
 */

#ifndef CONFIG_VERSIONED_COMMUNICATION_INC_VERSIONED_OBD2_H_
#define CONFIG_VERSIONED_COMMUNICATION_INC_VERSIONED_OBD2_H_

#include "common.h"
#include "config_common_types.h"
#include "obd2_types.h"

typedef enum {
  OBD2_CONFIG_VERSION_V1 = 0,
  OBD2_CONFIG_VERSION_MAX
}obd2_config_versions_t;

typedef enum {
  OBD2_CONFIG_MODE_01_LINKAGE_SOURCE_HARD = 0,
  OBD2_CONFIG_MODE_01_LINKAGE_SOURCE_COMMON,
  OBD2_CONFIG_MODE_01_LINKAGE_SOURCE_MAX
}obd2_config_mode_01_linkage_source_t;

typedef struct {
    bool supported;
    obd2_config_mode_01_linkage_source_t source;

    ecu_config_parameter_id_t common_ids[4];
}obd2_config_mode_01_linkage_parameter_t;

typedef struct {
    bool supported;
    obd2_config_mode_01_linkage_parameter_t parameters[OBD2_PID_01_MAX];
}obd2_config_mode_01_linkage_t;

typedef struct {
    bool enabled;

    obd2_config_mode_01_linkage_t mode_01_linkage;

    uint32_t align ALIGNED_CACHE;
}obd2_config_v1_t ALIGNED_CACHE;

typedef obd2_config_v1_t obd2_config_t;

#endif /* CONFIG_VERSIONED_COMMUNICATION_INC_VERSIONED_OBD2_H_ */
