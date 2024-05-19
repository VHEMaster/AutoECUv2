/*
 * config_aps.h
 *
 *  Created on: May 14, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_INC_CONFIG_APS_H_
#define CONFIG_INC_CONFIG_APS_H_

#include "config_sensors.h"
#include "aps.h"

error_t ecu_sensors_aps_init(ecu_sensor_aps_t instance, aps_ctx_t *ctx);
error_t ecu_sensors_aps_get_default_config(ecu_sensor_aps_t instance, aps_config_t *config);
error_t ecu_sensors_aps_configure(ecu_sensor_aps_t instance, const aps_config_t *config);
error_t ecu_sensors_aps_reset(ecu_sensor_aps_t instance);

error_t ecu_sensors_aps_get_value(ecu_sensor_aps_t instance, aps_data_t *data);
error_t ecu_sensors_aps_get_diag(ecu_sensor_aps_t instance, aps_diag_t *diag);


#endif /* CONFIG_INC_CONFIG_APS_H_ */
