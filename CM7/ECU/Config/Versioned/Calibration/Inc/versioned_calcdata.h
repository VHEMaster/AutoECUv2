/*
 * versioned_calcdata.h
 *
 *  Created on: Feb 11, 2025
 *      Author: VHEMaster
 */

#ifndef CONFIG_VERSIONED_CALIBRATION_INC_VERSIONED_CALCDATA_H_
#define CONFIG_VERSIONED_CALIBRATION_INC_VERSIONED_CALCDATA_H_

#include "common.h"
#include "versioned_tables.h"

typedef enum {
  ECU_CONFIG_CALCDATA_VERSION_V1 = 0,
  ECU_CONFIG_CALCDATA_VERSION_MAX
}ecu_config_calcdata_versions_t;

#define CALCDATA_RELATION_INPUT_VARIANTED_ITEMS_MAX 32

typedef enum {
  CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V1 = 0,
  CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V2,
  CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V3,
  CALCDATA_RELATION_INPUT_VARIANTED_ITEM_MAX
}ecu_config_calcdata_relation_input_varianted_index_t;

typedef enum {
  CALCDATA_OUTPUT_VARIANTED_ITEM_V1 = 0,
  CALCDATA_OUTPUT_VARIANTED_ITEM_V2,
  CALCDATA_OUTPUT_VARIANTED_ITEM_MAX
}ecu_config_calcdata_output_varianted_index_t;

typedef enum {
  CALCDATA_RELATION_INPUT_SOURCE_SENSOR_GLOBAL_FIRST = 0,
  CALCDATA_RELATION_INPUT_SOURCE_SENSOR_GLOBAL_APS = CALCDATA_RELATION_INPUT_SOURCE_SENSOR_GLOBAL_FIRST,
  CALCDATA_RELATION_INPUT_SOURCE_SENSOR_GLOBAL_CKP,
  CALCDATA_RELATION_INPUT_SOURCE_SENSOR_GLOBAL_ECT,
  CALCDATA_RELATION_INPUT_SOURCE_SENSOR_GLOBAL_OPS,
  CALCDATA_RELATION_INPUT_SOURCE_SENSOR_GLOBAL_OTS,
  CALCDATA_RELATION_INPUT_SOURCE_SENSOR_GLOBAL_VSS,
  CALCDATA_RELATION_INPUT_SOURCE_SENSOR_GLOBAL_LAST,
  CALCDATA_RELATION_INPUT_SOURCE_SENSOR_GLOBAL_COUNT = CALCDATA_RELATION_INPUT_SOURCE_SENSOR_GLOBAL_LAST - CALCDATA_RELATION_INPUT_SOURCE_SENSOR_GLOBAL_FIRST,

  CALCDATA_RELATION_INPUT_SOURCE_SENSOR_BANKED_FIRST = CALCDATA_RELATION_INPUT_SOURCE_SENSOR_GLOBAL_LAST,
  CALCDATA_RELATION_INPUT_SOURCE_SENSOR_BANKED_CMP_IN = CALCDATA_RELATION_INPUT_SOURCE_SENSOR_BANKED_FIRST,
  CALCDATA_RELATION_INPUT_SOURCE_SENSOR_BANKED_CMP_EX,
  CALCDATA_RELATION_INPUT_SOURCE_SENSOR_BANKED_EGT,
  CALCDATA_RELATION_INPUT_SOURCE_SENSOR_BANKED_IAT,
  CALCDATA_RELATION_INPUT_SOURCE_SENSOR_BANKED_MAF,
  CALCDATA_RELATION_INPUT_SOURCE_SENSOR_BANKED_MAP,
  CALCDATA_RELATION_INPUT_SOURCE_SENSOR_BANKED_TPS,
  CALCDATA_RELATION_INPUT_SOURCE_SENSOR_BANKED_LAST,
  CALCDATA_RELATION_INPUT_SOURCE_SENSOR_BANKED_COUNT = CALCDATA_RELATION_INPUT_SOURCE_SENSOR_BANKED_LAST - CALCDATA_RELATION_INPUT_SOURCE_SENSOR_BANKED_FIRST,

  CALCDATA_RELATION_INPUT_SOURCE_CALC_FIRST = CALCDATA_RELATION_INPUT_SOURCE_SENSOR_BANKED_LAST,
  CALCDATA_RELATION_INPUT_SOURCE_CALC_IAT_MANIFOLD = CALCDATA_RELATION_INPUT_SOURCE_CALC_FIRST,
  CALCDATA_RELATION_INPUT_SOURCE_CALC_CYCLE_CHARGE,
  CALCDATA_RELATION_INPUT_SOURCE_CALC_MASS_AIR_FLOW,
  CALCDATA_RELATION_INPUT_SOURCE_CALC_IDLE_TARGET_RPM_DEVIATION,

  CALCDATA_RELATION_INPUT_SOURCE_CALC_LAST,
  CALCDATA_RELATION_INPUT_SOURCE_CALC_COUNT = CALCDATA_RELATION_INPUT_SOURCE_CALC_LAST - CALCDATA_RELATION_INPUT_SOURCE_CALC_FIRST,

  CALCDATA_RELATION_INPUT_SOURCE_MAX = CALCDATA_RELATION_INPUT_SOURCE_CALC_LAST,
}ecu_config_calcdata_relation_input_source_index_t;

