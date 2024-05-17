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

#define ECU_CONFIG_MOTOR_PWM_DEPTH_MIN    (256)

error_t ecu_devices_motor_init(ecu_device_motor_t instance, l9960_ctx_t *ctx);
error_t ecu_devices_motor_get_default_config(ecu_device_motor_t instance, l9960_config_t *config);
error_t ecu_devices_motor_configure(ecu_device_motor_t instance, const l9960_config_t *config);
error_t ecu_devices_motor_hwsc(ecu_device_motor_t instance);
error_t ecu_devices_motor_diagoff(ecu_device_motor_t instance);
error_t ecu_devices_motor_reset(ecu_device_motor_t instance);

error_t ecu_devices_motor_get_diag(ecu_device_motor_t instance, l9960_diag_t *diag);

error_t ecu_devices_motor_set_enabled(ecu_device_motor_t instance, bool enabled);
error_t ecu_devices_motor_set_dutycycle(ecu_device_motor_t instance, float dutycycle);
error_t ecu_devices_motor_set_frequency(ecu_device_motor_t instance, float frequency);

error_t ecu_devices_motor_reconfigure(ecu_device_motor_t instance);

#endif /* CONFIG_INC_CONFIG_MOTOR_H_ */
