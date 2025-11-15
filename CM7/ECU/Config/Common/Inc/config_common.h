/*
 * config_common.h
 *
 *  Created on: Nov 9, 2025
 *      Author: VHEMaster
 */

#ifndef CONFIG_COMMON_INC_CONFIG_COMMON_H_
#define CONFIG_COMMON_INC_CONFIG_COMMON_H_

#include "common.h"
#include "core.h"

#define ECU_ENTITY_NONE           (255)

#define ECU_ENTITY_TYPE_MAX       (16)
#define ECU_ENTITY_INSTANCE_MAX   (8)
#define ECU_ENTITY_PARAMETERS_MAX (8)

typedef uint8_t ecu_config_common_entity_type_t;
typedef uint32_t ecu_config_common_entity_instance_t;
typedef uint32_t ecu_config_common_entity_parameter_t;

typedef enum {
  ECU_COMMON_ENTITY_DEVICE = 0,
  ECU_COMMON_ENTITY_SENSOR,
  ECU_COMMON_ENTITY_MODULE,
  ECU_COMMON_ENTITY_TIMING,
  ECU_COMMON_ENTITY_MAX
}ecu_config_common_entity_t;

error_t ecu_config_common_init(ecu_core_ctx_t *ctx);

error_t ecu_config_common_get_entity_max(ecu_config_common_entity_t *entity_max);
error_t ecu_config_common_get_entity_type_max(ecu_config_common_entity_t entity, ecu_config_common_entity_type_t *type_max);
error_t ecu_config_common_get_entity_type_instance_max(ecu_config_common_entity_t entity, ecu_config_common_entity_type_t type, ecu_config_common_entity_instance_t *instance_max);

#endif /* CONFIG_COMMON_INC_CONFIG_COMMON_H_ */
