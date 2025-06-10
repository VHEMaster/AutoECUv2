/*
 * calcdata_simulation.c
 *
 *  Created on: Jun 7, 2025
 *      Author: VHEMaster
 */

#include "calcdata_simulation.h"

volatile bool calcdata_simulation_apply = true;

void core_calcdata_simulation(ecu_core_ctx_t *ctx)
{
  ecu_core_runtime_global_parameters_ctx_t *params = &ctx->runtime.global.parameters_simulated;
  ecu_core_runtime_value_ctx_t *value;

  if(calcdata_simulation_apply) {
    value = &params->sensors[ECU_SENSOR_TYPE_IAT][ECU_SENSOR_IAT_1];
    value->value = 25.0f; value->valid = true;

    value = &params->sensors[ECU_SENSOR_TYPE_IAT][ECU_SENSOR_IAT_2];
    value->value = 25.0f; value->valid = true;

    value = &params->sensors[ECU_SENSOR_TYPE_ECT][ECU_SENSOR_ECT_1];
    value->value = 90.0f; value->valid = true;

    value = &params->sensors[ECU_SENSOR_TYPE_MAF][ECU_SENSOR_MAF_1];
    value->value = 250.0f; value->valid = true;

    value = &params->sensors[ECU_SENSOR_TYPE_MAP][ECU_SENSOR_MAP_1];
    value->value = 0.880f; value->valid = true;

    value = &params->sensors[ECU_SENSOR_TYPE_TPS][ECU_SENSOR_TPS_1];
    value->value = 30.0f; value->valid = true;
  }
}
