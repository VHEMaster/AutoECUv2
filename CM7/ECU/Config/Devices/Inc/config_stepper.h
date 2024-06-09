/*
 * config_stepper.h
 *
 *  Created on: Apr 26, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_INC_CONFIG_STEPPER_H_
#define CONFIG_INC_CONFIG_STEPPER_H_

#include "config_devices.h"
#include "tle4729.h"

error_t ecu_devices_stepper_init(ecu_device_stepper_t instance, tle4729_ctx_t *ctx);
error_t ecu_devices_stepper_get_default_config(ecu_device_stepper_t instance, tle4729_config_t *config);
error_t ecu_devices_stepper_configure(ecu_device_stepper_t instance, const tle4729_config_t *config);
error_t ecu_devices_stepper_reset(ecu_device_stepper_t instance);

error_t ecu_devices_stepper_enable(ecu_device_stepper_t instance, bool enabled);
error_t ecu_devices_stepper_set_target(ecu_device_stepper_t instance, int32_t position);
error_t ecu_devices_stepper_get_target(ecu_device_stepper_t instance, int32_t *position);
error_t ecu_devices_stepper_set_current(ecu_device_stepper_t instance, int32_t position);

error_t ecu_devices_stepper_get_current(ecu_device_stepper_t instance, int32_t *position);
error_t ecu_devices_stepper_pos_reset(ecu_device_stepper_t instance, int32_t position);
error_t ecu_devices_stepper_is_failure(ecu_device_stepper_t instance, bool *failure);

#endif /* CONFIG_INC_CONFIG_STEPPER_H_ */
