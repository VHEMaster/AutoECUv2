/*
 * versioned_cylinders.h
 *
 *  Created on: Jun 6, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_VERSIONED_CORE_INC_VERSIONED_CYLINDERS_H_
#define CONFIG_VERSIONED_CORE_INC_VERSIONED_CYLINDERS_H_

#include "common.h"
#include "config_gpio.h"

typedef enum {
  ECU_CONFIG_CYLINDERS_VERSION_V1 = 0,
  ECU_CONFIG_CYLINDERS_VERSION_MAX
}ecu_config_cylinders_versions_t;

typedef struct {
    ecu_bank_t bank;
    float pos_offset;
    ecu_cylinder_t cy_opposed;
}ecu_config_cylinder_t;

typedef struct {
    uint8_t banks_count;
    uint8_t cylinders_count;
    float engine_displacement;
    ecu_config_cylinder_t cylinders[ECU_CYLINDER_MAX];

    float rough_measure_range;

    uint32_t align ALIGNED_CACHE;
}ecu_config_cylinders_v1_t ALIGNED_CACHE;

typedef ecu_config_cylinders_v1_t ecu_config_cylinders_t;

#endif /* CONFIG_VERSIONED_CORE_INC_VERSIONED_CYLINDERS_H_ */
