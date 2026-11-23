/*
 * versioned_id.h
 *
 *  Created on: May 3, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_VERSIONED_SOFTWARE_INC_VERSIONED_ID_H_
#define CONFIG_VERSIONED_SOFTWARE_INC_VERSIONED_ID_H_

#include "common.h"

typedef enum {
  ECU_CONFIG_DATA_ID_VERSION_V1 = 0,
  ECU_CONFIG_DATA_ID_VERSION_MAX
}ecu_config_data_identification_versions_t;

#define ECU_CONFIG_ID_VIN_LENGTH        (17)
#define ECU_CONFIG_ID_VIN_MAX           (ECU_CONFIG_ID_VIN_LENGTH + 1)

#define ECU_CONFIG_ID_ECU_TYPE_LENGTH   (3)
#define ECU_CONFIG_ID_ECU_TYPE_MAX      (ECU_CONFIG_ID_ECU_TYPE_LENGTH + 1)

#define ECU_CONFIG_ID_ECU_NAME_LENGTH   (16)
#define ECU_CONFIG_ID_ECU_NAME_MAX      (ECU_CONFIG_ID_ECU_NAME_LENGTH + 1)

#define ECU_CONFIG_ID_CALIB_ID_LENGTH   (16)
#define ECU_CONFIG_ID_CALIB_ID_MAX      (ECU_CONFIG_ID_CALIB_ID_LENGTH + 1)

typedef struct {
    char vin[ECU_CONFIG_ID_VIN_MAX];
    char ecu_type[ECU_CONFIG_ID_ECU_TYPE_MAX];
    char ecu_name[ECU_CONFIG_ID_ECU_NAME_MAX];
    char calibration_id[ECU_CONFIG_ID_CALIB_ID_MAX];

    uint32_t align ALIGNED_CACHE;
}ecu_config_data_identification_v1_t ALIGNED_CACHE;

typedef ecu_config_data_identification_v1_t ecu_config_data_identification_t;

#endif /* CONFIG_VERSIONED_SOFTWARE_INC_VERSIONED_ID_H_ */
