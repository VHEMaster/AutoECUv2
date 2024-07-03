/*
 * versioned_wgcv.h
 *
 *  Created on: May 15, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_VERSIONED_MODULES_INC_VERSIONED_WGCV_H_
#define CONFIG_VERSIONED_MODULES_INC_VERSIONED_WGCV_H_

#include "common.h"

#include "pid.h"
#include "config_sensors.h"
#include "config_devices.h"

typedef enum {
  WGCV_CONFIG_VERSION_V1 = 0,
  WGCV_CONFIG_VERSION_MAX
}wgcv_config_versions_t;

#define WGCV_RELATION_ITEMS_MIN    4
#define WGCV_RELATION_ITEMS_MAX    16

typedef struct {
    uint32_t items;
    float input[WGCV_RELATION_ITEMS_MAX];
    float output[WGCV_RELATION_ITEMS_MAX];
}wgcv_config_relation_t;

typedef struct {
    wgcv_config_relation_t full_closed;
    wgcv_config_relation_t full_open;
}wgcv_config_voltage_to_dutycycle_t;

typedef struct {
    bool enabled;
    ecu_gpio_input_pin_t power_voltage_pin;
    ecu_sensor_map_t sensor_map;
    ecu_gpio_output_pin_t output_pwm_pin;
    uint16_t pwm_freq;

    float sensor_map_atmospheric_pressure;

    wgcv_config_voltage_to_dutycycle_t voltage_to_pwm_dutycycle;
    float pwm_dutycycle_min;
    float pwm_dutycycle_max;

    float input_dutycycle_min;
    float input_dutycycle_max;

    math_pid_koffs_t pid_boost;
    math_pid_koffs_t pid_speed;

    uint32_t align ALIGNED_CACHE;
}wgcv_config_v1_t ALIGNED_CACHE;

typedef wgcv_config_v1_t wgcv_config_t;

#endif /* CONFIG_VERSIONED_MODULES_INC_VERSIONED_WGCV_H_ */
