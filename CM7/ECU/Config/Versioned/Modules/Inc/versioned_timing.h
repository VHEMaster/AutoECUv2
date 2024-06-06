/*
 * versioned_timing.h
 *
 *  Created on: Jun 6, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_VERSIONED_SENSORS_INC_VERSIONED_TIMING_H_
#define CONFIG_VERSIONED_SENSORS_INC_VERSIONED_TIMING_H_

#include "compiler.h"
#include "bool.h"
#include <stdint.h>

#include "config_sensors.h"

typedef enum {
  TIMING_CONFIG_VERSION_V1 = 0,
  TIMING_CONFIG_VERSION_MAX
}timing_config_versions_t;

typedef struct {
    bool enabled;
    ecu_sensor_cmp_t cmp_instance;
    float pos_relative;
    float pos_min;
    float pos_max;
}timing_config_cmp_t;

typedef struct {
    ecu_sensor_ckp_t ckp_instance;
    timing_config_cmp_t cmp_config[ECU_SENSOR_CMP_MAX];

    uint32_t align ALIGNED_CACHE;
}timing_config_v1_t ALIGNED_CACHE;

typedef timing_config_v1_t timing_config_t;

#endif /* CONFIG_VERSIONED_SENSORS_INC_VERSIONED_TIMING_H_ */
