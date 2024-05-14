/*
 * versioned_ckp.h
 *
 *  Created on: May 14, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_VERSIONED_SENSORS_INC_VERSIONED_CKP_H_
#define CONFIG_VERSIONED_SENSORS_INC_VERSIONED_CKP_H_

#include "compiler.h"
#include <stdint.h>

typedef enum {
  CKP_CONFIG_VERSION_V1 = 0,
  CKP_CONFIG_VERSION_MAX
}ckp_config_versions_t;

typedef struct {

    uint32_t align ALIGNED_CACHE;
}ckp_config_v1_t ALIGNED_CACHE;

typedef ckp_config_v1_t ckp_config_t;

#endif /* CONFIG_VERSIONED_SENSORS_INC_VERSIONED_CKP_H_ */
