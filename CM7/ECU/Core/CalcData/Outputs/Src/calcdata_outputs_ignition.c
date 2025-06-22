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

  const ecu_core_runtime_banked_source_bank_ctx_t *runtime_banked;
  const ecu_core_runtime_value_ctx_t *input_value_run;
  const ecu_core_runtime_value_ctx_t *input_value_start;
  const ecu_core_runtime_value_ctx_t *runup_ign_duration;
  ecu_core_runtime_value_ctx_t output_value;
  ecu_core_runtime_global_parameters_timing_ignition_ctx_t *output_ptr;
  ecu_config_calcdata_output_varianted_index_t output_variant;
  bool runup_flag = ctx->runtime.global.misc.runup_flag;
  time_float_s_t running_time_current = ctx->runtime.global.misc.running_time_current;
  float time_blending;

  output_ptr = &ctx->runtime.global.parameters.timings.ignition;

  for(ecu_bank_t bank = 0; bank < banks_count; bank++) {
    output_variant = CALCDATA_OUTPUT_VARIANTED_ITEM_V1;

    runtime_banked = &ctx->runtime.banked.source.banks[bank];
    input_value_run = &runtime_banked->outputs[CALCDATA_OUTPUT_IGNITION_ADVANCE].variants[output_variant];
    input_value_start = &runtime_banked->outputs[CALCDATA_OUTPUT_STARTUP_IGNITION_ADVANCE].variants[output_variant];
    runup_ign_duration = &runtime_banked->outputs[CALCDATA_OUTPUT_STARTUP_RUNUP_IGN_DURATION].variants[output_variant];

    if(runup_flag) {
      time_blending = running_time_current / runup_ign_duration->value;
      if(time_blending == time_blending && time_blending < 1.0f) {
        output_value.value = BLEND(input_value_start->value, input_value_run->value, time_blending);
        output_value.valid = true;
      } else {
        output_value.value = input_value_run->value;
        output_value.valid = true;
      }
    } else {
      output_value.value = input_value_start->value;
      output_value.valid = true;
    }

    if(output_value.valid) {
      output_ptr->write[bank].ignition_advance = output_value.value;
      output_ptr->write[bank].allowed = true;
    } else {
      output_ptr->write[bank].allowed = false;
    }
  }

  output_ptr->write_valid = true;
}
