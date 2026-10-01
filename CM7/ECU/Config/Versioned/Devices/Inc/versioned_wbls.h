/*
 * versioned_wbls.h
 *
 *  Created on: Apr 30, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_PROJECT_INC_VERSIONED_WBLS_H_
#define CONFIG_PROJECT_INC_VERSIONED_WBLS_H_

#include "bool.h"
#include "pid.h"
#include "time.h"
#include "compiler.h"
#include <stdint.h>

typedef enum {
  WBLS_CONFIG_VERSION_V1 = 0,
  WBLS_CONFIG_VERSION_MAX
}wbls_config_versions_t;

#define CJ125_RELATION_ITEMS_MAX      (32)

typedef enum {
  CJ125_AF_8 = 0,
  CJ125_AF_17,
  CJ125_AF_MAX
}cj125_af_t;

typedef struct {
    uint8_t items;
    float input[CJ125_RELATION_ITEMS_MAX];
    float output[CJ125_RELATION_ITEMS_MAX];
}cj125_config_relation_t;

typedef enum {
  CJ125_CONFIG_PRC_OFF = 0,
  CJ125_CONFIG_PRC_10UA = 1,
  CJ125_CONFIG_PRC_20UA = 2,
  CJ125_CONFIG_PRC_40UA = 4,
  CJ125_CONFIG_PRC_80UA = 8,
  CJ125_CONFIG_PRC_MAX = 15
}cj125_config_prc_t;

typedef enum {
  CJ125_CONFIG_HEATED_TEMP_THR_SRC_RESISTANCE = 0,
  CJ125_CONFIG_HEATED_TEMP_THR_SRC_TEMPERATURE,
  CJ125_CONFIG_HEATED_TEMP_THR_SRC_COUNT
}cj125_config_heated_temp_thr_src_t;

typedef enum {
  CJ125_CONFIG_STAGED_UNITED = 0,
  CJ125_CONFIG_STAGED_STAGED,
  CJ125_CONFIG_STAGED_COUNT
}cj125_config_staged_nunited_t;

typedef enum {
  CJ125_CONFIG_STAGED_STAGE_IDLE = 0,
  CJ125_CONFIG_STAGED_STAGE_PREHEAT,
  CJ125_CONFIG_STAGED_STAGE_HEATUP_INITIAL,
  CJ125_CONFIG_STAGED_STAGE_HEATUP_FINAL,
  CJ125_CONFIG_STAGED_STAGE_OPERATING,
  CJ125_CONFIG_STAGED_STAGE_COUNT
}cj125_config_staged_stage_t;

typedef struct  {
    bool reg_enscun;
    bool reg_set_dia_q;
    bool pa_enabled;
    cj125_config_prc_t pump_ref_current;
}cj125_config_stage_t;

typedef struct {
    bool enabled;

    cj125_config_relation_t res_to_temp_relation;
    cj125_config_relation_t curr_to_lambda_relation;

    cj125_config_heated_temp_thr_src_t heated_value_thr_source;
    bool heated_value_thr_override;

    float heatup_ref_resistance_cplt;
    float heatup_ref_temperature_cplt;

    float heatup_ref_resistance_initial;
    float heatup_ref_temperature_initial;

    float calibration_ref_resistance;
    float calibration_ref_res_max_deviation;
    float shunt_resistance;
    float pushpull_resistance;
    cj125_af_t ampfactor;

    cj125_config_staged_nunited_t staged_nunited;
    cj125_config_stage_t config_united;
    cj125_config_stage_t config_staged[CJ125_CONFIG_STAGED_STAGE_COUNT];


    bool lambda_temperature_correction;
    float temperature_correction_value;
    float temperature_correction_reference;

    float heater_preheat_voltage;
    float heater_initial_voltage;
    float heater_initial_max_voltage;
    float heater_max_voltage;
    float heater_ramp_rate;
    float heater_nominal_voltage;
    time_us_t heater_temperature_timeout;
    math_pid_koffs_t heater_pid_koffs;
    time_us_t heater_pid_update_period;

    time_delta_us_t pid_cb_period;

    uint32_t align ALIGNED_CACHE;
}cj125_config_v1_t ALIGNED_CACHE;

typedef cj125_config_v1_t cj125_config_t;

#endif /* CONFIG_PROJECT_INC_VERSIONED_WBLS_H_ */
