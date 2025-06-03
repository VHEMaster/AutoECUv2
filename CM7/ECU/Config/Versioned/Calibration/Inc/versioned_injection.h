/*
 * versioned_injection.h
 *
 *  Created on: Jul 7, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_VERSIONED_CORE_INC_VERSIONED_INJECTION_H_
#define CONFIG_VERSIONED_CORE_INC_VERSIONED_INJECTION_H_

#include "common.h"
#include "config_gpio.h"
#include "versioned_cylinders.h"

#define INJECTION_RELATION_ITEMS_MIN 4
#define INJECTION_RELATION_ITEMS_MAX 16

typedef enum {
  ECU_CONFIG_INJECTION_VERSION_V1 = 0,
  ECU_CONFIG_INJECTION_VERSION_MAX
}ecu_config_injection_versions_t;

typedef enum {
  ECU_CONFIG_INJECTION_PROCESS_UPDATE_TRIGGER_ALWAYS = 0,
  ECU_CONFIG_INJECTION_PROCESS_UPDATE_TRIGGER_1OF2_1ST,
  ECU_CONFIG_INJECTION_PROCESS_UPDATE_TRIGGER_1OF2_2ND,
  ECU_CONFIG_INJECTION_PROCESS_UPDATE_TRIGGER_MAX,
}ecu_config_injection_process_update_trigger_t;

typedef enum {
  ECU_CONFIG_INJECTION_GROUP_PRIMARY = 0,
  ECU_CONFIG_INJECTION_GROUP_MAX,
}ecu_config_injection_group_t;

typedef enum {
  ECU_CONFIG_INJECTION_GROUP_PROCESS_UPDATE_TRIGGER_ALWAYS = 0,
  ECU_CONFIG_INJECTION_GROUP_PROCESS_UPDATE_TRIGGER_1OF2_1ST,
  ECU_CONFIG_INJECTION_GROUP_PROCESS_UPDATE_TRIGGER_1OF2_2ND,
  ECU_CONFIG_INJECTION_GROUP_PROCESS_UPDATE_TRIGGER_MAX,
}ecu_config_injection_group_process_update_trigger_t;

typedef enum {
  ECU_CONFIG_INJECTION_GROUP_PHASE_MODE_BEGINNING = 0,
  ECU_CONFIG_INJECTION_GROUP_PHASE_MODE_ENDING,
  ECU_CONFIG_INJECTION_GROUP_PHASE_MODE_MAX,
}ecu_config_injection_group_phase_mode_t;

typedef enum {
  ECU_CONFIG_INJECTION_GROUP_LATE_PHASE_MODE_DISABLED = 0,
  ECU_CONFIG_INJECTION_GROUP_LATE_PHASE_MODE_BEGINNING,
  ECU_CONFIG_INJECTION_GROUP_LATE_PHASE_MODE_ENDING,
  ECU_CONFIG_INJECTION_GROUP_LATE_PHASE_MODE_MAX,
}ecu_config_injection_group_late_phase_mode_t;

typedef enum {
  ECU_CONFIG_INJECTION_GROUP_MODE_SEQUENTIAL_ONLY = 0,
  ECU_CONFIG_INJECTION_GROUP_MODE_SEMISEQUENTIAL_ONLY,
  ECU_CONFIG_INJECTION_GROUP_MODE_SEQUENTIAL_AND_SEMISEQUENTIAL,
  ECU_CONFIG_INJECTION_GROUP_MODE_MONO,
  ECU_CONFIG_INJECTION_GROUP_MODE_MAX,
}ecu_config_injection_group_mode_t;

typedef enum {
  ECU_CONFIG_INJECTION_USPD_SOURCE_PER_SENSOR_PULSE = 0,
  ECU_CONFIG_INJECTION_USPD_SOURCE_PER_REVOLUTION,
  ECU_CONFIG_INJECTION_USPD_SOURCE_MAX,
}ecu_config_injection_uspd_source_t;

typedef enum {
  ECU_CONFIG_INJECTION_GROUP_DUTYCYCLE_LIMIT_MODE_CUTOFF = 0,
  ECU_CONFIG_INJECTION_GROUP_DUTYCYCLE_LIMIT_MODE_CLAMP,
  ECU_CONFIG_INJECTION_GROUP_DUTYCYCLE_LIMIT_MODE_MAX,
}ecu_config_injection_dutycycle_limit_mode_t;

typedef enum {
  ECU_CONFIG_INJECTION_GROUP_PERF_UNITS_CC = 0,
  ECU_CONFIG_INJECTION_GROUP_PERF_UNITS_G,
  ECU_CONFIG_INJECTION_GROUP_PERF_UNITS_MAX,
}ecu_config_injection_perf_units_t;

typedef enum {
  ECU_CONFIG_INJECTION_GROUP_PERF_PRESSURE_SOURCE_NONE = 0,
  ECU_CONFIG_INJECTION_GROUP_PERF_PRESSURE_SOURCE_RELATIVE_MAP,
  ECU_CONFIG_INJECTION_GROUP_PERF_PRESSURE_SOURCE_MAX
}ecu_config_injection_perf_press_source_t;

typedef struct {
    uint32_t items;
    float input[INJECTION_RELATION_ITEMS_MAX];
    float output[INJECTION_RELATION_ITEMS_MAX];
}ecu_config_injection_relation_t;

typedef struct {
    bool disabled;
    ecu_gpio_output_pin_t output_pin;

    float performance_static_mul;
    float performance_static_add;

    float performance_dynamic_mul;
    float performance_dynamic_add;

    float phase_add;
}ecu_config_injection_group_cylinder_setup_t;

typedef struct {
    bool enabled;
    ecu_config_injection_group_process_update_trigger_t process_update_trigger;
    ecu_config_injection_group_mode_t mode;
    ecu_config_injection_group_cylinder_setup_t cylinders[ECU_CYLINDER_MAX];

    ecu_config_injection_relation_t voltage_to_performance_dynamic;
    float performance_static;

    float performance_static_semiseq_mul;
    float performance_static_semiseq_add;
    float performance_static_seq_mul;
    float performance_static_seq_add;

    ecu_config_injection_perf_units_t performance_static_units;
    float performance_fuel_mass_per_cc;

    ecu_config_injection_perf_press_source_t performance_fuel_pressure_source;
    ecu_config_io_map_t performance_fuel_pressure_map_type;
    float performance_fuel_pressure;

    float phase_add;
    float phase_slew_rate;
    ecu_config_injection_group_phase_mode_t phase_mode;
    ecu_config_injection_relation_t rpm_to_phase_add;

    ecu_config_injection_group_late_phase_mode_t enrichment_late_phase_mode;
    ecu_config_injection_relation_t enrichment_late_phase;

    float inject_mass_low_threshold;
    float inject_mass_reduction;
    float dutycycle_warning;
    float dutycycle_limit;
    ecu_config_injection_dutycycle_limit_mode_t dutycycle_limit_mode;
}ecu_config_injection_group_setup_t;

typedef struct {
    float signal_prepare_advance;
    ecu_config_injection_process_update_trigger_t process_update_trigger;
    ecu_config_injection_uspd_source_t uspd_source;
    ecu_gpio_input_pin_t power_voltage_pin;
    ecu_config_injection_group_setup_t groups[ECU_CONFIG_INJECTION_GROUP_MAX];

    uint32_t align ALIGNED_CACHE;
}ecu_config_injection_v1_t ALIGNED_CACHE;

typedef ecu_config_injection_v1_t ecu_config_injection_t;

#endif /* CONFIG_VERSIONED_CORE_INC_VERSIONED_INJECTION_H_ */
