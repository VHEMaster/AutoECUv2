/*
 * calcdata_outputs_ignition.c
 *
 *  Created on: Jun 22, 2025
 *      Author: VHEMaster
 */

#include "calcdata_outputs_ignition.h"
#include "config_global.h"

void calcdata_outputs_ignition(ecu_core_ctx_t *ctx)
{
  const uint32_t banks_count = ctx->runtime.global.banks_count;
  const ecu_core_runtime_value_ctx_t *input_value;
  ecu_core_runtime_global_parameters_timing_ignition_ctx_t *output_ptr;
  ecu_config_calcdata_output_varianted_index_t output_variant;

  output_ptr = &ctx->runtime.global.parameters.timings.ignition;

  for(ecu_bank_t bank = 0; bank < banks_count; bank++) {
    output_variant = CALCDATA_OUTPUT_VARIANTED_ITEM_V1;

    input_value = &ctx->runtime.banked.source.banks[bank].outputs[CALCDATA_OUTPUT_IGNITION_ADVANCE].variants[output_variant];

    if(input_value->valid) {
      output_ptr->write[bank].ignition_advance = input_value->value;
      output_ptr->write[bank].allowed = true;
    } else {
      output_ptr->write[bank].allowed = false;
    }
  }

  output_ptr->write_valid = true;
}
