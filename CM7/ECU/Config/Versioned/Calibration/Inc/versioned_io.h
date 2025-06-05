/*
 * versioned_io.h
 *
 *  Created on: Jun 6, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_VERSIONED_CORE_INC_VERSIONED_IO_H_
#define CONFIG_VERSIONED_CORE_INC_VERSIONED_IO_H_

#include "common.h"
#include "config_sensors.h"
#include "config_devices.h"
#include "config_modules.h"

typedef uint32_t ecu_config_io_instance_t;

typedef enum {
  ECU_CONFIG_IO_VERSION_V1 = 0,
  ECU_CONFIG_IO_VERSION_MAX
}ecu_config_io_versions_t;

typedef enum {
  ECU_CONFIG_IO_CKP_PRIMARY = 0,
  ECU_CONFIG_IO_CKP_MAX
}ecu_config_io_ckp_t;

typedef enum {
  ECU_CONFIG_IO_CMP_SOHC = 0,
  ECU_CONFIG_IO_CMP_DOHC_INTAKE,
  ECU_CONFIG_IO_CMP_DOHC_EXHAUST,
  ECU_CONFIG_IO_CMP_MAX
}ecu_config_io_cmp_t;

typedef enum {
  ECU_CONFIG_IO_ECT_PRIMARY = 0,
  ECU_CONFIG_IO_ECT_SECONDARY,
  ECU_CONFIG_IO_ECT_MAX
}ecu_config_io_ect_t;

typedef enum {
  ECU_CONFIG_IO_EGT_PRIMARY = 0,
  ECU_CONFIG_IO_EGT_MAX
}ecu_config_io_egt_t;

typedef enum {
  ECU_CONFIG_IO_IAT_INLET = 0,
  ECU_CONFIG_IO_IAT_THROTTLE,
  ECU_CONFIG_IO_IAT_MANIFOLD,
  ECU_CONFIG_IO_IAT_MAX
}ecu_config_io_iat_t;

typedef enum {
  ECU_CONFIG_IO_MAF_PRIMARY = 0,
  ECU_CONFIG_IO_MAF_MAX
}ecu_config_io_maf_t;

typedef enum {
  ECU_CONFIG_IO_MAP_INLET = 0,
  ECU_CONFIG_IO_MAP_THROTTLE,
  ECU_CONFIG_IO_MAP_MANIFOLD,
  ECU_CONFIG_IO_MAP_MAX
}ecu_config_io_map_t;

typedef enum {
  ECU_CONFIG_IO_TPS_PRIMARY = 0,
  ECU_CONFIG_IO_TPS_MAX
}ecu_config_io_tps_t;

typedef enum {
  ECU_CONFIG_IO_VSS_PRIMARY = 0,
  ECU_CONFIG_IO_VSS_MAX
}ecu_config_io_vss_t;

typedef enum {
  ECU_CONFIG_IO_APS_ACCELERATOR = 0,
  ECU_CONFIG_IO_APS_MAX
}ecu_config_io_aps_t;

typedef enum {
  ECU_CONFIG_IO_OTS_PRIMARY = 0,
  ECU_CONFIG_IO_OTS_MAX
}ecu_config_io_ots_t;

typedef enum {
  ECU_CONFIG_IO_OPS_PRIMARY = 0,
  ECU_CONFIG_IO_OPS_MAX
}ecu_config_io_ops_t;

typedef enum {
  ECU_CONFIG_IO_STEPPER_IDLEVALVE = 0,
  ECU_CONFIG_IO_STEPPER_MAX
}ecu_config_io_stepper_t;

typedef enum {
  ECU_CONFIG_IO_WBLS_PRIMARY = 0,
  ECU_CONFIG_IO_WBLS_MAX
}ecu_config_io_wbls_t;

typedef enum {
  ECU_CONFIG_IO_ETC_PRIMARY = 0,
  ECU_CONFIG_IO_ETC_MAX
}ecu_config_io_etc_t;

typedef enum {
  ECU_CONFIG_IO_VVT_SOHC = 0,
  ECU_CONFIG_IO_VVT_DOHC_INTAKE,
  ECU_CONFIG_IO_VVT_DOHC_EXHAUST,
  ECU_CONFIG_IO_VVT_MAX
}ecu_config_io_vvt_t;

typedef enum {
  ECU_CONFIG_IO_WGCV_PRIMARY = 0,
  ECU_CONFIG_IO_WGCV_MAX
}ecu_config_io_wgcv_t;

typedef enum {
  ECU_CONFIG_IO_FUELPUMP_PRIMARY = 0,
  ECU_CONFIG_IO_FUELPUMP_MAX
}ecu_config_io_fuelpump_t;

typedef enum {
  ECU_CONFIG_IO_COOLINGFAN_PRIMARY = 0,
  ECU_CONFIG_IO_COOLINGFAN_SECONDARY,
  ECU_CONFIG_IO_COOLINGFAN_MAX
}ecu_config_io_coolingfan_t;

typedef enum {
  ECU_CONFIG_IO_IGNPOWER_PRIMARY = 0,
  ECU_CONFIG_IO_IGNPOWER_MAX
}ecu_config_io_ignpower_t;

typedef enum {
  ECU_CONFIG_IO_INDICATION_CHECKENGINE = 0,
  ECU_CONFIG_IO_INDICATION_SHIFT_GREEN,
  ECU_CONFIG_IO_INDICATION_SHIFT_RED,
  ECU_CONFIG_IO_INDICATION_CUTOFF,
  ECU_CONFIG_IO_INDICATION_RSVD1,
  ECU_CONFIG_IO_INDICATION_RSVD2,
  ECU_CONFIG_IO_INDICATION_MAX
}ecu_config_io_indication_t;

typedef enum {
  ECU_CONFIG_IO_TIMING_PRIMARY = 0,
  ECU_CONFIG_IO_TIMING_MAX
}ecu_config_io_timing_t;

typedef struct {


}ecu_config_io_misc_t;

typedef struct {
    ecu_config_io_cmp_t sensor_cmp[ECU_CONFIG_IO_CMP_MAX];
    ecu_config_io_ect_t sensor_egt[ECU_CONFIG_IO_EGT_MAX];
    ecu_config_io_iat_t sensor_iat[ECU_CONFIG_IO_IAT_MAX];
    ecu_config_io_maf_t sensor_maf[ECU_CONFIG_IO_MAF_MAX];
    ecu_config_io_map_t sensor_map[ECU_CONFIG_IO_MAP_MAX];
    ecu_config_io_vss_t sensor_tps[ECU_CONFIG_IO_TPS_MAX];


    ecu_config_io_stepper_t device_stepper[ECU_CONFIG_IO_STEPPER_MAX];
    ecu_config_io_wbls_t device_wbls[ECU_CONFIG_IO_WBLS_MAX];

    ecu_config_io_etc_t module_etc[ECU_CONFIG_IO_ETC_MAX];
    ecu_config_io_vvt_t module_vvt[ECU_CONFIG_IO_VVT_MAX];
    ecu_config_io_wgcv_t module_wgcv[ECU_CONFIG_IO_WGCV_MAX];

}ecu_config_io_common_t;

typedef struct {
    ecu_config_io_cmp_t sensor_ckp[ECU_CONFIG_IO_CKP_MAX];
    ecu_config_io_ect_t sensor_ect[ECU_CONFIG_IO_ECT_MAX];
    ecu_config_io_vss_t sensor_vss[ECU_CONFIG_IO_VSS_MAX];
    ecu_config_io_aps_t sensor_aps[ECU_CONFIG_IO_APS_MAX];
    ecu_config_io_ots_t sensor_ots[ECU_CONFIG_IO_OTS_MAX];
    ecu_config_io_ops_t sensor_ops[ECU_CONFIG_IO_OPS_MAX];

    ecu_config_io_fuelpump_t module_fuelpump[ECU_CONFIG_IO_FUELPUMP_MAX];
    ecu_config_io_coolingfan_t module_coolingfan[ECU_CONFIG_IO_COOLINGFAN_MAX];
    ecu_config_io_ignpower_t module_ignpower[ECU_CONFIG_IO_IGNPOWER_MAX];
    ecu_config_io_indication_t module_indication[ECU_CONFIG_IO_INDICATION_MAX];
    ecu_config_io_timing_t module_timing[ECU_CONFIG_IO_TIMING_MAX];

    ecu_config_io_common_t common;

}ecu_config_io_global_t;

typedef struct {
    ecu_config_io_common_t common;
}ecu_config_io_bank_t;

typedef struct {
    ecu_config_io_global_t global;
    ecu_config_io_bank_t banks[ECU_BANK_MAX];
}ecu_config_io_banked_t;

typedef struct {
    ecu_config_io_misc_t misc;

    ecu_config_io_banked_t banked;



    uint32_t align ALIGNED_CACHE;
}ecu_config_io_v1_t ALIGNED_CACHE;

typedef ecu_config_io_v1_t ecu_config_io_t;

#endif /* CONFIG_VERSIONED_CORE_INC_VERSIONED_IO_H_ */
