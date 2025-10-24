/*
 * config_engine.h
 *
 *  Created on: May 3, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_GLOBAL_INC_CONFIG_ENGINE_H_
#define CONFIG_GLOBAL_INC_CONFIG_ENGINE_H_

#include "versioned_calibration.h"
#include "versioned_runtime.h"
#include "versioned_devices.h"
#include "versioned_modules.h"
#include "versioned_comm.h"

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
    ots_config_t ots[ECU_SENSOR_OTS_MAX];
    ops_config_t ops[ECU_SENSOR_OPS_MAX];
}ecu_config_engine_sens_t;

typedef struct {
    timing_config_t timing[ECU_MODULE_TIMING_MAX];
    etc_config_t etc[ECU_MODULE_ETC_MAX];
    vvt_config_t vvt[ECU_MODULE_VVT_MAX];
    fuelpump_config_t fuelpump[ECU_MODULE_FUELPUMP_MAX];
    coolingfan_config_t coolingfan[ECU_MODULE_COOLINGFAN_MAX];
    ignpower_config_t ignpower[ECU_MODULE_IGNPOWER_MAX];
    indication_config_t indication[ECU_MODULE_INDICATION_MAX];
    wgcv_config_t wgcv[ECU_MODULE_WGCV_MAX];
}ecu_config_engine_modules_t;

typedef struct {
    ecu_config_data_identification_t id;
    ecu_config_io_t io;
    ecu_config_powermoding_t powermoding;
    ecu_config_cylinders_t cylinders;
    ecu_config_injection_t injection;
    ecu_config_ignition_t ignition;
    ecu_config_calcdata_t calcdata;
    ecu_config_tables_t tables;

}ecu_config_engine_calibration_t;

typedef struct {
    ecu_config_corrections_t corrections;

}ecu_config_engine_runtime_t;

typedef struct {
    router_config_t router[ECU_COMM_ROUTER_MAX];
    can_config_t can[ECU_COMM_CAN_MAX];
    isotp_config_t kwp[ECU_COMM_KWP_MAX];
    isotp_config_t isotp[ECU_COMM_ISOTP_MAX];
    isotp_config_t uds[ECU_COMM_UDS_MAX];
    isotp_config_t obd2[ECU_COMM_OBD2_MAX];

}ecu_config_engine_comm_t;

typedef struct {
    ecu_config_engine_devs_t devs;
    ecu_config_engine_sens_t sens;
    ecu_config_engine_modules_t modules;
    ecu_config_engine_calibration_t calibration;
    ecu_config_engine_runtime_t runtime;
    ecu_config_engine_comm_t comm;
}ecu_config_global_engine_t;

typedef struct {
    ecu_config_engine_runtime_t runtime;
}ecu_config_global_engine_double_t;

error_t ecu_config_global_get_global_engine_config(const ecu_config_global_engine_t **config);
error_t ecu_config_global_get_engine_calibration_config(const ecu_config_engine_calibration_t **config);
error_t ecu_config_global_get_engine_runtime_config(ecu_config_engine_runtime_t **config);

#endif /* CONFIG_GLOBAL_INC_CONFIG_ENGINE_H_ */
