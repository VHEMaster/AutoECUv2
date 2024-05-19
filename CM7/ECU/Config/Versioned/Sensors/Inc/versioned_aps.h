/*
 * versioned_aps.h
 *
 *  Created on: May 14, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_VERSIONED_SENSORS_INC_VERSIONED_APS_H_
#define CONFIG_VERSIONED_SENSORS_INC_VERSIONED_APS_H_

#include "config_gpio.h"
#include "compiler.h"
#include <stdint.h>

#define APS_CONFIG_SIGNALS_MAX    (2)

typedef enum {
  APS_CONFIG_VERSION_V1 = 0,
  APS_CONFIG_VERSION_MAX
}aps_config_versions_t;

typedef struct {
    float voltage_pos_min;
    float voltage_pos_max;
    float voltage_low_thr;
    float voltage_high_thr;

    ecu_gpio_input_pin_t input_pin;
}aps_config_signal_t;

typedef struct {
    bool enabled;
    uint8_t signals_used_count;
    aps_config_signal_t signals[APS_CONFIG_SIGNALS_MAX];

    float signals_position_imbalance_max;

    float position_min;
    float position_max;
    float position_min_clamp;
    float position_max_clamp;
    float slew_rate;
    float dead_zone;

    time_delta_us_t boot_time;

    uint32_t align ALIGNED_CACHE;
}aps_config_v1_t ALIGNED_CACHE;

typedef aps_config_v1_t aps_config_t;

#endif /* CONFIG_VERSIONED_SENSORS_INC_VERSIONED_APS_H_ */
