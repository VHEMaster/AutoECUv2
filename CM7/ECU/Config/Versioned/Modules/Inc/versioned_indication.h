/*
 * versioned_indication.h
 *
 *  Created on: Jun 24, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_VERSIONED_MODULES_INC_VERSIONED_INDICATION_H_
#define CONFIG_VERSIONED_MODULES_INC_VERSIONED_INDICATION_H_

#include "common.h"
#include "config_gpio.h"
#include "config_sensors.h"
#include "config_modules_types.h"

typedef enum {
  INDICATION_CONFIG_VERSION_V1 = 0,
  INDICATION_CONFIG_VERSION_MAX
}indication_config_versions_t;

typedef enum {
  INDICATION_CONFIG_TRIGGER_MANUAL_ONLY = 0,
  INDICATION_CONFIG_TRIGGER_CRANKSHAFT,
  INDICATION_CONFIG_TRIGGER_MAX
}indication_config_trigger_t;

typedef struct {
    bool enabled;

    indication_config_trigger_t trigger_source;

    ecu_sensor_ckp_t sensor_ckp;
    ecu_module_ignpower_t module_ignpower;
    ecu_gpio_output_pin_t output_pin;
    bool ignpower_off_disables;

    float ckp_rpm_min;
    float ckp_rpm_max;

    uint32_t align ALIGNED_CACHE;
}indication_config_v1_t ALIGNED_CACHE;

typedef indication_config_v1_t indication_config_t;

#endif /* CONFIG_VERSIONED_MODULES_INC_VERSIONED_INDICATION_H_ */
