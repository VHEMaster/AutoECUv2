/*
 * versioned_ignition.h
 *
 *  Created on: Jun 23, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_VERSIONED_MODULES_INC_VERSIONED_IGNITION_H_
#define CONFIG_VERSIONED_MODULES_INC_VERSIONED_IGNITION_H_

#include "common.h"
#include "config_gpio.h"

typedef enum {
  IGNITION_CONFIG_VERSION_V1 = 0,
  IGNITION_CONFIG_VERSION_MAX
}ignition_config_versions_t;

typedef struct {
    bool enabled;

    ecu_gpio_input_pin_t input_pin;

    time_delta_us_t input_debounce_off_to_on;
    time_delta_us_t input_debounce_on_to_off;
    bool input_gpio_inverted;

    uint32_t align ALIGNED_CACHE;
}ignition_config_v1_t ALIGNED_CACHE;

typedef ignition_config_v1_t ignition_config_t;

#endif /* CONFIG_VERSIONED_MODULES_INC_VERSIONED_IGNITION_H_ */
