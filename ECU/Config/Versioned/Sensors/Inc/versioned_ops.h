/*
 * versioned_ops.h
 *
 *  Created on: Jun 24, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_VERSIONED_SENSORS_INC_VERSIONED_OPS_H_
#define CONFIG_VERSIONED_SENSORS_INC_VERSIONED_OPS_H_

#include "common.h"
#include "time.h"
#include "config_gpio.h"

typedef enum {
  OPS_CONFIG_VERSION_V1 = 0,
  OPS_CONFIG_VERSION_MAX
}ops_config_versions_t;

#define OPS_RELATION_ITEMS_MAX    20
#define OPS_RELATION_ITEMS_MIN    4

typedef enum {
  OPS_SIGNAL_MODE_VOLTAGE = 0,
  OPS_SIGNAL_MODE_RESISTANCE,
  OPS_SIGNAL_MODE_MAX
}ops_config_signal_mode_t;

typedef enum {
  OPS_CALC_MODE_TABLE_REF_VALUE = 0,
  OPS_CALC_MODE_TABLE_VALUE,
  OPS_CALC_MODE_MAX
}ops_config_calc_mode_t;

typedef struct {
    uint32_t items;
    float input[OPS_RELATION_ITEMS_MAX];
    float output[OPS_RELATION_ITEMS_MAX];
}ops_config_relation_t;

typedef struct {
    ops_config_relation_t table;
    float input_low;
    float input_high;
    float input_step;
}ops_config_signal_mode_cfg_t;

typedef struct {
    bool enabled;
    ops_config_signal_mode_t signal_mode;
    ops_config_calc_mode_t calc_mode;
    ops_config_signal_mode_cfg_t signal_voltage_to_value;
    ops_config_signal_mode_cfg_t signal_resistance_to_value;
    float slew_rate;

    time_delta_us_t boot_time;

    ecu_gpio_input_pin_t input_pin;

    uint32_t align ALIGNED_CACHE;
}ops_config_v1_t ALIGNED_CACHE;

typedef ops_config_v1_t ops_config_t;

#endif /* CONFIG_VERSIONED_SENSORS_INC_VERSIONED_OPS_H_ */
