/*
 * calcdata_inputs_calc.c
 *
 *  Created on: Jun 6, 2025
 *      Author: VHEMaster
 */

#include "calcdata_inputs_calc.h"
#include "config_global.h"

#include "calcdata_proc.h"

void core_calcdata_inputs_calc_process(ecu_core_ctx_t *ctx)
{
  const uint32_t banks_count = ctx->runtime.global.banks_count;

  core_calcdata_proc_input_interp_invalidate(ctx);
  core_calcdata_proc_output_invalidate(ctx);



  (void)core_calcdata_proc_calc_output(ctx, CALCDATA_OUTPUT_IAT_ALPHA_BLENDING, CALCDATA_OUTPUT_VARIANTED_ITEM_MAX, NULL);



  for(ecu_bank_t bank = 0; bank < banks_count; bank++) {

  }
}
