/*
 * config_core.h
 *
 *  Created on: May 16, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_CORE_INC_CONFIG_CORE_H_
#define CONFIG_CORE_INC_CONFIG_CORE_H_


#include "errors.h"
#include "cylinders.h"
#include "timing.h"
#include "etc.h"

typedef uint32_t ecu_core_component_instance_t;

typedef enum {
  ECU_CORE_COMPONENT_TYPE_CYLINDERS = 0,
  ECU_CORE_COMPONENT_TYPE_TIMING,
  ECU_CORE_COMPONENT_TYPE_ETC,
  ECU_CORE_COMPONENT_TYPE_MAX
}ecu_core_component_type_t;

typedef void (*ecu_core_component_loop_func_t)(void *ctx);

typedef enum {
  ECU_CORE_COMPONENT_CYLINDERS_1 = 0,
  ECU_CORE_COMPONENT_CYLINDERS_MAX
}ecu_core_component_cylinders_t;

error_t ecu_core_components_init(void);
void ecu_core_components_loop_main(void);
void ecu_core_components_loop_slow(void);
void ecu_core_components_loop_fast(void);

error_t ecu_core_components_get_ctx(ecu_core_component_type_t type, ecu_core_component_instance_t instance, void **ctx);
error_t ecu_core_components_set_initialized(ecu_core_component_type_t type, ecu_core_component_instance_t instance, bool initialized);

error_t ecu_core_components_get_cylinders_ctx(ecu_core_component_cylinders_t instance, cylinders_ctx_t **ctx);

#endif /* CONFIG_CORE_COMPONENTS_INC_CONFIG_CORE_COMPONENTS_H_ */
