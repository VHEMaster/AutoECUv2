/*
 * config_engine.h
 *
 *  Created on: May 3, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_GLOBAL_INC_CONFIG_ENGINE_H_
#define CONFIG_GLOBAL_INC_CONFIG_ENGINE_H_

#include "versioned_sw.h"
#include "versioned_devices.h"
#include "versioned_modules.h"

typedef struct {
    l9966_config_t flexio[ECU_DEVICE_FLEXIO_MAX];
    l9960_config_t motor[ECU_DEVICE_MOTOR_MAX];
    tle4729_config_t stepper[ECU_DEVICE_STEPPER_MAX];
    cj125_config_t wbls[ECU_DEVICE_WBLS_MAX];

}ecu_config_engine_devs_t;

typedef struct {
    ckp_config_t ckp[ECU_SENSOR_CKP_MAX];
    cmp_config_t cmp[ECU_SENSOR_CMP_MAX];
    ect_config_t ect[ECU_SENSOR_ECT_MAX];
    egt_config_t egt[ECU_SENSOR_EGT_MAX];
    iat_config_t iat[ECU_SENSOR_IAT_MAX];
    maf_config_t maf[ECU_SENSOR_MAF_MAX];
    map_config_t map[ECU_SENSOR_MAP_MAX];
    tps_config_t tps[ECU_SENSOR_TPS_MAX];
    vss_config_t vss[ECU_SENSOR_VSS_MAX];
    aps_config_t aps[ECU_SENSOR_APS_MAX];
}ecu_config_engine_sens_t;

typedef struct {
    timing_config_t timing[ECU_MODULE_TIMING_MAX];
    etc_config_t etc[ECU_MODULE_ETC_MAX];
}ecu_config_engine_modules_t;

typedef struct {
    timing_config_t cylinders[ECU_CORE_COMPONENT_CYLINDERS_MAX];
}ecu_config_engine_core_components_t;

typedef struct {
    ecu_config_data_identification_t id;

}ecu_config_engine_sw_t;

typedef struct {
    ecu_config_engine_devs_t devs;
    ecu_config_engine_sens_t sens;
    ecu_config_engine_modules_t modules;
    ecu_config_engine_core_components_t core_components;
    ecu_config_engine_sw_t sw;
}ecu_config_global_engine_t;

#endif /* CONFIG_GLOBAL_INC_CONFIG_ENGINE_H_ */
