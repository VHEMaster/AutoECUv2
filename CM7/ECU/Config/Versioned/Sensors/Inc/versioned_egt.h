/*
 * versioned_egt.h
 *
 *  Created on: May 14, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_VERSIONED_SENSORS_INC_VERSIONED_EGT_H_
#define CONFIG_VERSIONED_SENSORS_INC_VERSIONED_EGT_H_

#include "config_gpio.h"
#include "time.h"
#include "compiler.h"
#include "bool.h"
#include "config_devices.h"
#include <stdint.h>

typedef enum {
  EGT_CONFIG_VERSION_V1 = 0,
  EGT_CONFIG_VERSION_MAX
}egt_config_versions_t;

#define EGT_RELATION_ITEMS_MAX    64
#define EGT_RELATION_ITEMS_MIN    8

typedef struct {
    bool enabled;
    time_delta_us_t boot_time;
    ecu_device_tcs_t tcs_instance;

    float temperature_too_high;
    float temperature_too_low;

    uint32_t align ALIGNED_CACHE;
}egt_config_v1_t ALIGNED_CACHE;

typedef egt_config_v1_t egt_config_t;

#endif /* CONFIG_VERSIONED_SENSORS_INC_VERSIONED_EGT_H_ */
