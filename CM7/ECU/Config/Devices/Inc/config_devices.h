/*
 * config_devices.h
 *
 *  Created on: Apr 5, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_INC_CONFIG_DEVICES_H_
#define CONFIG_INC_CONFIG_DEVICES_H_

#include "common.h"
#include "pulsedadc.h"
#include "max31855.h"
#include "l9966.h"
#include "cj125.h"
#include "tle4729.h"
#include "tle6240.h"
#include "l9960.h"
#include "qspi.h"

#include "config_devices_types.h"

error_t ecu_devices_init(void);
void ecu_devices_loop_main(void);
void ecu_devices_loop_slow(void);
void ecu_devices_loop_fast(void);

error_t ecu_devices_get_device_ctx(ecu_device_type_t type, ecu_device_instance_t instance, void **ctx);
error_t ecu_devices_set_device_initialized(ecu_device_type_t type, ecu_device_instance_t instance, bool initialized);
error_t ecu_devices_get_device_initialized(ecu_device_type_t type, ecu_device_instance_t instance, bool *initialized);
error_t ecu_devices_set_device_enabled(ecu_device_type_t type, ecu_device_instance_t instance, bool enabled);
error_t ecu_devices_get_device_enabled(ecu_device_type_t type, ecu_device_instance_t instance, bool *enabled);

error_t ecu_devices_get_type_max(ecu_device_type_t *type_max);
error_t ecu_devices_get_instance_max(ecu_device_type_t type, ecu_device_instance_t *instance_max);
error_t ecu_devices_get_instance_parameters_read(ecu_device_type_t type, ecu_device_instance_t instance, ecu_core_runtime_value_ctx_t **read, ecu_runtime_param_index_t *count);
error_t ecu_devices_get_instance_parameters_write(ecu_device_type_t type, ecu_device_instance_t instance, ecu_core_runtime_value_ctx_t **write, ecu_runtime_param_index_t *count);
error_t ecu_devices_get_type_parameters_count_read(ecu_device_type_t type, ecu_runtime_param_index_t *count);
error_t ecu_devices_get_type_parameters_count_write(ecu_device_type_t type, ecu_runtime_param_index_t *count);

error_t ecu_devices_get_pulsedadc_ctx(ecu_device_pulsedadc_t instance, pulsedadc_ctx_t **ctx);
error_t ecu_devices_get_tcs_ctx(ecu_device_tcs_t instance, max31855_ctx_t **ctx);
error_t ecu_devices_get_flexio_ctx(ecu_device_flexio_t instance, l9966_ctx_t **ctx);
error_t ecu_devices_get_wbls_ctx(ecu_device_wbls_t instance, cj125_ctx_t **ctx);
error_t ecu_devices_get_stepper_ctx(ecu_device_stepper_t instance, tle4729_ctx_t **ctx);
error_t ecu_devices_get_output_ctx(ecu_device_output_t instance, tle6240_ctx_t **ctx);
error_t ecu_devices_get_motor_ctx(ecu_device_motor_t instance, l9960_ctx_t **ctx);
error_t ecu_devices_get_flash_ctx(ecu_device_flash_t instance, qspi_ctx_t **ctx);

#endif /* CONFIG_INC_CONFIG_DEVICES_H_ */
