/*
 * versioned_fuelpump.h
 *
 *  Created on: May 15, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_VERSIONED_MODULES_INC_VERSIONED_FUELPUMP_H_
#define CONFIG_VERSIONED_MODULES_INC_VERSIONED_FUELPUMP_H_

#include "common.h"

#include "config_gpio.h"
#include "config_sensors.h"
#include "config_modules_types.h"

typedef enum {
  FUELPUMP_CONFIG_VERSION_V1 = 0,
  FUELPUMP_CONFIG_VERSION_MAX
}fuelpump_config_versions_t;

typedef enum {
  FUELPUMP_CONFIG_TRIGGER_ALWAYS_ON = 0,
  FUELPUMP_CONFIG_TRIGGER_IGNPOWER,
  FUELPUMP_CONFIG_TRIGGER_CRANKSHAFT,
  FUELPUMP_CONFIG_TRIGGER_GPIO_INPUT,
  FUELPUMP_CONFIG_TRIGGER_MANUAL,
  FUELPUMP_CONFIG_TRIGGER_FORCE_ONLY,
  FUELPUMP_CONFIG_TRIGGER_MAX,
}fuelpump_config_trigger_t;

typedef enum {
  FUELPUMP_CONFIG_CKP_TRIGGER_DETECTED = 0,
  FUELPUMP_CONFIG_CKP_TRIGGER_SYNCHRONIZED,
  FUELPUMP_CONFIG_CKP_TRIGGER_MAX,
}fuelpump_config_ckp_trigger_t;

typedef struct {
    bool enabled;

    fuelpump_config_trigger_t trigger_source;

    ecu_sensor_ckp_t sensor_ckp;
    ecu_module_ignpower_t module_ignpower;
    fuelpump_config_ckp_trigger_t ckp_trigger;

    ecu_gpio_output_pin_t output_drive_pin;
    ecu_gpio_input_pin_t input_control_pin;
    ecu_gpio_input_pin_t input_trigger_pin;
    bool trigger_gpio_inverted;
    bool control_gpio_inverted;

    float control_voltage_threshold_off;
    float control_voltage_threshold_on;

    time_delta_us_t control_time_delay_off;
    time_delta_us_t control_time_delay_on;
    time_delta_us_t prepumping_time;
    time_delta_us_t crankshaft_timeout;

    uint32_t align ALIGNED_CACHE;
}fuelpump_config_v1_t ALIGNED_CACHE;

typedef fuelpump_config_v1_t fuelpump_config_t;

#endif /* CONFIG_VERSIONED_MODULES_INC_VERSIONED_FUELPUMP_H_ */
