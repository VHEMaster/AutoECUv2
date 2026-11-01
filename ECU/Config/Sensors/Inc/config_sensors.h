/*
 * config_sensors.h
 *
 *  Created on: May 14, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_INC_CONFIG_SENSORS_H_
#define CONFIG_INC_CONFIG_SENSORS_H_

#include "common.h"
#include "ckp.h"
#include "cmp.h"
#include "ect.h"
#include "egt.h"
#include "iat.h"
#include "maf.h"
#include "map.h"
#include "tps.h"
#include "vss.h"
#include "aps.h"
#include "ots.h"
#include "ops.h"

#include "config_sensors_types.h"

error_t ecu_sensors_init(void);
void ecu_sensors_loop_main(void);
void ecu_sensors_loop_slow(void);
void ecu_sensors_loop_fast(void);

error_t ecu_sensors_get_sensor_ctx(ecu_sensor_type_t type, ecu_sensor_instance_t instance, void **ctx);
error_t ecu_sensors_set_sensor_initialized(ecu_sensor_type_t type, ecu_sensor_instance_t instance, bool initialized);
error_t ecu_sensors_get_sensor_initialized(ecu_sensor_type_t type, ecu_sensor_instance_t instance, bool *initialized);
error_t ecu_sensors_set_sensor_enabled(ecu_sensor_type_t type, ecu_sensor_instance_t instance, bool enabled);
error_t ecu_sensors_get_sensor_enabled(ecu_sensor_type_t type, ecu_sensor_instance_t instance, bool *enabled);

error_t ecu_sensors_get_type_max(ecu_sensor_type_t *type_max);
error_t ecu_sensors_get_instance_max(ecu_sensor_type_t type, ecu_sensor_instance_t *instance_max);
error_t ecu_sensors_get_instance_parameters_read(ecu_sensor_type_t type, ecu_sensor_instance_t instance, ecu_core_runtime_value_ctx_t **read, ecu_runtime_param_index_t *count);
error_t ecu_sensors_get_instance_parameters_write(ecu_sensor_type_t type, ecu_sensor_instance_t instance, ecu_core_runtime_value_ctx_t **write, ecu_runtime_param_index_t *count);
error_t ecu_sensors_get_type_parameters_count_read(ecu_sensor_type_t type, ecu_runtime_param_index_t *count);
error_t ecu_sensors_get_type_parameters_count_write(ecu_sensor_type_t type, ecu_runtime_param_index_t *count);

error_t ecu_sensors_get_ckp_ctx(ecu_sensor_ckp_t instance, ckp_ctx_t **ctx);
error_t ecu_sensors_get_cmp_ctx(ecu_sensor_cmp_t instance, cmp_ctx_t **ctx);
error_t ecu_sensors_get_ect_ctx(ecu_sensor_ect_t instance, ect_ctx_t **ctx);
error_t ecu_sensors_get_egt_ctx(ecu_sensor_egt_t instance, egt_ctx_t **ctx);
error_t ecu_sensors_get_iat_ctx(ecu_sensor_iat_t instance, iat_ctx_t **ctx);
error_t ecu_sensors_get_maf_ctx(ecu_sensor_maf_t instance, maf_ctx_t **ctx);
error_t ecu_sensors_get_map_ctx(ecu_sensor_map_t instance, map_ctx_t **ctx);
error_t ecu_sensors_get_tps_ctx(ecu_sensor_tps_t instance, tps_ctx_t **ctx);
error_t ecu_sensors_get_vss_ctx(ecu_sensor_vss_t instance, vss_ctx_t **ctx);
error_t ecu_sensors_get_aps_ctx(ecu_sensor_aps_t instance, aps_ctx_t **ctx);
error_t ecu_sensors_get_ots_ctx(ecu_sensor_ots_t instance, ots_ctx_t **ctx);
error_t ecu_sensors_get_ops_ctx(ecu_sensor_ops_t instance, ops_ctx_t **ctx);

#endif /* CONFIG_INC_CONFIG_SENSORS_H_ */
