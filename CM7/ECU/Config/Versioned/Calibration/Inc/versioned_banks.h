/*
 * versioned_id.h
 *
 *  Created on: May 3, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_VERSIONED_CORE_INC_VERSIONED_BANKS_H_
#define CONFIG_VERSIONED_CORE_INC_VERSIONED_BANKS_H_

#include "common.h"
#include "config_sensors.h"

typedef enum {
  ECU_CONFIG_BANKS_VERSION_V1 = 0,
  ECU_CONFIG_BANKS_VERSION_MAX
}ecu_config_banks_versions_t;

typedef struct {
    ecu_sensor_ckp_t sensor_crankshaft;
    ecu_sensor_cmp_t sensor_camshaft_intake;
    ecu_sensor_cmp_t sensor_camshaft_exhaust;

    //ecu_sensor_ect_t sensor_ect_;

    uint32_t align ALIGNED_CACHE;
}ecu_config_banks_v1_t ALIGNED_CACHE;

typedef ecu_config_banks_v1_t ecu_config_banks_t;

#endif /* CONFIG_VERSIONED_CORE_INC_VERSIONED_BANKS_H_ */
