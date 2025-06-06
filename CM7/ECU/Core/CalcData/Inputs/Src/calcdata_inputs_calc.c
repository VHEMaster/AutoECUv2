/*
 * calcdata_inputs_calc.c
 *
 *  Created on: Jun 6, 2025
 *      Author: VHEMaster
 */

#include "calcdata_inputs_calc.h"
#include "config_global.h"

void core_calcdata_inputs_calc_process(ecu_core_ctx_t *ctx)
{
  const uint32_t banks_count = ctx->runtime.global.banks_count;

  for(ecu_bank_t bank = 0; bank < banks_count; bank++) {

  }
}
