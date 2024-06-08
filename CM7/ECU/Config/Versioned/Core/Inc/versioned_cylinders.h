/*
 * versioned_cylinders.h
 *
 *  Created on: Jun 6, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_VERSIONED_SENSORS_INC_VERSIONED_CYLINDERS_H_
#define CONFIG_VERSIONED_SENSORS_INC_VERSIONED_CYLINDERS_H_

#include "common.h"

typedef enum {
  CYLINDERS_CONFIG_VERSION_V1 = 0,
  CYLINDERS_CONFIG_VERSION_MAX
}ecu_cylinders_config_versions_t;

typedef struct {
    float pos_offset;
}ecu_cylinders_config_cylinder_t;

typedef struct {
    uint8_t cylinders_count;
    ecu_cylinders_config_cylinder_t cylinders[ECU_CYLINDER_MAX];

    uint32_t align ALIGNED_CACHE;
}ecu_cylinders_config_v1_t ALIGNED_CACHE;

typedef ecu_cylinders_config_v1_t ecu_cylinders_config_t;

#endif /* CONFIG_VERSIONED_SENSORS_INC_VERSIONED_CYLINDERS_H_ */
