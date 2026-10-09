/*
 * versioned_cmp.h
 *
 *  Created on: May 14, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_VERSIONED_SENSORS_INC_VERSIONED_CMP_H_
#define CONFIG_VERSIONED_SENSORS_INC_VERSIONED_CMP_H_

#include "common.h"
#include "time.h"
#include "config_gpio.h"

typedef enum {
  CMP_CONFIG_VERSION_V1 = 0,
  CMP_CONFIG_VERSION_MAX
}cmp_config_versions_t;

typedef enum {
  CMP_CONFIG_SIGNAL_REF_TYPE_SINGLEPULSE = 0,
  CMP_CONFIG_SIGNAL_REF_TYPE_PATTERNED,
  CMP_CONFIG_SIGNAL_REF_TYPE_MAX,
}cmp_config_signal_ref_type_t;

typedef enum {
  CMP_CONFIG_SIGNAL_POLARITY_ACTIVE_HIGH = 0,
  CMP_CONFIG_SIGNAL_POLARITY_ACTIVE_LOW,
  CMP_CONFIG_SIGNAL_POLARITY_MAX,
}cmp_config_signal_polarity_t;

typedef struct {
    float pulse_edge_pos_min;
    float pulse_edge_pos_max;
    float pulse_width_min;
    float pulse_width_max;
    cmp_config_signal_polarity_t polarity;
}cmp_config_signal_ref_type_singlepulse_t;

#define CMP_CONFIG_PATTERNED_EDGES_MAX    16

typedef struct {
    float angle;  /* 0 .. 720 crank degrees at the calibrated VVT reference */
    bool rising;  /* true: LOW -> HIGH, false: HIGH -> LOW */
}cmp_config_patterned_edge_t;

typedef struct {
    uint8_t edges_count;
    float reference_offset;          /* CKP 0 -> profile geometry, crank degrees */
    float vvt_min;                   /* Diagnostic lower bound of relative VVT, crank degrees */
    float vvt_max;                   /* Diagnostic upper bound; not a synchronization gate */
    float angle_tolerance;           /* Per-edge position tolerance, crank degrees */
    float interval_tolerance;        /* Between-edge tolerance, crank degrees */
    float vvt_slew_rate;             /* Maximum VVT movement, crank degrees / second */
    cmp_config_patterned_edge_t edges[CMP_CONFIG_PATTERNED_EDGES_MAX];
}cmp_config_signal_ref_type_patterned_t;

typedef struct {
    cmp_config_signal_ref_type_singlepulse_t singlepulse;
    cmp_config_signal_ref_type_patterned_t patterned;
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
