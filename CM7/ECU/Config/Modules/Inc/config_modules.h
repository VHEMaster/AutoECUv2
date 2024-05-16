/*
 * config_modules.h
 *
 *  Created on: May 16, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_MODULES_INC_CONFIG_MODULES_H_
#define CONFIG_MODULES_INC_CONFIG_MODULES_H_


#include "errors.h"
#include "etc.h"

typedef uint32_t ecu_module_instance_t;

typedef enum {
  ECU_MODULE_TYPE_ETC = 0,
  ECU_MODULE_TYPE_MAX
}ecu_module_type_t;

typedef void (*ecu_module_loop_func_t)(void *ctx);

typedef enum {
  ECU_MODULE_ETC_1 = 0,
  ECU_MODULE_ETC_2,
  ECU_MODULE_ETC_MAX
}ecu_module_etc_t;

error_t ecu_modules_init(void);
void ecu_modules_loop_main(void);
void ecu_modules_loop_slow(void);
void ecu_modules_loop_fast(void);

error_t ecu_modules_get_module_ctx(ecu_module_type_t type, ecu_module_instance_t instance, void **ctx);
error_t ecu_modules_set_module_initialized(ecu_module_type_t type, ecu_module_instance_t instance, bool initialized);

error_t ecu_modules_get_etc_ctx(ecu_module_etc_t instance, etc_ctx_t **ctx);

#endif /* CONFIG_MODULES_INC_CONFIG_MODULES_H_ */
