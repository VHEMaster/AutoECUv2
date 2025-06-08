/*
 * calcdata_outputs.c
 *
 *  Created on: Jun 8, 2025
 *      Author: VHEMaster
 */

#include "calcdata_outputs.h"
#include "calcdata_proc.h"
#include "config_global.h"

void core_calcdata_outputs_process(ecu_core_ctx_t *ctx, ecu_core_calcdata_outputs_stage_t stage)
{
  switch(stage) {
    case CORE_CALCDATA_OUTPUTS_STAGE_1:
      (void)core_calcdata_proc_calc_output(ctx, CALCDATA_OUTPUT_IAT_ALPHA_BLENDING, CALCDATA_OUTPUT_VARIANTED_ITEM_MAX, NULL);

      break;
    default:
      break;
  }
}
