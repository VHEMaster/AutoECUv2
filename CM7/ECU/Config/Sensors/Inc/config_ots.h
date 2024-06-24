/*
 * config_ots.h
 *
 *  Created on: Jun 24, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_INC_CONFIG_OTS_H_
#define CONFIG_INC_CONFIG_OTS_H_

#include "config_sensors.h"
#include "ots.h"

error_t ecu_sensors_ots_init(ecu_sensor_ots_t instance, ots_ctx_t *ctx);
error_t ecu_sensors_ots_get_default_config(ecu_sensor_ots_t instance, ots_config_t *config);
error_t ecu_sensors_ots_configure(ecu_sensor_ots_t instance, const ots_config_t *config);
error_t ecu_sensors_ots_reset(ecu_sensor_ots_t instance);

error_t ecu_sensors_ots_get_value(ecu_sensor_ots_t instance, ots_data_t *data);
error_t ecu_sensors_ots_get_diag(ecu_sensor_ots_t instance, ots_diag_t *diag);

#endif /* CONFIG_INC_CONFIG_OTS_H_ */
