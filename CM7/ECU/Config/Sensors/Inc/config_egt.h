/*
 * config_egt.h
 *
 *  Created on: May 14, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_INC_CONFIG_EGT_H_
#define CONFIG_INC_CONFIG_EGT_H_

#include "config_sensors.h"
#include "egt.h"

error_t ecu_sensors_egt_init(ecu_sensor_egt_t instance, egt_ctx_t *ctx);
error_t ecu_sensors_egt_get_default_config(ecu_sensor_egt_t instance, egt_config_t *config);
error_t ecu_sensors_egt_configure(ecu_sensor_egt_t instance, const egt_config_t *config);
error_t ecu_sensors_egt_reset(ecu_sensor_egt_t instance);

error_t ecu_sensors_egt_get_value(ecu_sensor_egt_t instance, egt_data_t *data);
error_t ecu_sensors_egt_get_diag(ecu_sensor_egt_t instance, egt_diag_t *diag);

#endif /* CONFIG_INC_CONFIG_EGT_H_ */
