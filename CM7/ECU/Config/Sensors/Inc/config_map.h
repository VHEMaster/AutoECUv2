/*
 * config_map.h
 *
 *  Created on: May 14, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_INC_CONFIG_MAP_H_
#define CONFIG_INC_CONFIG_MAP_H_

#include "config_sensors.h"
#include "map.h"

error_t ecu_sensors_map_init(ecu_sensor_map_t instance, map_ctx_t *ctx);
error_t ecu_sensors_map_get_default_config(ecu_sensor_map_t instance, map_config_t *config);
error_t ecu_sensors_map_configure(ecu_sensor_map_t instance, const map_config_t *config);
error_t ecu_sensors_map_reset(ecu_sensor_map_t instance);

#endif /* CONFIG_INC_CONFIG_MAP_H_ */
