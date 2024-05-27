/*
 * versioned_cmp.h
 *
 *  Created on: May 14, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_VERSIONED_SENSORS_INC_VERSIONED_CMP_H_
#define CONFIG_VERSIONED_SENSORS_INC_VERSIONED_CMP_H_

#include "config_gpio.h"
#include "time.h"
#include "compiler.h"
#include "bool.h"
#include <stdint.h>

typedef enum {
  CMP_CONFIG_VERSION_V1 = 0,
  CMP_CONFIG_VERSION_MAX
}cmp_config_versions_t;

typedef enum {
  CMP_CONFIG_SIGNAL_REF_TYPE_SINGLEPULSE = 0,
  CMP_CONFIG_SIGNAL_REF_TYPE_MAX,
}cmp_config_signal_ref_type_t;

typedef struct {

}cmp_config_signal_ref_type_singlepulse_t;

typedef struct {
    cmp_config_signal_ref_type_singlepulse_t singlepulse;
}cmp_config_signal_ref_types_config_t;

typedef struct {
    bool enabled;

    time_delta_us_t boot_time;
    bool desync_on_error;

    ecu_gpio_input_pin_t input_pin;

    cmp_config_signal_ref_type_t signal_ref_type;
    cmp_config_signal_ref_types_config_t signal_ref_types_config;

    uint32_t align ALIGNED_CACHE;
}cmp_config_v1_t ALIGNED_CACHE;

typedef cmp_config_v1_t cmp_config_t;

#endif /* CONFIG_VERSIONED_SENSORS_INC_VERSIONED_CMP_H_ */
