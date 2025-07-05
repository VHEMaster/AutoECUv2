/*
 * calcdata_outputs_ignition.c
 *
 *  Created on: Jun 22, 2025
 *      Author: VHEMaster
 */

#include "calcdata_outputs_ignition.h"
#include "config_global.h"

#include "calcdata_proc.h"

void calcdata_outputs_ignition(ecu_core_ctx_t *ctx)
{
  const uint32_t banks_count = ctx->runtime.global.banks_count;

  const ecu_core_runtime_value_ctx_t *input_value_run;
  const ecu_core_runtime_value_ctx_t *input_value_start;
  const ecu_core_runtime_value_ctx_t *runup_ign_duration;

  const ecu_core_runtime_value_ctx_t *input_rpm;
  const ecu_core_runtime_value_ctx_t *input_target_rpm;
  const ecu_core_runtime_value_ctx_t *input_target_ignition_advance;

  ecu_core_runtime_value_ctx_t output_value;
  ecu_core_runtime_global_parameters_timing_ignition_ctx_t *output_ptr;
  bool runup_flag = ctx->runtime.global.misc.runup_flag;
  time_float_s_t running_time_current = ctx->runtime.global.misc.running_time_current;
  float time_blending;

  output_ptr = &ctx->runtime.global.parameters.timings.ignition;

  for(ecu_bank_t bank = 0; bank < banks_count; bank++) {

    (void)core_calcdata_proc_get_output_ptr(ctx, bank, CALCDATA_OUTPUT_IGNITION_ADVANCE, &input_value_run);
    (void)core_calcdata_proc_get_output_ptr(ctx, bank, CALCDATA_OUTPUT_STARTUP_IGNITION_ADVANCE, &input_value_start);
    (void)core_calcdata_proc_get_output_ptr(ctx, bank, CALCDATA_OUTPUT_STARTUP_RUNUP_IGN_DURATION, &runup_ign_duration);

    (void)core_calcdata_proc_get_input_ptr(ctx, bank, CALCDATA_RELATION_INPUT_SOURCE_SENSOR_GLOBAL_CKP, &input_rpm);
    (void)core_calcdata_proc_get_output_ptr(ctx, bank, CALCDATA_OUTPUT_IDLE_TARGET_RPM, &input_target_rpm);
    (void)core_calcdata_proc_get_output_ptr(ctx, bank, CALCDATA_OUTPUT_IDLE_TARGET_IGNITION_ADVANCE, &input_target_ignition_advance);

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
