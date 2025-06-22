/*
 * calcdata_outputs_injection.c
 *
 *  Created on: Jun 22, 2025
 *      Author: VHEMaster
 */

#include "calcdata_outputs_injection.h"
#include "config_global.h"

void calcdata_outputs_injection(ecu_core_ctx_t *ctx)
{
  const uint32_t banks_count = ctx->runtime.global.banks_count;
  const ecu_core_runtime_banked_source_bank_ctx_t *runtime_banked;
  const ecu_core_runtime_value_ctx_t *input_inj_phase;
  const ecu_core_runtime_value_ctx_t *input_inj_afr;
  const ecu_core_runtime_value_ctx_t *input_cycle_charge;

  ecu_core_runtime_global_parameters_timing_injection_ctx_t *output_ptr;
  ecu_config_calcdata_output_varianted_index_t output_variant;

  float value_afr, value_cycle_charge, value_phase, value_mass;

  output_ptr = &ctx->runtime.global.parameters.timings.injection;

  for(ecu_bank_t bank = 0; bank < banks_count; bank++) {
    output_variant = CALCDATA_OUTPUT_VARIANTED_ITEM_V1;
    runtime_banked = &ctx->runtime.banked.source.banks[bank];

    input_cycle_charge = &runtime_banked->inputs[CALCDATA_RELATION_INPUT_SOURCE_CALC_CYCLE_CHARGE].value;
    input_inj_phase = &runtime_banked->outputs[CALCDATA_OUTPUT_INJECTION_PHASE].variants[output_variant];
    input_inj_afr = &runtime_banked->outputs[CALCDATA_OUTPUT_AIR_TO_FUEL_RATIO].variants[output_variant];

    value_cycle_charge = input_cycle_charge->value;
    value_phase = input_inj_phase->value;
    value_afr = input_inj_afr->value;

    if(input_cycle_charge->valid /* && input_inj_phase->valid && input_inj_afr->valid */) {

      value_mass = 0.0f;

      output_ptr->write[bank].allowed = true;
      output_ptr->write[bank].injection_mass = value_mass;
      output_ptr->write[bank].injection_phase = value_phase;
    } else {
      value_cycle_charge = 0.0f;
      output_ptr->write[bank].allowed = false;
      output_ptr->write[bank].injection_mass = 0.0f;
      output_ptr->write[bank].injection_phase = value_phase;
    }
  }

  output_ptr->write_valid = true;
}
