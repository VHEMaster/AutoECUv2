/*
 * versioned_etc.h
 *
 *  Created on: May 15, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_VERSIONED_MODULES_INC_VERSIONED_ETC_H_
#define CONFIG_VERSIONED_MODULES_INC_VERSIONED_ETC_H_

#include "common.h"

#include "pid.h"
#include "config_sensors.h"
#include "config_devices.h"

typedef enum {
  ETC_CONFIG_VERSION_V1 = 0,
  ETC_CONFIG_VERSION_MAX
}etc_config_versions_t;

#define ETC_ADAPTATION_TPS_AVERAGE_DEFAULT      (8)
#define ETC_ADAPTATION_TPS_AVERAGE_MAX          (32)

typedef enum {
  ETC_ADAPTATION_POINT_LOW_1 = 0,
  ETC_ADAPTATION_POINT_LOW_2,
  ETC_ADAPTATION_POINT_DEFAULT_LOWER,
  ETC_ADAPTATION_POINT_DEFAULT_UPPER,
  ETC_ADAPTATION_POINT_MIDDLE_1,
  ETC_ADAPTATION_POINT_MIDDLE_2,
  ETC_ADAPTATION_POINT_HIGH_1,
  ETC_ADAPTATION_POINT_HIGH_2,
  ETC_ADAPTATION_POINTS
}etc_adaptation_point_id_t;

typedef struct {
    float position_min;
    float position_max;
    float dutycycle_max;
    uint8_t tps_average_samples;
    bool disable_pid_tuning; /* Skip PID trials and add only mechanical compensation to legacy control. */
    bool disable_model; /* Use legacy control in normal operation. */
}etc_adaptation_setup_t;

typedef struct {
    float position;
    float spring;
    float friction;
}etc_adaptation_point_t;

typedef struct {
    float opening; /* Signed motor effort at first opening motion after a closing approach. */
    float closing; /* Signed motor effort at first closing motion after an opening approach. */
}etc_adaptation_reversal_t;

typedef struct {
    bool valid;
    etc_adaptation_point_t points[ETC_ADAPTATION_POINTS];
    math_pid_koffs_t pid_position;
    math_pid_koffs_t pid_speed;
    float spring_scale_lower;
    float spring_scale_upper;
    float friction_scale_lower;
    float friction_scale_upper;
    uint8_t tps_average_samples;
    bool reversal_valid;
    etc_adaptation_reversal_t reversal[ETC_ADAPTATION_POINTS];
}etc_adaptation_config_t;

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

    etc_adaptation_setup_t adaptation_setup;
    etc_adaptation_config_t adaptation;

    uint32_t align ALIGNED_CACHE;
}etc_config_v1_t ALIGNED_CACHE;

typedef etc_config_v1_t etc_config_t;

#endif /* CONFIG_VERSIONED_MODULES_INC_VERSIONED_ETC_H_ */
