/*
 * config_timings.h
 *
 *  Created on: Nov 13, 2025
 *      Author: VHEMaster
 */

#ifndef CONFIG_TIMINGS_INC_CONFIG_TIMINGS_H_
#define CONFIG_TIMINGS_INC_CONFIG_TIMINGS_H_

#include "errors.h"
#include "timing_base.h"
#include "timing_ignition.h"
#include "timing_injection.h"
#include "timing_rough.h"

#include "config_timings_types.h"

typedef void (*ecu_timing_loop_func_t)(void *ctx);

error_t ecu_timings_init(void);
void ecu_timings_loop_main(void);
void ecu_timings_loop_slow(void);
void ecu_timings_loop_fast(void);

error_t ecu_timings_get_timing_ctx(ecu_timing_type_t type, ecu_timing_instance_t instance, void **ctx);
error_t ecu_timings_set_timing_initialized(ecu_timing_type_t type, ecu_timing_instance_t instance, bool initialized);
error_t ecu_timings_get_timing_initialized(ecu_timing_type_t type, ecu_timing_instance_t instance, bool *initialized);
error_t ecu_timings_set_timing_enabled(ecu_timing_type_t type, ecu_timing_instance_t instance, bool enabled);
error_t ecu_timings_get_timing_enabled(ecu_timing_type_t type, ecu_timing_instance_t instance, bool *enabled);

error_t ecu_timings_get_type_max(ecu_timing_type_t *type_max);
error_t ecu_timings_get_instance_max(ecu_timing_type_t type, ecu_timing_instance_t *instance_max);
error_t ecu_timings_get_instance_parameters_read(ecu_timing_type_t type, ecu_timing_instance_t instance, ecu_core_runtime_value_ctx_t **read, ecu_runtime_param_index_t *count);
error_t ecu_timings_get_instance_parameters_write(ecu_timing_type_t type, ecu_timing_instance_t instance, ecu_core_runtime_value_ctx_t **write, ecu_runtime_param_index_t *count);

error_t ecu_timings_get_base_ctx(ecu_timing_base_t instance, timing_base_ctx_t **ctx);
error_t ecu_timings_get_ignition_ctx(ecu_timing_base_t instance, ignition_ctx_t **ctx);
error_t ecu_timings_get_injection_ctx(ecu_timing_base_t instance, injection_ctx_t **ctx);
error_t ecu_timings_get_rough_ctx(ecu_timing_base_t instance, rough_ctx_t **ctx);

#endif /* CONFIG_TIMINGS_INC_CONFIG_TIMINGS_H_ */
