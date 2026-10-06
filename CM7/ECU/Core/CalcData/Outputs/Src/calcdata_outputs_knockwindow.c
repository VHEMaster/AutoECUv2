/*
 * calcdata_outputs_knockwindow.c
 *
 *  Created on: 6 окт. 2026 г.
 *      Author: VHEMaster
 */

#include "calcdata_outputs_knockwindow.h"
#include "config_global.h"

#include "calcdata_proc.h"

void calcdata_outputs_knockwindow(ecu_core_ctx_t *ctx)
{
  const ecu_core_runtime_value_ctx_t *knock_window_start;
  const ecu_core_runtime_value_ctx_t *knock_window_end;
  ecu_core_runtime_value_ctx_t output_value_start;
  ecu_core_runtime_value_ctx_t output_value_end;
  ecu_bank_t bank;

  ecu_core_runtime_global_instance_parameters_ctx_t *output_ptr;
  ecu_timing_knockwindow_write_params_t param_index;

  output_ptr = &ctx->runtime.global.parameters_virtual[ECU_CORE_RUNTIME_PARAMS_VIRT_SOURCE_INTERNAL].timings[ECU_TIMING_TYPE_KNOCKWINDOW][ECU_TIMING_KNOCKWINDOW_1];

  memset(&output_value_start, 0, sizeof(output_value_start));
  memset(&output_value_end, 0, sizeof(output_value_end));

  // TODO: define prober bank??
  bank = ECU_BANK_1;

  (void)core_calcdata_proc_get_output_ptr(ctx, bank, CALCDATA_OUTPUT_KNOCK_WINDOW_START, &knock_window_start);
  (void)core_calcdata_proc_get_output_ptr(ctx, bank, CALCDATA_OUTPUT_KNOCK_WINDOW_END, &knock_window_end);

  output_value_start = *knock_window_start;
  output_value_end = *knock_window_end;

  if(output_value_start.valid && output_value_end.valid) {
    param_index = ECU_TIMING_KNOCKWINDOW_WRITE_PARAM_ALLOWED;
    output_ptr->params[ECU_COMMON_WRITE][param_index].value = ECU_RUNTIME_PARAMETER_TRUE;
    output_ptr->params[ECU_COMMON_WRITE][param_index].valid = true;

    param_index = ECU_TIMING_KNOCKWINDOW_WRITE_PARAM_KNOCK_WINDOW_START;
    output_ptr->params[ECU_COMMON_WRITE][param_index].value = output_value_start.value;
    output_ptr->params[ECU_COMMON_WRITE][param_index].valid = true;

    param_index = ECU_TIMING_KNOCKWINDOW_WRITE_PARAM_KNOCK_WINDOW_END;
    output_ptr->params[ECU_COMMON_WRITE][param_index].value = output_value_end.value;
    output_ptr->params[ECU_COMMON_WRITE][param_index].valid = true;
  } else {
    param_index = ECU_TIMING_KNOCKWINDOW_WRITE_PARAM_ALLOWED;
    output_ptr->params[ECU_COMMON_WRITE][param_index].value = ECU_RUNTIME_PARAMETER_FALSE;
    output_ptr->params[ECU_COMMON_WRITE][param_index].valid = true;
  }
}
