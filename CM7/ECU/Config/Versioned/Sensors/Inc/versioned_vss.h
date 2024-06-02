/*
 * versioned_vss.h
 *
 *  Created on: Jun 2, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_VERSIONED_SENSORS_INC_VERSIONED_VSS_H_
#define CONFIG_VERSIONED_SENSORS_INC_VERSIONED_VSS_H_

#include "config_gpio.h"
#include "time.h"
#include "compiler.h"
#include "bool.h"
#include <stdint.h>

typedef enum {
  VSS_CONFIG_VERSION_V1 = 0,
  VSS_CONFIG_VERSION_MAX
}vss_config_versions_t;

typedef struct {
    bool enabled;

    time_delta_us_t boot_time;

    ecu_gpio_input_pin_t input_pin;

    float pulses_per_meter;
    float input_correction;
    float speed_max;

    uint32_t align ALIGNED_CACHE;
}vss_config_v1_t ALIGNED_CACHE;

typedef vss_config_v1_t vss_config_t;

#endif /* CONFIG_VERSIONED_SENSORS_INC_VERSIONED_VSS_H_ */
