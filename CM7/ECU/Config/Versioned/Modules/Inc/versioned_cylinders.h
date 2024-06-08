/*
 * versioned_cylinders.h
 *
 *  Created on: Jun 6, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_VERSIONED_SENSORS_INC_VERSIONED_CYLINDERS_H_
#define CONFIG_VERSIONED_SENSORS_INC_VERSIONED_CYLINDERS_H_

#include "compiler.h"
#include "bool.h"
#include <stdint.h>

typedef enum {
  CYLINDERS_CONFIG_VERSION_V1 = 0,
  CYLINDERS_CONFIG_VERSION_MAX
}cylinders_config_versions_t;

typedef enum {
  CYLINDERS_CY_1 = 0,
  CYLINDERS_CY_2,
  CYLINDERS_CY_3,
  CYLINDERS_CY_4,
  CYLINDERS_CY_5,
  CYLINDERS_CY_6,
  CYLINDERS_CY_7,
  CYLINDERS_CY_8,
  CYLINDERS_CY_MAX,
}cylinders_t;

typedef struct {
    float pos_offset;
}cylinders_config_cylinder_t;

typedef struct {
    uint8_t cylinders_count;
    cylinders_config_cylinder_t cylinders[CYLINDERS_CY_MAX];

    uint32_t align ALIGNED_CACHE;
}cylinders_config_v1_t ALIGNED_CACHE;

typedef cylinders_config_v1_t cylinders_config_t;

#endif /* CONFIG_VERSIONED_SENSORS_INC_VERSIONED_CYLINDERS_H_ */
