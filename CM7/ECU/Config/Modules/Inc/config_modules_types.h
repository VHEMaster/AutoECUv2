/*
 * config_modules_types.h
 *
 *  Created on: Jun 24, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_MODULES_INC_CONFIG_MODULES_TYPES_H_
#define CONFIG_MODULES_INC_CONFIG_MODULES_TYPES_H_

#include "common.h"

#define ECU_MODULE_NONE     (-1)

typedef uint32_t ecu_module_instance_t;

typedef enum {
  ECU_MODULE_TYPE_TIMING = 0,
  ECU_MODULE_TYPE_ETC,
  ECU_MODULE_TYPE_VVT,
  ECU_MODULE_TYPE_FUELPUMP,
  ECU_MODULE_TYPE_COOLINGFAN,
  ECU_MODULE_TYPE_IGNPOWER,
  ECU_MODULE_TYPE_INDICATION,
  ECU_MODULE_TYPE_WGCV,
  ECU_MODULE_TYPE_MAX
}ecu_module_type_t;

typedef enum {
  ECU_MODULE_TIMING_1 = 0,
  ECU_MODULE_TIMING_MAX
}ecu_module_timing_t;

typedef enum {
  ECU_MODULE_ETC_1 = 0,
  ECU_MODULE_ETC_2,
  ECU_MODULE_ETC_MAX
}ecu_module_etc_t;

typedef enum {
  ECU_MODULE_VVT_1 = 0,
  ECU_MODULE_VVT_2,
  ECU_MODULE_VVT_3,
  ECU_MODULE_VVT_4,
  ECU_MODULE_VVT_MAX
}ecu_module_vvt_t;

typedef enum {
  ECU_MODULE_FUELPUMP_1 = 0,
  ECU_MODULE_FUELPUMP_2,
  ECU_MODULE_FUELPUMP_MAX
}ecu_module_fuelpump_t;

typedef enum {
  ECU_MODULE_COOLINGFAN_1 = 0,
  ECU_MODULE_COOLINGFAN_2,
  ECU_MODULE_COOLINGFAN_MAX
}ecu_module_coolingfan_t;

typedef enum {
  ECU_MODULE_IGNPOWER_1 = 0,
  ECU_MODULE_IGNPOWER_MAX
}ecu_module_ignpower_t;

typedef enum {
  ECU_MODULE_INDICATION_1 = 0,
  ECU_MODULE_INDICATION_2,
  ECU_MODULE_INDICATION_3,
  ECU_MODULE_INDICATION_4,
  ECU_MODULE_INDICATION_5,
  ECU_MODULE_INDICATION_6,
  ECU_MODULE_INDICATION_MAX
}ecu_module_indication_t;

typedef enum {
  ECU_MODULE_WGCV_1 = 0,
  ECU_MODULE_WGCV_2,
  ECU_MODULE_WGCV_MAX
}ecu_module_wgcv_t;

#endif /* CONFIG_MODULES_INC_CONFIG_MODULES_TYPES_H_ */
