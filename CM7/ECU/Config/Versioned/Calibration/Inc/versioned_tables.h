/*
 * versioned_tables.h
 *
 *  Created on: Feb 11, 2025
 *      Author: VHEMaster
 */

#ifndef CONFIG_VERSIONED_CALIBRATION_INC_VERSIONED_TABLES_H_
#define CONFIG_VERSIONED_CALIBRATION_INC_VERSIONED_TABLES_H_

#include "versioned_id.h"
#include "versioned_io.h"
#include "versioned_cylinders.h"
#include "versioned_injection.h"
#include "versioned_ignition.h"
#include "versioned_sensdata.h"

typedef enum {
  ECU_CONFIG_TABLES_VERSION_V1 = 0,
  ECU_CONFIG_TABLES_VERSION_MAX
}ecu_config_tables_versions_t;

typedef struct {
    ecu_config_sensdata_relation_input_select_t axis_x;
    ecu_config_sensdata_relation_input_select_t axis_y;
    float table[SENSDATA_RELATION_INPUT_VARIANTED_ITEMS_MAX][SENSDATA_RELATION_INPUT_VARIANTED_ITEMS_MAX];
}ecu_config_tables_table_2d_t;

typedef struct {
    ecu_config_tables_table_2d_t relative_filling;

    ecu_config_tables_table_2d_t ignition_advance;

    ecu_config_tables_table_2d_t air_fuel_ratio;
    ecu_config_tables_table_2d_t injection_phase;

    uint32_t align ALIGNED_CACHE;
}ecu_config_tables_v1_t ALIGNED_CACHE;

typedef ecu_config_tables_v1_t ecu_config_tables_t;

#endif /* CONFIG_VERSIONED_CALIBRATION_INC_VERSIONED_TABLES_H_ */
