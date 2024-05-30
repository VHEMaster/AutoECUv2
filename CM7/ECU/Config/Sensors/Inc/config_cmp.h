/*
 * config_cmp.h
 *
 *  Created on: May 14, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_INC_CONFIG_CMP_H_
#define CONFIG_INC_CONFIG_CMP_H_

#include "config_sensors.h"
#include "cmp.h"

#define ECU_SENSORS_CMP_CALLBACKS_MAX   8

error_t ecu_sensors_cmp_init(ecu_sensor_cmp_t instance, cmp_ctx_t *ctx);
error_t ecu_sensors_cmp_get_default_config(ecu_sensor_cmp_t instance, cmp_config_t *config);
error_t ecu_sensors_cmp_configure(ecu_sensor_cmp_t instance, const cmp_config_t *config);
error_t ecu_sensors_cmp_reset(ecu_sensor_cmp_t instance);

error_t ecu_sensors_cmp_get_value(ecu_sensor_cmp_t instance, cmp_data_t *data);
error_t ecu_sensors_cmp_get_diag(ecu_sensor_cmp_t instance, cmp_diag_t *diag);

error_t ecu_sensors_cmp_register_cb(ecu_sensor_cmp_t instance, cmp_signal_update_cb_t callback, void *usrdata);

#endif /* CONFIG_INC_CONFIG_CMP_H_ */
