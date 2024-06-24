/*
 * versioned_coolingfan.h
 *
 *  Created on: May 15, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_VERSIONED_MODULES_INC_VERSIONED_COOLINGFAN_H_
#define CONFIG_VERSIONED_MODULES_INC_VERSIONED_COOLINGFAN_H_

#include "common.h"

#include "config_gpio.h"
#include "config_sensors.h"
#include "config_modules_types.h"

typedef enum {
  COOLINGFAN_CONFIG_VERSION_V1 = 0,
  COOLINGFAN_CONFIG_VERSION_MAX
}coolingfan_config_versions_t;

typedef enum {
  COOLINGFAN_CONFIG_CKP_TRIGGER_DETECTED = 0,
  COOLINGFAN_CONFIG_CKP_TRIGGER_SYNCHRONIZED,
  COOLINGFAN_CONFIG_CKP_TRIGGER_MAX,
}coolingfan_config_ckp_trigger_t;

typedef struct {
    bool enabled;

    ecu_sensor_ckp_t sensor_ckp;
    ecu_module_ignpower_t module_ignpower;
    coolingfan_config_ckp_trigger_t ckp_trigger;

    ecu_gpio_output_pin_t output_drive_pin;
    ecu_gpio_input_pin_t input_control_pin;
    ecu_gpio_input_pin_t input_force_pin;
    bool force_gpio_inverted;
    bool control_gpio_inverted;
    bool allow_force_when_stopped;

    float crankshaft_rpm_min;
    float control_voltage_threshold_off;
    float control_voltage_threshold_on;

    time_delta_us_t control_time_delay_off;
    time_delta_us_t control_time_delay_on;
    time_delta_us_t crankshaft_timeout;

    uint32_t align ALIGNED_CACHE;
}coolingfan_config_v1_t ALIGNED_CACHE;

typedef coolingfan_config_v1_t coolingfan_config_t;

#endif /* CONFIG_VERSIONED_MODULES_INC_VERSIONED_COOLINGFAN_H_ */
