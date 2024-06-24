/*
 * versioned_ots.h
 *
 *  Created on: Jun 24, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_VERSIONED_SENSORS_INC_VERSIONED_OTS_H_
#define CONFIG_VERSIONED_SENSORS_INC_VERSIONED_OTS_H_

#include "common.h"
#include "time.h"
#include "config_gpio.h"

typedef enum {
  OTS_CONFIG_VERSION_V1 = 0,
  OTS_CONFIG_VERSION_MAX
}ots_config_versions_t;

#define OTS_RELATION_ITEMS_MAX    64
#define OTS_RELATION_ITEMS_MIN    8

typedef enum {
  OTS_SIGNAL_MODE_VOLTAGE = 0,
  OTS_SIGNAL_MODE_RESISTANCE,
  OTS_SIGNAL_MODE_MAX
}ots_config_signal_mode_t;

typedef enum {
  OTS_CALC_MODE_TABLE_REF_VALUE = 0,
  OTS_CALC_MODE_TABLE_VALUE,
  OTS_CALC_MODE_RESISTANCE_CALCULATED,
  OTS_CALC_MODE_MAX
}ots_config_calc_mode_t;

typedef struct {
    uint32_t items;
    float input[OTS_RELATION_ITEMS_MAX];
    float output[OTS_RELATION_ITEMS_MAX];
}ots_config_relation_t;

typedef struct {
    ots_config_relation_t table;
    float input_low;
    float input_high;
    float input_step;
}ots_config_signal_mode_cfg_t;

typedef struct {
    float calibration_resistance;
    float calibration_temperature;
    float thermistor_beta;
}ots_config_resistance_calc_cfg_t;

typedef struct {
    bool enabled;
    ots_config_signal_mode_t signal_mode;
    ots_config_calc_mode_t calc_mode;
    ots_config_signal_mode_cfg_t signal_voltage_to_value;
    ots_config_signal_mode_cfg_t signal_resistance_to_value;
    ots_config_resistance_calc_cfg_t signal_resistance_calculated;
    float slew_rate;

    time_delta_us_t boot_time;

    ecu_gpio_input_pin_t input_pin;

    uint32_t align ALIGNED_CACHE;
}ots_config_v1_t ALIGNED_CACHE;

typedef ots_config_v1_t ots_config_t;

#endif /* CONFIG_VERSIONED_SENSORS_INC_VERSIONED_OTS_H_ */
