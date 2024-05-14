/*
 * config_sensors.h
 *
 *  Created on: May 14, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_INC_CONFIG_SENSORS_H_
#define CONFIG_INC_CONFIG_SENSORS_H_

#include "errors.h"
#include "ckp.h"
#include "cmp.h"
#include "ect.h"
#include "egt.h"
#include "iat.h"
#include "maf.h"
#include "map.h"
#include "tps.h"
#include "aps.h"

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
  ECU_SENSOR_TYPE_APS,
  ECU_SENSOR_TYPE_MAX
}ecu_sensor_type_t;

typedef void (*ecu_sensor_loop_func_t)(void *ctx);

typedef enum {
  ECU_SENSOR_CKP_1 = 0,
  ECU_SENSOR_CKP_MAX
}ecu_sensor_ckp_t;

typedef enum {
  ECU_SENSOR_CMP_1 = 0,
  ECU_SENSOR_CMP_2,
  ECU_SENSOR_CMP_3,
  ECU_SENSOR_CMP_4,
  ECU_SENSOR_CMP_MAX
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
  ECU_SENSOR_APS_1 = 0,
  ECU_SENSOR_APS_MAX
}ecu_sensor_aps_t;

error_t ecu_sensors_init(void);
void ecu_sensors_loop_main(void);
void ecu_sensors_loop_slow(void);
void ecu_sensors_loop_fast(void);

error_t ecu_sensors_get_sensor_ctx(ecu_sensor_type_t type, ecu_sensor_instance_t instance, void **ctx);
error_t ecu_sensors_set_sensor_initialized(ecu_sensor_type_t type, ecu_sensor_instance_t instance, bool initialized);

error_t ecu_sensors_get_ckp_ctx(ecu_sensor_ckp_t instance, ckp_ctx_t **ctx);
error_t ecu_sensors_get_cmp_ctx(ecu_sensor_cmp_t instance, cmp_ctx_t **ctx);
error_t ecu_sensors_get_ect_ctx(ecu_sensor_ect_t instance, ect_ctx_t **ctx);
error_t ecu_sensors_get_egt_ctx(ecu_sensor_egt_t instance, egt_ctx_t **ctx);
error_t ecu_sensors_get_iat_ctx(ecu_sensor_iat_t instance, iat_ctx_t **ctx);
error_t ecu_sensors_get_maf_ctx(ecu_sensor_maf_t instance, maf_ctx_t **ctx);
error_t ecu_sensors_get_map_ctx(ecu_sensor_map_t instance, map_ctx_t **ctx);
error_t ecu_sensors_get_tps_ctx(ecu_sensor_tps_t instance, tps_ctx_t **ctx);
error_t ecu_sensors_get_aps_ctx(ecu_sensor_aps_t instance, aps_ctx_t **ctx);

#endif /* CONFIG_INC_CONFIG_SENSORS_H_ */