typedef enum {
  CALCDATA_OUTPUT_TYPE_NONE = -1,
  CALCDATA_OUTPUT_TYPE_FAILSAFE,
  CALCDATA_OUTPUT_TYPE_1D,
  CALCDATA_OUTPUT_TYPE_2D,
  CALCDATA_OUTPUT_TYPE_MAX,
}ecu_config_calcdata_output_data_type_t;

typedef enum {
  CALCDATA_OUTPUT_IAT_ALPHA_BLENDING = 0,
  CALCDATA_OUTPUT_IAT_DIRECT_INFLUENCE,
  CALCDATA_OUTPUT_VOLUMETRIC_EFFICIENCY,
  CALCDATA_OUTPUT_IGNITION_ADVANCE,
  CALCDATA_OUTPUT_AIR_TO_FUEL_RATIO,
  CALCDATA_OUTPUT_INJECTION_PHASE,

  CALCDATA_OUTPUT_STARTUP_IGNITION_ADVANCE,
  CALCDATA_OUTPUT_STARTUP_INJECTION_PHASE,
  CALCDATA_OUTPUT_STARTUP_POS_INJ_INFLUENCE,
  CALCDATA_OUTPUT_STARTUP_LARGE_INJ_CHARGE,
  CALCDATA_OUTPUT_STARTUP_SMALL_INJ_CHARGE,
  CALCDATA_OUTPUT_STARTUP_RUNUP_INJ_DURATION,
  CALCDATA_OUTPUT_STARTUP_RUNUP_IGN_DURATION,
  CALCDATA_OUTPUT_STARTUP_RUNUP_IGN_ADVANCE,
  CALCDATA_OUTPUT_STARTUP_COLD_INJ_CORR,
  CALCDATA_OUTPUT_STARTUP_COLD_INJ_TIME,
  CALCDATA_OUTPUT_STARTUP_ECON_BYPASS_TIME,

  CALCDATA_OUTPUT_WARMUP_IDLE_INJ_CORR,

  CALCDATA_OUTPUT_IDLE_RPM_THR_FLAG_L,
  CALCDATA_OUTPUT_IDLE_RPM_THR_FLAG_H,
  CALCDATA_OUTPUT_IDLE_TARGET_RPM,
  CALCDATA_OUTPUT_IDLE_TARGET_IGNITION_ADVANCE,
  CALCDATA_OUTPUT_IDLE_TARGET_MASS_AIR_FLOW,
  CALCDATA_OUTPUT_IDLE_INITIAL_IGNITION_ADVANCE,
  CALCDATA_OUTPUT_IDLE_TPS_IGNITION_CTRL,

  CALCDATA_OUTPUT_IDLE_REG_STEPPER_PID_RPM_P,
  CALCDATA_OUTPUT_IDLE_REG_STEPPER_PID_RPM_I,
  CALCDATA_OUTPUT_IDLE_REG_STEPPER_PID_RPM_D,
  CALCDATA_OUTPUT_IDLE_REG_STEPPER_PID_MAF_P,
  CALCDATA_OUTPUT_IDLE_REG_STEPPER_PID_MAF_I,
  CALCDATA_OUTPUT_IDLE_REG_STEPPER_PID_MAF_D,
  CALCDATA_OUTPUT_IDLE_REG_ETC_PID_RPM_P,
  CALCDATA_OUTPUT_IDLE_REG_ETC_PID_RPM_I,
  CALCDATA_OUTPUT_IDLE_REG_ETC_PID_RPM_D,
  CALCDATA_OUTPUT_IDLE_REG_ETC_PID_MAF_P,
  CALCDATA_OUTPUT_IDLE_REG_ETC_PID_MAF_I,
  CALCDATA_OUTPUT_IDLE_REG_ETC_PID_MAF_D,
  CALCDATA_OUTPUT_IDLE_IGN_ADV_PID_RPM_P,
  CALCDATA_OUTPUT_IDLE_IGN_ADV_PID_RPM_I,
  CALCDATA_OUTPUT_IDLE_IGN_ADV_PID_RPM_D,

  CALCDATA_OUTPUT_ETC_STOPPED_POS,
  CALCDATA_OUTPUT_ETC_STARTUP_POS,
  CALCDATA_OUTPUT_ETC_IDLE_TARGET_POS,
  CALCDATA_OUTPUT_ETC_RUNNING_POS,
  CALCDATA_OUTPUT_ETC_RUNUP_MOVE_TIME,
  CALCDATA_OUTPUT_ETC_APS_IGNITION_CTRL,



  CALCDATA_OUTPUT_MAX,
}ecu_config_calcdata_output_data_index_t;

