/*
 * config_modules.h
 *
 *  Created on: May 16, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_MODULES_INC_CONFIG_MODULES_H_
#define CONFIG_MODULES_INC_CONFIG_MODULES_H_


#include "errors.h"
#include "timing.h"
#include "etc.h"
#include "vvt.h"
#include "fuelpump.h"
#include "coolingfan.h"
#include "ignpower.h"
#include "indication.h"
#include "config_modules_types.h"

typedef void (*ecu_module_loop_func_t)(void *ctx);

error_t ecu_modules_init(void);
void ecu_modules_loop_main(void);
void ecu_modules_loop_slow(void);
void ecu_modules_loop_fast(void);

error_t ecu_modules_get_module_ctx(ecu_module_type_t type, ecu_module_instance_t instance, void **ctx);
error_t ecu_modules_set_module_initialized(ecu_module_type_t type, ecu_module_instance_t instance, bool initialized);

error_t ecu_modules_get_timing_ctx(ecu_module_timing_t instance, timing_ctx_t **ctx);
error_t ecu_modules_get_etc_ctx(ecu_module_etc_t instance, etc_ctx_t **ctx);
error_t ecu_modules_get_vvt_ctx(ecu_module_vvt_t instance, vvt_ctx_t **ctx);
error_t ecu_modules_get_fuelpump_ctx(ecu_module_fuelpump_t instance, fuelpump_ctx_t **ctx);
error_t ecu_modules_get_coolingfan_ctx(ecu_module_coolingfan_t instance, coolingfan_ctx_t **ctx);
error_t ecu_modules_get_ignpower_ctx(ecu_module_ignpower_t instance, ignpower_ctx_t **ctx);
error_t ecu_modules_get_indication_ctx(ecu_module_indication_t instance, indication_ctx_t **ctx);

#endif /* CONFIG_MODULES_INC_CONFIG_MODULES_H_ */
