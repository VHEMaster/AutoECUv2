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
  CALCDATA_RELATION_INPUT_SOURCE_SENSOR_APS = 0,
  CALCDATA_RELATION_INPUT_SOURCE_SENSOR_CKP,
  CALCDATA_RELATION_INPUT_SOURCE_SENSOR_CMP,
  CALCDATA_RELATION_INPUT_SOURCE_SENSOR_ECT,
  CALCDATA_RELATION_INPUT_SOURCE_SENSOR_EGT,
  CALCDATA_RELATION_INPUT_SOURCE_SENSOR_IAT,
  CALCDATA_RELATION_INPUT_SOURCE_SENSOR_MAF,
  CALCDATA_RELATION_INPUT_SOURCE_SENSOR_MAP,
  CALCDATA_RELATION_INPUT_SOURCE_SENSOR_OPS,
  CALCDATA_RELATION_INPUT_SOURCE_SENSOR_OTS,
  CALCDATA_RELATION_INPUT_SOURCE_SENSOR_TPS,
  CALCDATA_RELATION_INPUT_SOURCE_SENSOR_VSS,

  CALCDATA_RELATION_INPUT_SOURCE_CALC_CYCLE_FILLING,
  CALCDATA_RELATION_INPUT_SOURCE_CALC_MASS_AIR_FLOW,
  CALCDATA_RELATION_INPUT_SOURCE_CALC_ENGINE_LOAD,
  CALCDATA_RELATION_INPUT_SOURCE_MAX,
}ecu_config_calcdata_relation_input_source_index_t;

typedef struct {
    ecu_config_calcdata_relation_input_source_index_t source;
    ecu_config_calcdata_relation_input_varianted_index_t variant;
}ecu_config_calcdata_relation_input_select_t;

typedef struct {
    ecu_config_tables_table_index_t table_index;
}ecu_config_calcdata_relation_input_varianted_item_t;

typedef struct {
    uint8_t variations;
    ecu_config_calcdata_relation_input_varianted_item_t variants[CALCDATA_RELATION_INPUT_VARIANTED_ITEM_MAX];
}ecu_config_calcdata_relation_input_varianted_t;

typedef struct {
    ecu_config_calcdata_relation_input_varianted_t sources[CALCDATA_RELATION_INPUT_SOURCE_MAX];

    uint32_t align ALIGNED_CACHE;
}ecu_config_calcdata_v1_t ALIGNED_CACHE;

typedef ecu_config_calcdata_v1_t ecu_config_calcdata_t;

#endif /* CONFIG_VERSIONED_CALIBRATION_INC_VERSIONED_CALCDATA_H_ */