typedef enum {
  CALCDATA_IAT_ALPHA_BLENDING_FILTER_1 = 0,
  CALCDATA_IAT_ALPHA_BLENDING_FILTER_2,
  CALCDATA_IAT_ALPHA_BLENDING_FILTER_MAX
}ecu_config_calcdata_setup_iat_sensor_alpha_blending_filter_number_t;

typedef enum {
  CALCDATA_AIR_CALC_MODEL_INDEX_1 = 0,
  CALCDATA_AIR_CALC_MODEL_INDEX_2,
  CALCDATA_AIR_CALC_MODEL_INDEX_3,
  CALCDATA_AIR_CALC_MODEL_INDEX_4,
  CALCDATA_AIR_CALC_MODEL_INDEX_MAX
}ecu_config_calcdata_setup_air_calc_model_index_t;

typedef enum {
  CALCDATA_AIR_CALC_MODEL_SOURCE_NONE = 0,
  CALCDATA_AIR_CALC_MODEL_SOURCE_MAP,
  CALCDATA_AIR_CALC_MODEL_SOURCE_TPS,
  CALCDATA_AIR_CALC_MODEL_SOURCE_MAF,
  CALCDATA_AIR_CALC_MODEL_SOURCE_MAX
}ecu_config_calcdata_setup_air_calc_model_source_type_t;

typedef enum {
  CALCDATA_ECT_CALC_MODEL_MEAN = 0,
  CALCDATA_ECT_CALC_MODEL_MAXIMAL,
  CALCDATA_ECT_CALC_MODEL_RMS,
  CALCDATA_ECT_CALC_MODEL_MAX
}ecu_config_calcdata_setup_ect_calc_model_t;

