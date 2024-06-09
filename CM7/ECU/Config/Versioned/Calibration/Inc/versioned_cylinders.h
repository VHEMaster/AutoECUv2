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

    float ignition_advance_offset;
    float injection_phase_offset;
    float injection_pulse_gain;

    ecu_gpio_output_pin_t out_ignition;
    ecu_gpio_output_pin_t out_injection;
}ecu_config_cylinder_t;

typedef struct {
    uint8_t cylinders_count;
    ecu_config_cylinder_t cylinders[ECU_CYLINDER_MAX];

    uint32_t align ALIGNED_CACHE;
}ecu_config_cylinders_v1_t ALIGNED_CACHE;

typedef ecu_config_cylinders_v1_t ecu_config_cylinders_t;

#endif /* CONFIG_VERSIONED_CORE_INC_VERSIONED_CYLINDERS_H_ */
