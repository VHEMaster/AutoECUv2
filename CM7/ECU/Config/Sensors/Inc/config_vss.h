/*
 * config_vss.h
 *
 *  Created on: Jun 2, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_INC_CONFIG_VSS_H_
#define CONFIG_INC_CONFIG_VSS_H_

#include "config_sensors.h"
#include "vss.h"

error_t ecu_sensors_vss_init(ecu_sensor_vss_t instance, vss_ctx_t *ctx);
error_t ecu_sensors_vss_get_default_config(ecu_sensor_vss_t instance, vss_config_t *config);
error_t ecu_sensors_vss_configure(ecu_sensor_vss_t instance, const vss_config_t *config);
error_t ecu_sensors_vss_reset(ecu_sensor_vss_t instance);

error_t ecu_sensors_vss_get_value(ecu_sensor_vss_t instance, vss_data_t *data);
error_t ecu_sensors_vss_get_diag(ecu_sensor_vss_t instance, vss_diag_t *diag);

#endif /* CONFIG_INC_CONFIG_VSS_H_ */
