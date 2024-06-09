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
  CYLINDERS_CONFIG_VERSION_V1 = 0,
  CYLINDERS_CONFIG_VERSION_MAX
}ecu_cylinders_config_versions_t;

typedef struct {
    ecu_bank_t bank;
    float pos_offset;

    float ignition_advance_offset;
    float injection_phase_offset;
    float injection_pulse_gain;

    ecu_gpio_output_pin_t out_ignition;
    ecu_gpio_output_pin_t out_injection;
}ecu_cylinders_config_cylinder_t;

typedef struct {
    uint8_t cylinders_count;
    ecu_cylinders_config_cylinder_t cylinders[ECU_CYLINDER_MAX];

    uint32_t align ALIGNED_CACHE;
}ecu_cylinders_config_v1_t ALIGNED_CACHE;

typedef ecu_cylinders_config_v1_t ecu_cylinders_config_t;

#endif /* CONFIG_VERSIONED_CORE_INC_VERSIONED_CYLINDERS_H_ */
