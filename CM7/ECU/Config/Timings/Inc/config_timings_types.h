/*
 * config_timings_types.h
 *
 *  Created on: Nov 13, 2025
 *      Author: VHEMaster
 */

#ifndef CONFIG_TIMINGS_INC_CONFIG_TIMINGS_TYPES_H_
#define CONFIG_TIMINGS_INC_CONFIG_TIMINGS_TYPES_H_

#include "common.h"

#define ECU_TIMINGS_MAX (       \
    ECU_TIMING_BASE_MAX       + \
    ECU_TIMING_IGNITION_MAX   + \
    ECU_TIMING_INJECTION_MAX  + \
    ECU_TIMING_ROUGH_MAX)

#define ECU_TIMING_NONE         (255)
#define ECU_TIMING_INSTANCE_MAX (6)

typedef uint32_t ecu_timing_instance_t;

typedef enum {
  ECU_TIMING_TYPE_BASE = 0,
  ECU_TIMING_TYPE_IGNITION,
  ECU_TIMING_TYPE_INJECTION,
  ECU_TIMING_TYPE_ROUGH,
  ECU_TIMING_TYPE_MAX
}ecu_timing_type_t;

typedef enum {
  ECU_TIMING_BASE_1 = 0,
  ECU_TIMING_BASE_MAX
}ecu_timing_base_t;

typedef enum {
  ECU_TIMING_IGNITION_1 = 0,
  ECU_TIMING_IGNITION_MAX
}ecu_timing_ignition_t;

typedef enum {
  ECU_TIMING_INJECTION_1 = 0,
  ECU_TIMING_INJECTION_MAX
}ecu_timing_injection_t;

typedef enum {
  ECU_TIMING_ROUGH_1 = 0,
  ECU_TIMING_ROUGH_MAX
}ecu_timing_rough_t;

#endif /* CONFIG_TIMINGS_INC_CONFIG_TIMINGS_TYPES_H_ */
