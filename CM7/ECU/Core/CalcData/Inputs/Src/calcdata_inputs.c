/*
 * calcdata_inputs.c
 *
 *  Created on: Jun 5, 2025
 *      Author: VHEMaster
 */

#include "calcdata_inputs.h"
#include "config_global.h"

#include "calcdata_inputs_sensors.h"
#include "calcdata_inputs_calc.h"

void core_calcdata_inputs_process(ecu_core_ctx_t *ctx, ecu_core_calcdata_inputs_stage_t stage)
{
  switch(stage) {
    case CORE_CALCDATA_INPUTS_STAGE_1:
      core_calcdata_inputs_sensors_process(ctx);
      break;
    case CORE_CALCDATA_INPUTS_STAGE_2:
      core_calcdata_inputs_calc_process(ctx);
      break;
    default:
      break;
  }
}
