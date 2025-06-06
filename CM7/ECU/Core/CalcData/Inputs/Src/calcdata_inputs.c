/*
 * core_calcdata_inputs.c
 *
 *  Created on: Jun 5, 2025
 *      Author: VHEMaster
 */

#include "calcdata_inputs.h"
#include "config_global.h"

#include "calcdata_inputs_sensors.h"
#include "calcdata_inputs_calc.h"

void core_calcdata_inputs_process(ecu_core_ctx_t *ctx)
{
  core_calcdata_inputs_sensors_process(ctx);
  core_calcdata_inputs_calc_process(ctx);
}
