/*
 * config_sensors_types.h
 *
 *  Created on: Nov 14, 2025
 *      Author: VHEMaster
 */

#ifndef CONFIG_INC_CONFIG_SENSORS_TYPES_H_
#define CONFIG_INC_CONFIG_SENSORS_TYPES_H_

#include "common.h"

#define ECU_SENSOR_NONE               (255)

#define ECU_SENSOR_INSTANCE_MAX       (4)
#define ECU_SENSOR_READ_PARAM_DATA    (4)

typedef uint32_t ecu_sensor_instance_t;

typedef enum {
  ECU_SENSOR_TYPE_CKP = 0,
  ECU_SENSOR_TYPE_CMP,
  ECU_SENSOR_TYPE_ECT,
  ECU_SENSOR_TYPE_EGT,
  ECU_SENSOR_TYPE_IAT,
  ECU_SENSOR_TYPE_MAF,
  ECU_SENSOR_TYPE_MAP,
  ECU_SENSOR_TYPE_TPS,
  ECU_SENSOR_TYPE_VSS,
  ECU_SENSOR_TYPE_APS,
  ECU_SENSOR_TYPE_OTS,
  ECU_SENSOR_TYPE_OPS,
  ECU_SENSOR_TYPE_MAX
}ecu_sensor_type_t;

typedef void (*ecu_sensor_loop_func_t)(void *ctx);

typedef enum {
  ECU_SENSOR_CKP_1 = CKP_INSTANCE_1,
  ECU_SENSOR_CKP_MAX = CKP_INSTANCE_MAX
}ecu_sensor_ckp_t;

typedef enum {
  ECU_SENSOR_CMP_1 = CMP_INSTANCE_1,
  ECU_SENSOR_CMP_2 = CMP_INSTANCE_2,
  ECU_SENSOR_CMP_3 = CMP_INSTANCE_3,
  ECU_SENSOR_CMP_4 = CMP_INSTANCE_4,
  ECU_SENSOR_CMP_MAX = CMP_INSTANCE_MAX
}ecu_sensor_cmp_t;

typedef enum {
  ECU_SENSOR_ECT_1 = 0,
  ECU_SENSOR_ECT_2,
  ECU_SENSOR_ECT_MAX
}ecu_sensor_ect_t;

typedef enum {
  ECU_SENSOR_EGT_1 = 0,
  ECU_SENSOR_EGT_2,
  ECU_SENSOR_EGT_MAX
}ecu_sensor_egt_t;

typedef enum {
  ECU_SENSOR_IAT_1 = 0,
  ECU_SENSOR_IAT_2,
  ECU_SENSOR_IAT_MAX
}ecu_sensor_iat_t;

typedef enum {
  ECU_SENSOR_MAF_1 = 0,
  ECU_SENSOR_MAF_2,
  ECU_SENSOR_MAF_MAX
}ecu_sensor_maf_t;

typedef enum {
  ECU_SENSOR_MAP_1 = 0,
  ECU_SENSOR_MAP_2,
  ECU_SENSOR_MAP_3,
  ECU_SENSOR_MAP_4,
  ECU_SENSOR_MAP_MAX
}ecu_sensor_map_t;

typedef enum {
  ECU_SENSOR_TPS_1 = 0,
  ECU_SENSOR_TPS_2,
  ECU_SENSOR_TPS_MAX
}ecu_sensor_tps_t;

typedef enum {
  ECU_SENSOR_VSS_1 = 0,
  ECU_SENSOR_VSS_MAX
}ecu_sensor_vss_t;

typedef enum {
  ECU_SENSOR_APS_1 = 0,
  ECU_SENSOR_APS_MAX
}ecu_sensor_aps_t;

typedef enum {
  ECU_SENSOR_OTS_1 = 0,
  ECU_SENSOR_OTS_2,
  ECU_SENSOR_OTS_MAX
}ecu_sensor_ots_t;

typedef enum {
  ECU_SENSOR_OPS_1 = 0,
  ECU_SENSOR_OPS_2,
  ECU_SENSOR_OPS_MAX
}ecu_sensor_ops_t;

#endif /* CONFIG_INC_CONFIG_SENSORS_TYPES_H_ */
