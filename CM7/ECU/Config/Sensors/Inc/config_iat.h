/*
 * config_iat.h
 *
 *  Created on: May 14, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_INC_CONFIG_IAT_H_
#define CONFIG_INC_CONFIG_IAT_H_

#include "config_sensors.h"
#include "iat.h"

error_t ecu_sensors_iat_init(ecu_sensor_iat_t instance, iat_ctx_t *ctx);
error_t ecu_sensors_iat_get_default_config(ecu_sensor_iat_t instance, iat_config_t *config);
error_t ecu_sensors_iat_configure(ecu_sensor_iat_t instance, const iat_config_t *config);
error_t ecu_sensors_iat_reset(ecu_sensor_iat_t instance);

error_t ecu_sensors_iat_get_value(ecu_sensor_iat_t instance, iat_data_t *data);
error_t ecu_sensors_iat_get_diag(ecu_sensor_iat_t instance, iat_diag_t *diag);

#endif /* CONFIG_INC_CONFIG_IAT_H_ */
