/*
 * versioned_maf.h
 *
 *  Created on: May 14, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_VERSIONED_SENSORS_INC_VERSIONED_MAF_H_
#define CONFIG_VERSIONED_SENSORS_INC_VERSIONED_MAF_H_

#include "config_gpio.h"
#include "time.h"
#include "compiler.h"
#include "bool.h"
#include <stdint.h>

typedef enum {
  MAF_CONFIG_VERSION_V1 = 0,
  MAF_CONFIG_VERSION_MAX
}maf_config_versions_t;

#define MAF_RELATION_ITEMS_MAX    256

typedef enum {
  MAF_SIGNAL_MODE_ANALOG = 0,
  MAF_SIGNAL_MODE_FREQUENCY,
  MAF_SIGNAL_MODE_MAX
}maf_config_signal_mode_t;

typedef enum {
  MAF_CALC_MODE_TABLE_REF_VALUE = 0,
  MAF_CALC_MODE_TABLE_VALUE,
  MAF_CALC_MODE_OFFSET_GAIN,
  MAF_CALC_MODE_MAX
}maf_config_calc_mode_t;

typedef struct {
    uint32_t items;
    float input[MAF_RELATION_ITEMS_MAX];
    float output[MAF_RELATION_ITEMS_MAX];
}maf_config_relation_t;

typedef struct {
    maf_config_relation_t table;
    float input_low;
    float input_high;
    float input_step;
    float gain;
    float offset;
}maf_config_signal_mode_cfg_t;

typedef struct {
    bool enabled;
    maf_config_signal_mode_t signal_mode;
    maf_config_calc_mode_t calc_mode;
    maf_config_signal_mode_cfg_t signal_voltage_to_value;
    maf_config_signal_mode_cfg_t signal_frequency_to_value;

    time_delta_us_t boot_time;

    ecu_gpio_input_pin_t input_pin;

    uint32_t align ALIGNED_CACHE;
}maf_config_v1_t ALIGNED_CACHE;

typedef maf_config_v1_t maf_config_t;

#endif /* CONFIG_VERSIONED_SENSORS_INC_VERSIONED_MAF_H_ */