typedef enum {
  CALCDATA_IAT_DIRECT_INFLUENCE_TYPE_NONE = 0,
  CALCDATA_IAT_DIRECT_INFLUENCE_TYPE_RECALC,
  CALCDATA_IAT_DIRECT_INFLUENCE_TYPE_TABLE,
  CALCDATA_IAT_DIRECT_INFLUENCE_TYPE_MAX,
}ecu_config_calcdata_setup_air_calc_model_iat_direct_influence_source_t;

typedef enum {
  CALCDATA_IDLE_POS_FLAG_MODE_NONE = 0,
  CALCDATA_IDLE_POS_FLAG_MODE_TPS,
  CALCDATA_IDLE_POS_FLAG_MODE_APS,
  CALCDATA_IDLE_POS_FLAG_MODE_MAP,
  CALCDATA_IDLE_POS_FLAG_MODE_MAF,
  CALCDATA_IDLE_POS_FLAG_MODE_MAX
}ecu_config_calcdata_setup_idle_pos_flag_mode_t;

typedef enum {
  CALCDATA_IDLE_IGN_CTRL_SRC_NONE = 0,
  CALCDATA_IDLE_IGN_CTRL_SRC_TPS,
  CALCDATA_IDLE_IGN_CTRL_SRC_APS,
  CALCDATA_IDLE_IGN_CTRL_SRC_MAX
}ecu_config_calcdata_setup_idle_ign_ctrl_src_t;

typedef enum {
  CALCDATA_IDLE_REGULATOR_NONE = 0,
  CALCDATA_IDLE_REGULATOR_STEPPER,
  CALCDATA_IDLE_REGULATOR_ETC,
  CALCDATA_IDLE_REGULATOR_MAX
}ecu_config_calcdata_setup_idle_regulator_mode_t;

typedef struct {
    ecu_config_calcdata_relation_input_source_index_t source;
    ecu_config_calcdata_relation_input_varianted_index_t variant;
}ecu_config_calcdata_relation_input_select_t;

typedef struct {
    ecu_config_tables_table_index_t table_index;
}ecu_config_calcdata_relation_input_item_t;

typedef struct {
    uint8_t variations;
    ecu_config_calcdata_relation_input_item_t variants[CALCDATA_RELATION_INPUT_VARIANTED_ITEM_MAX];
}ecu_config_calcdata_relation_input_varianted_item_t;

typedef struct {
    float value;
}ecu_config_calcdata_output_data_item_static_t;

typedef struct {
    bool interpolation_limit;
    ecu_config_tables_table_index_t table_index;
    ecu_config_calcdata_relation_input_select_t source_x;
}ecu_config_calcdata_output_data_item_1d_t;

typedef struct {
    bool interpolation_limit;
    ecu_config_tables_table_index_t table_index;
    ecu_config_calcdata_relation_input_select_t source_x;
    ecu_config_calcdata_relation_input_select_t source_y;
}ecu_config_calcdata_output_data_item_2d_t;

typedef struct {
    ecu_config_calcdata_output_data_type_t type;
    ecu_config_calcdata_output_data_item_1d_t data_1d;
    ecu_config_calcdata_output_data_item_2d_t data_2d;
}ecu_config_calcdata_output_data_item_t;

typedef struct {
    ecu_config_calcdata_output_data_item_static_t data_failsafe;
    uint8_t variations;
    ecu_config_calcdata_output_data_item_t variants[CALCDATA_OUTPUT_VARIANTED_ITEM_MAX];
}ecu_config_calcdata_output_data_varianted_item_t;

typedef struct {
    ecu_config_calcdata_relation_input_varianted_item_t items[CALCDATA_RELATION_INPUT_SOURCE_MAX];
}ecu_config_calcdata_relation_input_varianted_items_t;

typedef struct {
    ecu_config_calcdata_output_data_varianted_item_t items[CALCDATA_OUTPUT_MAX];
}ecu_config_calcdata_output_data_varianted_items_t;

