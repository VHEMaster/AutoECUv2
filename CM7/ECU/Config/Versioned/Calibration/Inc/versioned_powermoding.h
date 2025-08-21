/*
 * versioned_powermoding.h
 *
 *  Created on: Aug 18, 2025
 *      Author: VHEMaster
 */

#ifndef CONFIG_VERSIONED_SOFTWARE_INC_VERSIONED_POWERMODING_H_
#define CONFIG_VERSIONED_SOFTWARE_INC_VERSIONED_POWERMODING_H_

#include "common.h"
#include "time.h"

typedef enum {
  ECU_CONFIG_POWERMODING_VERSION_V1 = 0,
  ECU_CONFIG_POWERMODING_VERSION_MAX
}ecu_config_powermoding_versions_t;

typedef enum {
  ECU_CONFIG_POWERMODING_MODE_INITIAL_STANDBY = 0,
  ECU_CONFIG_POWERMODING_MODE_INITIAL_SLEEP,
  ECU_CONFIG_POWERMODING_MODE_INITIAL_COLDBOOT,
  ECU_CONFIG_POWERMODING_MODE_INITIAL_MAX
}ecu_config_powermoding_mode_initial_t;

typedef struct {

    uint32_t align ALIGNED_CACHE;
}ecu_config_powermoding_v1_t ALIGNED_CACHE;

typedef ecu_config_powermoding_v1_t ecu_config_powermoding_t;

#endif /* CONFIG_VERSIONED_SOFTWARE_INC_VERSIONED_POWERMODING_H_ */
