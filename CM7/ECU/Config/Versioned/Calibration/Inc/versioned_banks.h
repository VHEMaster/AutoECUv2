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

typedef enum {
  ECU_CONFIG_BANKS_CMP_INTAKE = 0,
  ECU_CONFIG_BANKS_CMP_EXHAUST,
  ECU_CONFIG_BANKS_CMP_MAX
}ecu_config_banks_cmp_t;

typedef enum {
  ECU_CONFIG_BANKS_ECT_PRIMARY = 0,
  ECU_CONFIG_BANKS_ECT_SECONDARY,
  ECU_CONFIG_BANKS_ECT_MAX
}ecu_config_banks_ect_t;

typedef enum {
  ECU_CONFIG_BANKS_EGT_PRIMARY = 0,
  ECU_CONFIG_BANKS_EGT_MAX
}ecu_config_banks_egt_t;

typedef enum {
  ECU_CONFIG_BANKS_IAT_INLET = 0,
  ECU_CONFIG_BANKS_IAT_THROTTLE,
  ECU_CONFIG_BANKS_IAT_MANIFOLD,
  ECU_CONFIG_BANKS_IAT_MAX
}ecu_config_banks_iat_t;

typedef enum {
  ECU_CONFIG_BANKS_MAF_PRIMARY = 0,
  ECU_CONFIG_BANKS_MAF_MAX
}ecu_config_banks_maf_t;

typedef enum {
  ECU_CONFIG_BANKS_MAP_INLET = 0,
  ECU_CONFIG_BANKS_MAP_THROTTLE,
  ECU_CONFIG_BANKS_MAP_MANIFOLD,
  ECU_CONFIG_BANKS_MAP_MAX
}ecu_config_banks_map_t;

typedef enum {
  ECU_CONFIG_BANKS_TPS_PRIMARY = 0,
  ECU_CONFIG_BANKS_TPS_MAX
}ecu_config_banks_tps_t;

typedef enum {
  ECU_CONFIG_BANKS_VSS_PRIMARY = 0,
  ECU_CONFIG_BANKS_VSS_MAX
}ecu_config_banks_vss_t;

typedef enum {
  ECU_CONFIG_BANKS_APS_ACCELERATOR = 0,
  ECU_CONFIG_BANKS_APS_MAX
}ecu_config_banks_aps_t;

typedef enum {
  ECU_CONFIG_BANKS_ETC_PRIMARY = 0,
  ECU_CONFIG_BANKS_ETC_MAX
}ecu_config_banks_etc_t;

typedef struct {
    ecu_sensor_cmp_t sensor_cmp[ECU_CONFIG_BANKS_CMP_MAX];
    ecu_sensor_ect_t sensor_ect[ECU_CONFIG_BANKS_ECT_MAX];
    ecu_sensor_ect_t sensor_egt[ECU_CONFIG_BANKS_EGT_MAX];
    ecu_sensor_iat_t sensor_iat[ECU_CONFIG_BANKS_IAT_MAX];
    ecu_sensor_maf_t sensor_maf[ECU_CONFIG_BANKS_MAF_MAX];
    ecu_sensor_map_t sensor_map[ECU_CONFIG_BANKS_MAP_MAX];
    ecu_sensor_vss_t sensor_tps[ECU_CONFIG_BANKS_TPS_MAX];
    ecu_sensor_vss_t sensor_vss[ECU_CONFIG_BANKS_VSS_MAX];
    ecu_sensor_aps_t sensor_aps[ECU_CONFIG_BANKS_APS_MAX];

    ecu_module_etc_t module_etc[ECU_CONFIG_BANKS_ETC_MAX];

    uint32_t align ALIGNED_CACHE;
}ecu_config_banks_v1_t ALIGNED_CACHE;

typedef ecu_config_banks_v1_t ecu_config_banks_t;

#endif /* CONFIG_VERSIONED_CORE_INC_VERSIONED_BANKS_H_ */
