/*
 * config_ect.h
 *
 *  Created on: May 14, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_INC_CONFIG_ECT_H_
#define CONFIG_INC_CONFIG_ECT_H_

#include "config_sensors.h"
#include "ect.h"

error_t ecu_sensors_ect_init(ecu_sensor_ect_t instance, ect_ctx_t *ctx);
error_t ecu_sensors_ect_get_default_config(ecu_sensor_ect_t instance, ect_config_t *config);
error_t ecu_sensors_ect_configure(ecu_sensor_ect_t instance, const ect_config_t *config);
error_t ecu_sensors_ect_reset(ecu_sensor_ect_t instance);

#endif /* CONFIG_INC_CONFIG_ECT_H_ */
