/*
 * versioned_stepper.h
 *
 *  Created on: May 1, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_PROJECT_INC_VERSIONED_STEPPER_H_
#define CONFIG_PROJECT_INC_VERSIONED_STEPPER_H_

#include <stdint.h>

typedef enum {
  STEPPER_CONFIG_VERSION_V1 = 0,
  STEPPER_CONFIG_VERSION_MAX
}stepper_config_versions_t;

#define TLE4729_RELATION_ITEMS_MAX    (16)

typedef struct {
    uint8_t items;
    float input[TLE4729_RELATION_ITEMS_MAX];
    float output[TLE4729_RELATION_ITEMS_MAX];
}tle7429_config_relation_t;

typedef struct {
    tle7429_config_relation_t voltage_to_step_time_mult;
    tle7429_config_relation_t speed_to_step_time_ms;
    uint8_t acceleration_steps;

    int32_t pos_min;
    int32_t pos_max;

}tle4729_config_v1_t;

typedef tle4729_config_v1_t tle4729_config_t;

#endif /* CONFIG_PROJECT_INC_VERSIONED_STEPPER_H_ */
