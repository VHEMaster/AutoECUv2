/*
 * versioned_ignition.h
 *
 *  Created on: Jul 7, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_VERSIONED_CORE_INC_VERSIONED_IGNITION_H_
#define CONFIG_VERSIONED_CORE_INC_VERSIONED_IGNITION_H_

#include "common.h"
#include "config_gpio.h"
#include "versioned_cylinders.h"

#define IGNITION_RELATION_ITEMS_MIN 4
#define IGNITION_RELATION_ITEMS_MAX 16

typedef enum {
  IGNITION_CONFIG_VERSION_V1 = 0,
  IGNITION_CONFIG_VERSION_MAX
}ignition_config_versions_t;

typedef enum {
  IGNITION_CONFIG_PROCESS_UPDATE_TRIGGER_ALWAYS = 0,
  IGNITION_CONFIG_PROCESS_UPDATE_TRIGGER_1OF2_1ST,
  IGNITION_CONFIG_PROCESS_UPDATE_TRIGGER_1OF2_2ND,
  IGNITION_CONFIG_PROCESS_UPDATE_TRIGGER_MAX,
}ignition_config_process_update_trigger_t;

typedef enum {
  IGNITION_CONFIG_GROUP_PRIMARY = 0,
  IGNITION_CONFIG_GROUP_SECONDARY,
  IGNITION_CONFIG_GROUP_TERTINARY,
  IGNITION_CONFIG_GROUP_MAX,
}ignition_config_group_t;

typedef enum {
  IGNITION_CONFIG_GROUP_PROCESS_UPDATE_TRIGGER_ALWAYS = 0,
  IGNITION_CONFIG_GROUP_PROCESS_UPDATE_TRIGGER_1OF3_1ST,
  IGNITION_CONFIG_GROUP_PROCESS_UPDATE_TRIGGER_1OF3_2ND,
  IGNITION_CONFIG_GROUP_PROCESS_UPDATE_TRIGGER_1OF3_3RD,
  IGNITION_CONFIG_GROUP_PROCESS_UPDATE_TRIGGER_MAX,
}ignition_config_group_process_update_trigger_t;

typedef enum {
  IGNITION_CONFIG_GROUP_MODE_SEQUENTIAL_ONLY = 0,
  IGNITION_CONFIG_GROUP_MODE_SEMISEQUENTIAL_ONLY,
  IGNITION_CONFIG_GROUP_MODE_SEQUENTIAL_AND_SEMISEQUENTIAL,
  IGNITION_CONFIG_GROUP_MODE_DISTRIBUTOR,
  IGNITION_CONFIG_GROUP_MODE_MAX,
}ignition_config_group_mode_t;

typedef enum {
  IGNITION_CONFIG_GROUP_ADVANCE_SLEW_RATE_MODE_BY_SECOND = 0,
  IGNITION_CONFIG_GROUP_ADVANCE_SLEW_RATE_MODE_BY_REVOLUTION,
  IGNITION_CONFIG_GROUP_ADVANCE_SLEW_RATE_MODE_MAX,
}ignition_config_group_advance_slew_rate_mode_t;

typedef enum {
  IGNITION_CONFIG_USPD_SOURCE_PER_SENSOR_PULSE = 0,
  IGNITION_CONFIG_USPD_SOURCE_PER_REVOLUTION,
  IGNITION_CONFIG_USPD_SOURCE_MAX,
}ignition_config_uspd_source_t;

typedef struct {
    uint32_t items;
    float input[IGNITION_RELATION_ITEMS_MAX];
    float output[IGNITION_RELATION_ITEMS_MAX];
}ignition_config_relation_t;

typedef struct {
    bool disabled;
    ecu_gpio_output_pin_t output_pin;
    float advance_add;

}ignition_config_group_cylinder_setup_t;

typedef struct {
    bool enabled;
    ignition_config_group_process_update_trigger_t process_update_trigger;
    ignition_config_group_mode_t mode;
    ignition_config_group_cylinder_setup_t cylinders[ECU_CYLINDER_MAX];

    float advance_add;
    ignition_config_group_advance_slew_rate_mode_t advance_slew_rate_mode;
    float advance_slew_rate_earlier;
    float advance_slew_rate_later;
    ignition_config_relation_t voltage_to_saturation_time;
    ignition_config_relation_t rpm_to_saturation_mult;
    float distributor_dutycycle;

}ignition_config_group_setup_t;

typedef struct {
    bool enabled;
    float signal_prepare_advance;
    ignition_config_process_update_trigger_t process_update_trigger;
    ignition_config_uspd_source_t uspd_source;
    ecu_gpio_input_pin_t power_voltage_pin;
    bool use_ignition_acceptance;
    ignition_config_group_setup_t groups[IGNITION_CONFIG_GROUP_MAX];

    uint32_t align ALIGNED_CACHE;
}ignition_config_v1_t ALIGNED_CACHE;

typedef ignition_config_v1_t ignition_config_t;

#endif /* CONFIG_VERSIONED_CORE_INC_VERSIONED_IGNITION_H_ */
