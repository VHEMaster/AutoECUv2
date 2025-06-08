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
  CALCDATA_RELATION_INPUT_SOURCE_SENSOR_BANKED_MAF,
  CALCDATA_RELATION_INPUT_SOURCE_SENSOR_BANKED_TPS,
  CALCDATA_RELATION_INPUT_SOURCE_SENSOR_BANKED_LAST,
  CALCDATA_RELATION_INPUT_SOURCE_SENSOR_BANKED_COUNT = CALCDATA_RELATION_INPUT_SOURCE_SENSOR_BANKED_LAST - CALCDATA_RELATION_INPUT_SOURCE_SENSOR_BANKED_FIRST,

  CALCDATA_RELATION_INPUT_SOURCE_CALC_FIRST = CALCDATA_RELATION_INPUT_SOURCE_SENSOR_BANKED_LAST,
  CALCDATA_RELATION_INPUT_SOURCE_CALC_IAT_MANIFOLD = CALCDATA_RELATION_INPUT_SOURCE_CALC_FIRST,
  CALCDATA_RELATION_INPUT_SOURCE_CALC_MAP_MANIFOLD,
  CALCDATA_RELATION_INPUT_SOURCE_CALC_CYCLE_FILLING,
  CALCDATA_RELATION_INPUT_SOURCE_CALC_MASS_AIR_FLOW,
  CALCDATA_RELATION_INPUT_SOURCE_CALC_ENGINE_LOAD,
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
  CALCDATA_OUTPUT_RELATIVE_FILLING,
  CALCDATA_OUTPUT_IGNITION_ADVANCE,
  CALCDATA_OUTPUT_AIR_TO_FUEL_RATIO,
  CALCDATA_OUTPUT_INJECTION_PHASE,

  CALCDATA_OUTPUT_MAX,
}ecu_config_calcdata_output_data_index_t;

typedef enum {
  CALCDATA_IAT_ALPHA_BLENDING_FILTER_1 = 0,
  CALCDATA_IAT_ALPHA_BLENDING_FILTER_2,
  CALCDATA_IAT_ALPHA_BLENDING_FILTER_MAX
}ecu_config_calcdata_setup_iat_sensor_alpha_blending_filter_number_t;

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
    ecu_config_tables_table_index_t table_index;
    ecu_config_calcdata_relation_input_select_t source_x;
}ecu_config_calcdata_output_data_item_1d_t;

typedef struct {
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
    ecu_config_calcdata_setup_iat_t iat_model;
}ecu_config_calcdata_setup_t;

typedef struct {
    ecu_config_calcdata_relation_input_varianted_items_t relation_sources;
    ecu_config_calcdata_output_data_varianted_items_t output_data;
    ecu_config_calcdata_setup_t setup;

    uint32_t align ALIGNED_CACHE;
}ecu_config_calcdata_v1_t ALIGNED_CACHE;

typedef ecu_config_calcdata_v1_t ecu_config_calcdata_t;

#endif /* CONFIG_VERSIONED_CALIBRATION_INC_VERSIONED_CALCDATA_H_ */
