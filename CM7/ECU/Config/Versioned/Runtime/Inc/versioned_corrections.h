/*
 * versioned_corrections.h
 *
 *  Created on: Jun 6, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_VERSIONED_RUNTIME_INC_VERSIONED_CORRECTIONS_H_
#define CONFIG_VERSIONED_RUNTIME_INC_VERSIONED_CORRECTIONS_H_

#include "common.h"

typedef enum {
  CORRECTIONS_CONFIG_VERSION_V1 = 0,
  CORRECTIONS_CONFIG_VERSION_MAX
}ecu_config_corrections_versions_t;

typedef struct {

}ecu_config_corrections_global_t;

typedef struct {

}ecu_config_corrections_bank_t;

typedef struct {

}ecu_config_corrections_cylinder_t;

typedef struct {
    ecu_config_corrections_global_t global;
    ecu_config_corrections_bank_t bank[ECU_BANK_MAX];
    ecu_config_corrections_cylinder_t cylinder[ECU_CYLINDER_MAX];

    uint32_t align ALIGNED_CACHE;
}ecu_config_corrections_v1_t ALIGNED_CACHE;

typedef ecu_config_corrections_v1_t ecu_config_corrections_t;

#endif /* CONFIG_VERSIONED_RUNTIME_INC_VERSIONED_CORRECTIONS_H_ */
