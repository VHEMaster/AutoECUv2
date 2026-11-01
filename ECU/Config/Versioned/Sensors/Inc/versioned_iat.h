/*
 * versioned_iat.h
 *
 *  Created on: May 14, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_VERSIONED_SENSORS_INC_VERSIONED_IAT_H_
#define CONFIG_VERSIONED_SENSORS_INC_VERSIONED_IAT_H_

#include "common.h"
#include "time.h"
#include "config_gpio.h"

typedef enum {
  IAT_CONFIG_VERSION_V1 = 0,
  IAT_CONFIG_VERSION_MAX
}iat_config_versions_t;

#define IAT_RELATION_ITEMS_MAX    64
#define IAT_RELATION_ITEMS_MIN    8

typedef enum {
  IAT_SIGNAL_MODE_VOLTAGE = 0,
  IAT_SIGNAL_MODE_RESISTANCE,
  IAT_SIGNAL_MODE_MAX
}iat_config_signal_mode_t;

typedef enum {
  IAT_CALC_MODE_TABLE_REF_VALUE = 0,
  IAT_CALC_MODE_TABLE_VALUE,
  IAT_CALC_MODE_RESISTANCE_CALCULATED,
  IAT_CALC_MODE_MAX
}iat_config_calc_mode_t;

typedef struct {
    uint32_t items;
    float input[IAT_RELATION_ITEMS_MAX];
    float output[IAT_RELATION_ITEMS_MAX];
}iat_config_relation_t;

typedef struct {
    iat_config_relation_t table;
    float input_low;
    float input_high;
    float input_step;
}iat_config_signal_mode_cfg_t;

typedef struct {
    float calibration_resistance;
    float calibration_temperature;
    float thermistor_beta;
}iat_config_resistance_calc_cfg_t;

typedef struct {
    bool enabled;
    iat_config_signal_mode_t signal_mode;
    iat_config_calc_mode_t calc_mode;
    iat_config_signal_mode_cfg_t signal_voltage_to_value;
    iat_config_signal_mode_cfg_t signal_resistance_to_value;
    iat_config_resistance_calc_cfg_t signal_resistance_calculated;
    float slew_rate;

    time_delta_us_t boot_time;

    ecu_gpio_input_pin_t input_pin;

    uint32_t align ALIGNED_CACHE;
}iat_config_v1_t ALIGNED_CACHE;

typedef iat_config_v1_t iat_config_t;

#endif /* CONFIG_VERSIONED_SENSORS_INC_VERSIONED_IAT_H_ */
