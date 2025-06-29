/*
 * calcdata_outputs_etc.c
 *
 *  Created on: Jun 29, 2025
 *      Author: VHEMaster
 */

#include "calcdata_outputs_idle.h"
#include "config_global.h"

#include "calcdata_proc.h"

void calcdata_outputs_etc(ecu_core_ctx_t *ctx)
{
  const uint32_t banks_count = ctx->runtime.global.banks_count;
  time_float_s_t runned_time_overall = ctx->runtime.global.misc.runned_time_overall;
  time_float_s_t running_time_current = ctx->runtime.global.misc.running_time_current;
  bool runup_flag = ctx->runtime.global.misc.runup_flag;
  bool idle_flag = ctx->runtime.global.misc.idle_flag;

  const ecu_core_runtime_value_ctx_t *input_rpm;
  const ecu_core_runtime_value_ctx_t *input_aps;


  for(ecu_bank_t bank = 0; bank < banks_count; bank++) {
    (void)core_calcdata_proc_get_input_ptr(ctx, bank, CALCDATA_RELATION_INPUT_SOURCE_SENSOR_GLOBAL_CKP, &input_rpm);
    (void)core_calcdata_proc_get_input_ptr(ctx, bank, CALCDATA_RELATION_INPUT_SOURCE_SENSOR_GLOBAL_APS, &input_aps);


    //TODO: IMPLEMENT
  }

}