typedef struct {
    float volume;
    float low_pass;
}ecu_config_calcdata_setup_iat_sensor_alpha_blending_filter_t;

typedef struct {
    ecu_config_calcdata_output_varianted_index_t calc_variant;
    ecu_config_calcdata_setup_iat_sensor_alpha_blending_filter_t filters[CALCDATA_IAT_ALPHA_BLENDING_FILTER_MAX];
}ecu_config_calcdata_setup_iat_sensor_alpha_blending_t;

typedef struct {
    ecu_config_calcdata_setup_iat_sensor_alpha_blending_t alpha_blending;
}ecu_config_calcdata_setup_iat_sensor_t;

typedef struct {
    ecu_config_calcdata_setup_iat_sensor_t sensors[ECU_CONFIG_IO_IAT_MAX];
}ecu_config_calcdata_setup_iat_t;

typedef struct {
    ecu_config_calcdata_setup_ect_calc_model_t calc_model;
    float rms_min;
}ecu_config_calcdata_setup_ect_t;

typedef struct {
    ecu_config_calcdata_setup_air_calc_model_iat_direct_influence_source_t direct_type;
    ecu_config_calcdata_output_varianted_index_t direct_table_variant;
}ecu_config_calcdata_setup_air_calc_model_iat_influence_t;

typedef struct {
    ecu_config_calcdata_setup_air_calc_model_iat_influence_t iat_influence;
    ecu_config_calcdata_setup_air_calc_model_source_type_t model;
    ecu_config_calcdata_output_varianted_index_t ve_out_variant;
}ecu_config_calcdata_setup_air_calc_model_source_t;

typedef struct {
    float air_temperature;
    float absolute_pressure;
}ecu_config_calcdata_setup_air_calc_model_air_conditions_t;

typedef struct {
    ecu_config_calcdata_setup_air_calc_model_air_conditions_t normal_conditions;
    ecu_config_calcdata_setup_air_calc_model_source_t sources[CALCDATA_AIR_CALC_MODEL_INDEX_MAX];
}ecu_config_calcdata_setup_air_calc_model_t;

typedef struct {
    ecu_config_calcdata_setup_idle_regulator_mode_t regulator_mode;
    bool use_reg_pid_rpm;
    bool use_reg_pid_maf;
    bool use_ign_pid_rpm;

    float reg_pid_rpm_min;
    float reg_pid_rpm_max;
    float reg_pid_maf_min;
    float reg_pid_maf_max;
    float reg_pid_min;
    float reg_pid_max;
    float ign_pid_rpm_min;
    float ign_pid_rpm_max;

    ecu_config_calcdata_setup_idle_ign_ctrl_src_t ignition_ctrl_source;
    ecu_config_calcdata_setup_idle_pos_flag_mode_t pos_flag_mode;
    bool use_initial_ign_advance;
    float pos_flag_thr_l;
    float pos_flag_thr_h;
}ecu_config_calcdata_setup_idle_t;

typedef struct {
    ecu_config_calcdata_setup_iat_t iat_model;
    ecu_config_calcdata_setup_ect_t ect_model;
    ecu_config_calcdata_setup_air_calc_model_t air_calc_model;
    ecu_config_calcdata_setup_idle_t idle;
    float runup_rpm_threshold_low;
    float runup_rpm_threshold_high;
    float startup_large_revs;
    float startup_large_to_small_trans_revs;
    float startup_transition_reset;
}ecu_config_calcdata_setup_t;

typedef struct {
    ecu_config_calcdata_relation_input_varianted_items_t relation_sources;
    ecu_config_calcdata_output_data_varianted_items_t output_data;
    ecu_config_calcdata_setup_t setup;

    uint32_t align ALIGNED_CACHE;
}ecu_config_calcdata_v1_t ALIGNED_CACHE;

typedef ecu_config_calcdata_v1_t ecu_config_calcdata_t;

#endif /* CONFIG_VERSIONED_CALIBRATION_INC_VERSIONED_CALCDATA_H_ */
