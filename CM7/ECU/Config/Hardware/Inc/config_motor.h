/*
 * config_motor.h
 *
 *  Created on: Apr 17, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_INC_CONFIG_MOTOR_H_
#define CONFIG_INC_CONFIG_MOTOR_H_

#include "config_devices.h"
#include "l9960.h"

error_t ecu_devices_motor_init(ecu_device_motor_t instance, l9960_ctx_t *ctx);
error_t ecu_devices_motor_get_default_config(ecu_device_motor_t instance, l9960_config_t *config);
error_t ecu_devices_motor_configure(ecu_device_motor_t instance, const l9960_config_t *config);
error_t ecu_devices_motor_reset(ecu_device_motor_t instance);

#endif /* CONFIG_INC_CONFIG_MOTOR_H_ */
