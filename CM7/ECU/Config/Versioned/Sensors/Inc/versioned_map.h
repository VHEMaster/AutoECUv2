/*
 * versioned_map.h
 *
 *  Created on: May 14, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_VERSIONED_SENSORS_INC_VERSIONED_MAP_H_
#define CONFIG_VERSIONED_SENSORS_INC_VERSIONED_MAP_H_

#include "config_gpio.h"
#include "time.h"
#include "compiler.h"
#include "bool.h"
#include <stdint.h>

typedef enum {
  MAP_CONFIG_VERSION_V1 = 0,
  MAP_CONFIG_VERSION_MAX
}map_config_versions_t;

#define MAP_RELATION_ITEMS_MAX    64
#define MAP_RELATION_ITEMS_MIN    8

typedef enum {
  MAP_SIGNAL_MODE_ANALOG = 0,
  MAP_SIGNAL_MODE_FREQUENCY,
  MAP_SIGNAL_MODE_MAX
}map_config_signal_mode_t;

typedef enum {
  MAP_CALC_MODE_TABLE_REF_VALUE = 0,
  MAP_CALC_MODE_TABLE_VALUE,
  MAP_CALC_MODE_OFFSET_GAIN,
  MAP_CALC_MODE_MAX
}map_config_calc_mode_t;

typedef struct {
    uint32_t items;
    float input[MAP_RELATION_ITEMS_MAX];
    float output[MAP_RELATION_ITEMS_MAX];
}map_config_relation_t;

typedef struct {
    map_config_relation_t table;
    float input_low;
    float input_high;
    float input_step;
    float gain;
    float offset;
}map_config_signal_mode_cfg_t;

typedef struct {
    bool enabled;
    map_config_signal_mode_t signal_mode;
    map_config_calc_mode_t calc_mode;
    map_config_signal_mode_cfg_t signal_voltage_to_value;
    map_config_signal_mode_cfg_t signal_frequency_to_value;

    time_delta_us_t boot_time;

    ecu_gpio_input_pin_t input_pin;

    uint32_t align ALIGNED_CACHE;
}map_config_v1_t ALIGNED_CACHE;

typedef map_config_v1_t map_config_t;

#endif /* CONFIG_VERSIONED_SENSORS_INC_VERSIONED_MAP_H_ */
