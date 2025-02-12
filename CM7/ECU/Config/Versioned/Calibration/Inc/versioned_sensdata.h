/*
 * versioned_sensdata.h
 *
 *  Created on: Feb 11, 2025
 *      Author: VHEMaster
 */

#ifndef CONFIG_VERSIONED_CALIBRATION_INC_VERSIONED_SENSDATA_H_
#define CONFIG_VERSIONED_CALIBRATION_INC_VERSIONED_SENSDATA_H_

#include "common.h"
#include "versioned_tables.h"

typedef enum {
  ECU_CONFIG_SENSDATA_VERSION_V1 = 0,
  ECU_CONFIG_SENSDATA_VERSION_MAX
}ecu_config_sensdata_versions_t;

#define SENSDATA_RELATION_INPUT_VARIANTED_ITEMS_MAX 32

typedef enum {
  SENSDATA_RELATION_INPUT_VARIANTED_ITEM_V1 = 0,
  SENSDATA_RELATION_INPUT_VARIANTED_ITEM_V2,
  SENSDATA_RELATION_INPUT_VARIANTED_ITEM_MAX
}ecu_config_sensdata_relation_input_varianted_index_t;

typedef enum {
  SENSDATA_RELATION_INPUT_SOURCE_SENSOR_APS = 0,
  SENSDATA_RELATION_INPUT_SOURCE_SENSOR_CKP,
  SENSDATA_RELATION_INPUT_SOURCE_SENSOR_CMP,
  SENSDATA_RELATION_INPUT_SOURCE_SENSOR_ECT,
  SENSDATA_RELATION_INPUT_SOURCE_SENSOR_EGT,
  SENSDATA_RELATION_INPUT_SOURCE_SENSOR_IAT,
  SENSDATA_RELATION_INPUT_SOURCE_SENSOR_MAF,
  SENSDATA_RELATION_INPUT_SOURCE_SENSOR_MAP,
  SENSDATA_RELATION_INPUT_SOURCE_SENSOR_OPS,
  SENSDATA_RELATION_INPUT_SOURCE_SENSOR_OTS,
  SENSDATA_RELATION_INPUT_SOURCE_SENSOR_TPS,
  SENSDATA_RELATION_INPUT_SOURCE_SENSOR_VSS,

  SENSDATA_RELATION_INPUT_SOURCE_CALC_CYCLE_FILLING,
  SENSDATA_RELATION_INPUT_SOURCE_CALC_MASS_AIR_FLOW,
  SENSDATA_RELATION_INPUT_SOURCE_CALC_ENGINE_LOAD,
  SENSDATA_RELATION_INPUT_SOURCE_MAX,
}ecu_config_sensdata_relation_input_source_index_t;

typedef struct {
    ecu_config_sensdata_relation_input_source_index_t source;
    ecu_config_sensdata_relation_input_varianted_index_t variant;
}ecu_config_sensdata_relation_input_select_t;

typedef struct {
    ecu_config_tables_table_index_t table_index;
}ecu_config_sensdata_relation_input_varianted_item_t;

typedef struct {
    uint8_t variations;
    ecu_config_sensdata_relation_input_varianted_item_t variants[SENSDATA_RELATION_INPUT_VARIANTED_ITEM_MAX];
}ecu_config_sensdata_relation_input_varianted_t;

typedef struct {
    ecu_config_sensdata_relation_input_varianted_t sources[SENSDATA_RELATION_INPUT_SOURCE_MAX];

    uint32_t align ALIGNED_CACHE;
}ecu_config_sensdata_v1_t ALIGNED_CACHE;

typedef ecu_config_sensdata_v1_t ecu_config_sensdata_t;

#endif /* CONFIG_VERSIONED_CALIBRATION_INC_VERSIONED_SENSDATA_H_ */
