/*
 * config_ops.h
 *
 *  Created on: Jun 24, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_INC_CONFIG_OPS_H_
#define CONFIG_INC_CONFIG_OPS_H_

#include "config_sensors.h"
#include "ops.h"

typedef enum {
  ECU_SENSOR_OPS_READ_PARAM_DATA = 0,
  ECU_SENSOR_OPS_READ_PARAM_MAX,
}ecu_sensor_ops_read_params_t;

error_t ecu_sensors_ops_init(ecu_sensor_ops_t instance, ops_ctx_t *ctx);
error_t ecu_sensors_ops_get_default_config(ecu_sensor_ops_t instance, ops_config_t *config);
error_t ecu_sensors_ops_configure(ecu_sensor_ops_t instance, const ops_config_t *config);
error_t ecu_sensors_ops_reset(ecu_sensor_ops_t instance);

error_t ecu_sensors_ops_get_value(ecu_sensor_ops_t instance, ops_data_t *data);
error_t ecu_sensors_ops_get_diag(ecu_sensor_ops_t instance, ops_diag_t *diag);

#endif /* CONFIG_INC_CONFIG_OPS_H_ */
