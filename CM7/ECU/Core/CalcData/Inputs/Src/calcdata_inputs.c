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
      calcdata_inputs_calc_iat_manifold(ctx);
      break;
    case CORE_CALCDATA_INPUTS_STAGE_3:
      calcdata_inputs_calc_cycle_charge(ctx);
      calcdata_inputs_calc_mass_air_flow(ctx);
      break;
    default:
      break;
  }
}
