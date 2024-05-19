/*
 * config_maf.h
 *
 *  Created on: May 14, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_INC_CONFIG_MAF_H_
#define CONFIG_INC_CONFIG_MAF_H_

#include "config_sensors.h"
#include "maf.h"

error_t ecu_sensors_maf_init(ecu_sensor_maf_t instance, maf_ctx_t *ctx);
error_t ecu_sensors_maf_get_default_config(ecu_sensor_maf_t instance, maf_config_t *config);
error_t ecu_sensors_maf_configure(ecu_sensor_maf_t instance, const maf_config_t *config);
error_t ecu_sensors_maf_reset(ecu_sensor_maf_t instance);

error_t ecu_sensors_maf_get_value(ecu_sensor_maf_t instance, maf_data_t *data);
error_t ecu_sensors_maf_get_diag(ecu_sensor_maf_t instance, maf_diag_t *diag);

#endif /* CONFIG_INC_CONFIG_MAF_H_ */
