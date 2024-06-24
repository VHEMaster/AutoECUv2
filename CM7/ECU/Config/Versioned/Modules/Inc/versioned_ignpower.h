/*
 * versioned_ignpower.h
 *
 *  Created on: Jun 23, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_VERSIONED_MODULES_INC_VERSIONED_IGNPOWER_H_
#define CONFIG_VERSIONED_MODULES_INC_VERSIONED_IGNPOWER_H_

#include "common.h"
#include "config_gpio.h"
#include "config_sensors.h"

typedef enum {
  IGNPOWER_CONFIG_VERSION_V1 = 0,
  IGNPOWER_CONFIG_VERSION_MAX
}ignpower_config_versions_t;

typedef struct {
    bool enabled;

    ecu_sensor_ckp_t sensor_ckp;

    ecu_gpio_input_pin_t input_signal_pin;
    ecu_gpio_output_pin_t output_relay_pin;

    time_delta_us_t input_debounce_off_to_on;
    time_delta_us_t input_debounce_on_to_off;

    bool components_operating_trigger;
    bool crankshaft_operating_trigger;

    time_delta_us_t output_shutdown_min_delay;
    time_delta_us_t output_shutdown_operation_delay;
    time_delta_us_t output_shutdown_crankshaft_delay;

    bool input_gpio_inverted;
    bool output_gpio_inverted;

    uint32_t align ALIGNED_CACHE;
}ignpower_config_v1_t ALIGNED_CACHE;

typedef ignpower_config_v1_t ignpower_config_t;

#endif /* CONFIG_VERSIONED_MODULES_INC_VERSIONED_IGNPOWER_H_ */
