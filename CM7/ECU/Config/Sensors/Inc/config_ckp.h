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

error_t ecu_sensors_ckp_init(ecu_sensor_ckp_t instance, ckp_ctx_t *ctx);
error_t ecu_sensors_ckp_get_default_config(ecu_sensor_ckp_t instance, ckp_config_t *config);
error_t ecu_sensors_ckp_configure(ecu_sensor_ckp_t instance, const ckp_config_t *config);
error_t ecu_sensors_ckp_reset(ecu_sensor_ckp_t instance);

#endif /* CONFIG_INC_CONFIG_CKP_H_ */
