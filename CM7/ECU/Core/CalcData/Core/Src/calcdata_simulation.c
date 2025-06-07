/*
 * calcdata_simulation.c
 *
 *  Created on: Jun 7, 2025
 *      Author: VHEMaster
 */

#include "calcdata_simulation.h"

void core_calcdata_simulation(ecu_core_ctx_t *ctx)
{
  ecu_core_runtime_global_parameters_ctx_t *params = &ctx->runtime.global.parameters_simulated;
  ecu_core_runtime_global_parameters_sensor_value_ctx_t *value;

  value = &params->sensors[ECU_SENSOR_TYPE_IAT][ECU_SENSOR_IAT_1];
  value = &params->sensors[ECU_SENSOR_TYPE_IAT][ECU_SENSOR_IAT_2];
  value->value = 20.0f; value->read_valid = true;

}
