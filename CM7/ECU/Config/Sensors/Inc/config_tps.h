/*
 * config_tps.h
 *
 *  Created on: May 14, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_INC_CONFIG_TPS_H_
#define CONFIG_INC_CONFIG_TPS_H_

#include "config_sensors.h"
#include "tps.h"

typedef struct {
    uint8_t signals_count;
    bool active[TPS_CONFIG_SIGNALS_MAX];
    float voltages[TPS_CONFIG_SIGNALS_MAX];
    float positions[TPS_CONFIG_SIGNALS_MAX];
    float position;
    float position_unfiltered;
    bool data_valid;
}ecu_sensor_tps_value_t;

error_t ecu_sensors_tps_init(ecu_sensor_tps_t instance, tps_ctx_t *ctx);
error_t ecu_sensors_tps_get_default_config(ecu_sensor_tps_t instance, tps_config_t *config);
error_t ecu_sensors_tps_configure(ecu_sensor_tps_t instance, const tps_config_t *config);
error_t ecu_sensors_tps_reset(ecu_sensor_tps_t instance);

error_t ecu_sensors_tps_get_value(ecu_sensor_tps_t instance, ecu_sensor_tps_value_t *value);
error_t ecu_sensors_tps_get_diag(ecu_sensor_tps_t instance, tps_diag_t *diag);

#endif /* CONFIG_INC_CONFIG_TPS_H_ */
