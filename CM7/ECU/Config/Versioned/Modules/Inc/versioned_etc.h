/*
 * versioned_etc.h
 *
 *  Created on: May 15, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_VERSIONED_SENSORS_INC_VERSIONED_ETC_H_
#define CONFIG_VERSIONED_SENSORS_INC_VERSIONED_ETC_H_

#include "compiler.h"
#include "bool.h"
#include <stdint.h>

#include "pid.h"
#include "config_sensors.h"
#include "config_devices.h"

#define ETC_CONFIG_SIGNALS_MAX    (2)

typedef enum {
  ETC_CONFIG_VERSION_V1 = 0,
  ETC_CONFIG_VERSION_MAX
}etc_config_versions_t;

typedef struct {
    bool enabled;
    ecu_gpio_input_pin_t power_voltage_pin;
    ecu_sensor_tps_t sensor_tps;
    ecu_device_motor_t device_motor;
    uint16_t pwm_freq;

    math_pid_koffs_t pid_position;
    math_pid_koffs_t pid_speed;

    float motor_reach_threshold;
    time_delta_us_t motor_reach_timeout;

    uint32_t align ALIGNED_CACHE;
}etc_config_v1_t ALIGNED_CACHE;

typedef etc_config_v1_t etc_config_t;

#endif /* CONFIG_VERSIONED_SENSORS_INC_VERSIONED_ETC_H_ */
