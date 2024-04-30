/*
 * versioned.h
 *
 *  Created on: Apr 30, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_VERSIONED_INC_VERSIONED_H_
#define CONFIG_VERSIONED_INC_VERSIONED_H_

#include "versioned_flexio.h"
#include "versioned_motor.h"
#include "versioned_wbls.h"

#include "errors.h"
#include <stdint.h>
#include <stddef.h>

#define ECU_CONFIG_COMPONENT_VERSIONS_MAX     (1)

typedef enum {
  ECU_CONFIG_COMPONENT_TYPE_FLEXIO = 0,
  ECU_CONFIG_COMPONENT_TYPE_MOTOR = 1,
  ECU_CONFIG_COMPONENT_TYPE_WBLS = 2,
  ECU_CONFIG_COMPONENT_TYPE_MAX
}ecu_config_component_type_t;

typedef error_t (*ecu_config_translate_prev_to_this_func_t)(const void *src, void *dest);

typedef struct {
    uint32_t version;
    uint32_t size;
    ecu_config_translate_prev_to_this_func_t translate_func;
}ecu_config_component_version_t;

typedef struct {
    uint32_t versions_count;
    ecu_config_component_version_t versions[ECU_CONFIG_COMPONENT_VERSIONS_MAX];
}ecu_config_component_versions_t;

typedef struct {
    ecu_config_component_versions_t components[ECU_CONFIG_COMPONENT_TYPE_MAX];
}ecu_config_components_versions_t;

#endif /* CONFIG_VERSIONED_INC_VERSIONED_H_ */
