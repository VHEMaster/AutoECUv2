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
#include "versioned_io.h"

#define INJECTION_RELATION_ITEMS_MIN 4
#define INJECTION_RELATION_ITEMS_MAX 16

typedef enum {
  INJECTION_CONFIG_VERSION_V1 = 0,
  INJECTION_CONFIG_VERSION_MAX
}injection_config_versions_t;

typedef enum {
  INJECTION_CONFIG_PROCESS_UPDATE_TRIGGER_ALWAYS = 0,
  INJECTION_CONFIG_PROCESS_UPDATE_TRIGGER_1OF2_1ST,
  INJECTION_CONFIG_PROCESS_UPDATE_TRIGGER_1OF2_2ND,
  INJECTION_CONFIG_PROCESS_UPDATE_TRIGGER_MAX,
}injection_config_process_update_trigger_t;

typedef enum {
  INJECTION_CONFIG_GROUP_PRIMARY = 0,
  INJECTION_CONFIG_GROUP_MAX,
}injection_config_group_t;

typedef enum {
  INJECTION_CONFIG_GROUP_PROCESS_UPDATE_TRIGGER_ALWAYS = 0,
  INJECTION_CONFIG_GROUP_PROCESS_UPDATE_TRIGGER_1OF2_1ST,
  INJECTION_CONFIG_GROUP_PROCESS_UPDATE_TRIGGER_1OF2_2ND,
  INJECTION_CONFIG_GROUP_PROCESS_UPDATE_TRIGGER_MAX,
}injection_config_group_process_update_trigger_t;

typedef enum {
  INJECTION_CONFIG_GROUP_PHASE_MODE_BEGINNING = 0,
  INJECTION_CONFIG_GROUP_PHASE_MODE_ENDING,
  INJECTION_CONFIG_GROUP_PHASE_MODE_MAX,
}injection_config_group_phase_mode_t;

typedef enum {
  INJECTION_CONFIG_GROUP_LATE_PHASE_MODE_DISABLED = 0,
  INJECTION_CONFIG_GROUP_LATE_PHASE_MODE_BEGINNING,
  INJECTION_CONFIG_GROUP_LATE_PHASE_MODE_ENDING,
  INJECTION_CONFIG_GROUP_LATE_PHASE_MODE_MAX,
}injection_config_group_late_phase_mode_t;

typedef enum {
  INJECTION_CONFIG_GROUP_MODE_SEQUENTIAL_ONLY = 0,
  INJECTION_CONFIG_GROUP_MODE_SEMISEQUENTIAL_ONLY,
  INJECTION_CONFIG_GROUP_MODE_SEQUENTIAL_AND_SEMISEQUENTIAL,
  INJECTION_CONFIG_GROUP_MODE_MONO,
  INJECTION_CONFIG_GROUP_MODE_MAX,
}injection_config_group_mode_t;

typedef enum {
  INJECTION_CONFIG_USPD_SOURCE_PER_SENSOR_PULSE = 0,
  INJECTION_CONFIG_USPD_SOURCE_PER_REVOLUTION,
  INJECTION_CONFIG_USPD_SOURCE_MAX,
}injection_config_uspd_source_t;

typedef enum {
  INJECTION_CONFIG_GROUP_DUTYCYCLE_LIMIT_MODE_CUTOFF = 0,
  INJECTION_CONFIG_GROUP_DUTYCYCLE_LIMIT_MODE_CLAMP,
  INJECTION_CONFIG_GROUP_DUTYCYCLE_LIMIT_MODE_MAX,
}injection_config_dutycycle_limit_mode_t;

typedef enum {
  INJECTION_CONFIG_GROUP_PERF_UNITS_CC = 0,
  INJECTION_CONFIG_GROUP_PERF_UNITS_G,
  INJECTION_CONFIG_GROUP_PERF_UNITS_MAX,
}injection_config_perf_units_t;

typedef enum {
  INJECTION_CONFIG_GROUP_PERF_PRESSURE_SOURCE_NONE = 0,
  INJECTION_CONFIG_GROUP_PERF_PRESSURE_SOURCE_RELATIVE_MAP,
  INJECTION_CONFIG_GROUP_PERF_PRESSURE_SOURCE_MAX
}injection_config_perf_press_source_t;

typedef struct {
    uint32_t items;
    float input[INJECTION_RELATION_ITEMS_MAX];
    float output[INJECTION_RELATION_ITEMS_MAX];
}injection_config_relation_t;

typedef struct {
    bool disabled;
    ecu_gpio_output_pin_t output_pin;

    float performance_static_mul;
    float performance_static_add;

    float performance_dynamic_mul;
    float performance_dynamic_add;

    float phase_add;
}injection_config_group_cylinder_setup_t;

typedef struct {
    bool enabled;
    injection_config_group_process_update_trigger_t process_update_trigger;
    injection_config_group_mode_t mode;
    injection_config_group_cylinder_setup_t cylinders[ECU_CYLINDER_MAX];

    injection_config_relation_t voltage_to_performance_dynamic;
    float performance_static;
    float performance_static_fuel_pressure;

    float performance_static_semiseq_mul;
    float performance_static_semiseq_add;
    float performance_static_seq_mul;
    float performance_static_seq_add;

    injection_config_perf_units_t performance_static_units;
    float performance_fuel_mass_per_cc;

    float performance_fuelramp_nominal_pressure;
    injection_config_perf_press_source_t performance_fuel_pressure_manifold_source;
    ecu_config_io_map_t performance_fuel_pressure_manifold_io_type;
    injection_config_perf_press_source_t performance_fuel_pressure_fuelramp_source;
    ecu_config_io_map_t performance_fuel_pressure_fuelramp_io_type;

    float performance_fuel_pressure_manifold_clamp_min;
    float performance_fuel_pressure_manifold_clamp_max;
    float performance_fuel_pressure_fuelramp_clamp_min;
    float performance_fuel_pressure_fuelramp_clamp_max;

    float phase_add;
    float phase_slew_rate;
    injection_config_group_phase_mode_t phase_mode;
    injection_config_relation_t rpm_to_phase_add;

    injection_config_group_late_phase_mode_t enrichment_late_phase_mode;
    injection_config_relation_t enrichment_late_phase;

    float inject_mass_low_threshold;
    float inject_mass_reduction;
    float dutycycle_warning;
    float dutycycle_limit;
    injection_config_dutycycle_limit_mode_t dutycycle_limit_mode;
}injection_config_group_setup_t;

typedef struct {
    bool enabled;
    float signal_prepare_advance;
    injection_config_process_update_trigger_t process_update_trigger;
    injection_config_uspd_source_t uspd_source;
    ecu_gpio_input_pin_t power_voltage_pin;
    injection_config_group_setup_t groups[INJECTION_CONFIG_GROUP_MAX];

    uint32_t align ALIGNED_CACHE;
}injection_config_v1_t ALIGNED_CACHE;

typedef injection_config_v1_t injection_config_t;

#endif /* CONFIG_VERSIONED_CORE_INC_VERSIONED_INJECTION_H_ */
