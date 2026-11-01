/*
 * versioned_ect.h
 *
 *  Created on: May 14, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_VERSIONED_SENSORS_INC_VERSIONED_ECT_H_
#define CONFIG_VERSIONED_SENSORS_INC_VERSIONED_ECT_H_

#include "common.h"
#include "time.h"
#include "config_gpio.h"

typedef enum {
  ECT_CONFIG_VERSION_V1 = 0,
  ECT_CONFIG_VERSION_MAX
}ect_config_versions_t;

#define ECT_RELATION_ITEMS_MAX    64
#define ECT_RELATION_ITEMS_MIN    8

typedef enum {
  ECT_SIGNAL_MODE_VOLTAGE = 0,
  ECT_SIGNAL_MODE_RESISTANCE,
  ECT_SIGNAL_MODE_MAX
}ect_config_signal_mode_t;

typedef enum {
  ECT_CALC_MODE_TABLE_REF_VALUE = 0,
  ECT_CALC_MODE_TABLE_VALUE,
  ECT_CALC_MODE_RESISTANCE_CALCULATED,
  ECT_CALC_MODE_MAX
}ect_config_calc_mode_t;

typedef struct {
    uint32_t items;
    float input[ECT_RELATION_ITEMS_MAX];
    float output[ECT_RELATION_ITEMS_MAX];
}ect_config_relation_t;

typedef struct {
    ect_config_relation_t table;
    float input_low;
    float input_high;
    float input_step;
}ect_config_signal_mode_cfg_t;

typedef struct {
    float calibration_resistance;
    float calibration_temperature;
    float thermistor_beta;
}ect_config_resistance_calc_cfg_t;

typedef struct {
    bool enabled;
    ect_config_signal_mode_t signal_mode;
    ect_config_calc_mode_t calc_mode;
    ect_config_signal_mode_cfg_t signal_voltage_to_value;
    ect_config_signal_mode_cfg_t signal_resistance_to_value;
    ect_config_resistance_calc_cfg_t signal_resistance_calculated;
    float slew_rate;

    time_delta_us_t boot_time;

    ecu_gpio_input_pin_t input_pin;

    uint32_t align ALIGNED_CACHE;
}ect_config_v1_t ALIGNED_CACHE;

typedef ect_config_v1_t ect_config_t;

#endif /* CONFIG_VERSIONED_SENSORS_INC_VERSIONED_ECT_H_ */
