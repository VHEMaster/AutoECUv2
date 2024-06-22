/*
 * versioned_timing.h
 *
 *  Created on: Jun 6, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_VERSIONED_MODULES_INC_VERSIONED_TIMING_H_
#define CONFIG_VERSIONED_MODULES_INC_VERSIONED_TIMING_H_

#include "common.h"

#include "config_sensors.h"

typedef enum {
  TIMING_CONFIG_VERSION_V1 = 0,
  TIMING_CONFIG_VERSION_MAX
}timing_config_versions_t;

typedef enum {
  TIMING_CONFIG_PHASEDSYNCMODE_DISABLED,
  TIMING_CONFIG_PHASEDSYNCMODE_CAMSHAFT,
  TIMING_CONFIG_PHASEDSYNCMODE_ROUGHTEST,
  TIMING_CONFIG_PHASEDSYNCMODE_MAX
}timing_config_phasedsyncmode_t;

typedef struct {
    float offset;
}timing_config_crankshaft_t;

typedef struct {
    bool enabled;
    bool use_for_phased_sync;
    ecu_sensor_cmp_t cmp_instance;
    float pos_relative;
    float pos_min;
    float pos_max;
}timing_config_camshaft_t;

typedef struct {
    timing_config_crankshaft_t crankshaft;
    timing_config_camshaft_t camshafts[ECU_SENSOR_CMP_MAX];

    timing_config_phasedsyncmode_t phased_syncmode;
    bool phased_only;

    uint32_t align ALIGNED_CACHE;
}timing_config_v1_t ALIGNED_CACHE;

typedef timing_config_v1_t timing_config_t;

#endif /* CONFIG_VERSIONED_MODULES_INC_VERSIONED_TIMING_H_ */
