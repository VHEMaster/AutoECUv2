/*
 * config_common_types.h
 *
 *  Created on: Nov 16, 2025
 *      Author: VHEMaster
 */

#ifndef CONFIG_COMMON_INC_CONFIG_COMMON_TYPES_H_
#define CONFIG_COMMON_INC_CONFIG_COMMON_TYPES_H_

#include "common.h"

#define ECU_ENTITY_NONE           (255)

#define ECU_ENTITY_TYPE_MAX       (16)
#define ECU_ENTITY_INSTANCE_MAX   (8)

#define ECU_COMMON_READ_WRITE_MAX (2)

typedef enum {
  ECU_COMMON_READ = 0,
  ECU_COMMON_WRITE
}ecu_config_common_read_write_t;

typedef uint8_t ecu_config_common_entity_type_t;
typedef uint32_t ecu_config_common_entity_instance_t;
typedef uint32_t ecu_config_common_entity_parameter_t;

typedef enum {
  ECU_COMMON_OVERALL_SOURCE_SIMULATED = 0,
  ECU_COMMON_OVERALL_SOURCE_DIAG,
  ECU_COMMON_OVERALL_SOURCE_EXTERNAL,
  ECU_COMMON_OVERALL_SOURCE_INTERNAL,
  ECU_COMMON_OVERALL_SOURCE_FINAL,
  ECU_COMMON_OVERALL_SOURCE_MAX,
}ecu_config_common_overall_source_t;

typedef enum {
  ECU_COMMON_ENTITY_DEVICE = 0,
  ECU_COMMON_ENTITY_SENSOR,
  ECU_COMMON_ENTITY_MODULE,
  ECU_COMMON_ENTITY_TIMING,
  ECU_COMMON_ENTITY_MAX
}ecu_config_common_entity_t;

typedef struct {
    ecu_runtime_param_index_t params_count;
    uint32_t param_first_index;
}ecu_config_common_shareable_layout_readwrite_ctx_t;

typedef struct {
    ecu_config_common_shareable_layout_readwrite_ctx_t readwrite[ECU_COMMON_READ_WRITE_MAX];
}ecu_config_common_shareable_layout_instance_ctx_t;

typedef struct {
    ecu_config_common_entity_instance_t instance_limit;
    ecu_config_common_entity_instance_t instance_count;
    ecu_config_common_shareable_layout_instance_ctx_t instances[ECU_ENTITY_INSTANCE_MAX];
}ecu_config_common_shareable_layout_type_ctx_t;

typedef struct {
    ecu_config_common_entity_type_t type_limit;
    ecu_config_common_entity_type_t type_count;
    ecu_config_common_shareable_layout_type_ctx_t types[ECU_ENTITY_TYPE_MAX];
}ecu_config_common_shareable_layout_entity_ctx_t;

typedef struct {
    ecu_config_common_entity_t entity_limit;
    ecu_config_common_entity_t entity_count;
    ecu_config_common_shareable_layout_entity_ctx_t entities[ECU_COMMON_ENTITY_MAX];
}ecu_config_common_shareable_layout_source_ctx_t;

typedef struct {
    ecu_config_common_overall_source_t sources_limit;
    ecu_config_common_overall_source_t sources_count;
    ecu_config_common_shareable_layout_source_ctx_t source;
}ecu_config_common_shareable_layout_ctx_t;

typedef union {
  uint32_t id;
  struct {
    ecu_config_common_entity_parameter_t parameter : 8;
    ecu_config_common_entity_instance_t instance : 8;
    ecu_config_common_entity_type_t type : 8;
    ecu_config_common_read_write_t read_write : 1;
    bool supported : 1;
    ecu_config_common_entity_t entity : 6;
  }bitfield;
}ecu_config_parameter_id_t;

#endif /* CONFIG_COMMON_INC_CONFIG_COMMON_TYPES_H_ */
