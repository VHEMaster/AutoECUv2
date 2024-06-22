/*
 * versioned_vvt.h
 *
 *  Created on: May 15, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_VERSIONED_MODULES_INC_VERSIONED_VVT_H_
#define CONFIG_VERSIONED_MODULES_INC_VERSIONED_VVT_H_

#include "common.h"

#include "pid.h"
#include "config_sensors.h"
#include "config_devices.h"

typedef enum {
  VVT_CONFIG_VERSION_V1 = 0,
  VVT_CONFIG_VERSION_MAX
}vvt_config_versions_t;

typedef struct {
    bool enabled;
    ecu_gpio_input_pin_t power_voltage_pin;
    ecu_sensor_cmp_t sensor_cmp;
    ecu_gpio_output_pin_t output_pwm_pin;
    uint16_t pwm_freq;

    float pwm_dutycycle_min;
    float pwm_dutycycle_max;

    math_pid_koffs_t pid_position;
    math_pid_koffs_t pid_speed;

    float pos_reach_threshold;
    time_delta_us_t pos_reach_timeout;

    uint32_t align ALIGNED_CACHE;
}vvt_config_v1_t ALIGNED_CACHE;

typedef vvt_config_v1_t vvt_config_t;

#endif /* CONFIG_VERSIONED_MODULES_INC_VERSIONED_VVT_H_ */
