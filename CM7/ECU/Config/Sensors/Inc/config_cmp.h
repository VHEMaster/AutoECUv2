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

error_t ecu_sensors_cmp_init(ecu_sensor_cmp_t instance, cmp_ctx_t *ctx);
error_t ecu_sensors_cmp_get_default_config(ecu_sensor_cmp_t instance, cmp_config_t *config);
error_t ecu_sensors_cmp_configure(ecu_sensor_cmp_t instance, const cmp_config_t *config);
error_t ecu_sensors_cmp_reset(ecu_sensor_cmp_t instance);

#endif /* CONFIG_INC_CONFIG_CMP_H_ */
