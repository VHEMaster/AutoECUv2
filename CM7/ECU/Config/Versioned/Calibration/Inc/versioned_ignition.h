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
  ECU_CONFIG_IGNITION_VERSION_V1 = 0,
  ECU_CONFIG_IGNITION_VERSION_MAX
}ecu_config_ignition_versions_t;

typedef enum {
  ECU_CONFIG_IGNITION_GROUP_PRIMARY = 0,
  ECU_CONFIG_IGNITION_GROUP_SECONDARY,
  ECU_CONFIG_IGNITION_GROUP_TERTINARY,
  ECU_CONFIG_IGNITION_GROUP_MAX,
}ecu_config_ignition_group_t;

typedef enum {
  ECU_CONFIG_IGNITION_GROUP_MODE_SEQUENTIAL_ONLY = 0,
  ECU_CONFIG_IGNITION_GROUP_MODE_SEMISEQUENTIAL_ONLY,
  ECU_CONFIG_IGNITION_GROUP_MODE_SEQUENTIAL_AND_SEMISEQUENTIAL,
  ECU_CONFIG_IGNITION_GROUP_MODE_DISTRIBUTOR,
  ECU_CONFIG_IGNITION_GROUP_MODE_MAX,
}ecu_config_ignition_group_mode_t;

typedef struct {
    uint32_t items;
    float input[IGNITION_RELATION_ITEMS_MAX];
    float output[IGNITION_RELATION_ITEMS_MAX];
}ecu_config_ignition_relation_t;

typedef struct {
    bool disabled;
    ecu_gpio_output_pin_t output_pin;
    float advance_add;

}ecu_config_ignition_group_cylinder_setup_t;

typedef struct {
    bool enabled;
    ecu_config_ignition_group_mode_t mode;
    ecu_config_ignition_group_cylinder_setup_t cylinders[ECU_CYLINDER_MAX];

    float advance_add;
    ecu_config_ignition_relation_t voltage_to_saturation_time;
    ecu_config_ignition_relation_t rpm_to_saturation_mult;
    float distributor_dutycycle;

}ecu_config_ignition_group_setup_t;

typedef struct {
    ecu_gpio_input_pin_t power_voltage_pin;
    ecu_config_ignition_group_setup_t groups[ECU_CONFIG_IGNITION_GROUP_MAX];

    uint32_t align ALIGNED_CACHE;
}ecu_config_ignition_v1_t ALIGNED_CACHE;

typedef ecu_config_ignition_v1_t ecu_config_ignition_t;

#endif /* CONFIG_VERSIONED_CORE_INC_VERSIONED_IGNITION_H_ */
