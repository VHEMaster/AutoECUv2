/*
 * config_ckp.h
 *
 *  Created on: May 14, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_INC_CONFIG_CKP_H_
#define CONFIG_INC_CONFIG_CKP_H_

#include "config_sensors.h"
#include "ckp.h"

#define ECU_SENSORS_CKP_CALLBACKS_MAX   16

error_t ecu_sensors_ckp_init(ecu_sensor_ckp_t instance, ckp_ctx_t *ctx);
error_t ecu_sensors_ckp_get_default_config(ecu_sensor_ckp_t instance, ckp_config_t *config);
error_t ecu_sensors_ckp_configure(ecu_sensor_ckp_t instance, const ckp_config_t *config);
error_t ecu_sensors_ckp_reset(ecu_sensor_ckp_t instance);

error_t ecu_sensors_ckp_get_value(ecu_sensor_ckp_t instance, ckp_data_t *data);
error_t ecu_sensors_ckp_get_diag(ecu_sensor_ckp_t instance, ckp_diag_t *diag);
error_t ecu_sensors_ckp_calculate_current_position(ecu_sensor_ckp_t instance, ckp_req_t *req, ckp_data_t *data);

error_t ecu_sensors_ckp_register_cb(ecu_sensor_ckp_t instance, ckp_signal_update_cb_t callback, void *usrdata);


#endif /* CONFIG_INC_CONFIG_CKP_H_ */
