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
  time_float_s_t runned_time_overall = ctx->runtime.global.misc.runned_time_overall;
  time_float_s_t running_time_current = ctx->runtime.global.misc.running_time_current;
  bool runup_flag = ctx->runtime.global.misc.runup_flag;
  bool turning_flag = ctx->timing_data.crankshaft.sensor_data.validity >= CKP_DATA_VALID;

  float startup_large_revs = ctx->calibration->calcdata.setup.startup_large_revs;
  float startup_large_to_small_trans_revs = ctx->calibration->calcdata.setup.startup_large_to_small_trans_revs;
  float startup_transition_reset = ctx->calibration->calcdata.setup.startup_transition_reset;
  float startup_revs_counter = ctx->runtime.global.misc.injection_startup_revs_counter;
  uint32_t ckp_revs_count = ctx->timing_data.crankshaft.sensor_data.revs_count;

  const ecu_core_runtime_banked_source_bank_ctx_t *runtime_banked;

  const ecu_core_runtime_value_ctx_t *input_inj_phase_run;
  const ecu_core_runtime_value_ctx_t *input_inj_phase_start;
  const ecu_core_runtime_value_ctx_t *runup_inj_duration;

  const ecu_core_runtime_value_ctx_t *runup_inj_cold_corr;
  const ecu_core_runtime_value_ctx_t *runup_inj_cold_time;

  const ecu_core_runtime_value_ctx_t *runup_inj_small_charge;
  const ecu_core_runtime_value_ctx_t *runup_inj_large_charge;

  const ecu_core_runtime_value_ctx_t *input_inj_afr;
  const ecu_core_runtime_value_ctx_t *input_cycle_charge;

  ecu_core_runtime_global_parameters_timing_injection_ctx_t *output_ptr;
  ecu_config_calcdata_output_varianted_index_t output_variant;

  float output_inj_phase;
  float output_inj_mass_run;
  float output_inj_mass_start;
  float output_inj_mass;

  float value_afr, value_cycle_charge;
  float time_blending;
  float cold_inj_coff;
  uint32_t revs_delta;

  output_ptr = &ctx->runtime.global.parameters.timings.injection;

  if(!turning_flag) {
    revs_delta = 0;
    startup_revs_counter = 0;
    ctx->runtime.global.misc.injection_revs_counter = ckp_revs_count;
  } else {
    revs_delta = ckp_revs_count - ctx->runtime.global.misc.injection_revs_counter;
    ctx->runtime.global.misc.injection_revs_counter = ckp_revs_count;
    if(!runup_flag) {
      startup_revs_counter = 0;
    } else {
      startup_revs_counter += revs_delta;
    }
  }

  for(ecu_bank_t bank = 0; bank < banks_count; bank++) {
    output_variant = CALCDATA_OUTPUT_VARIANTED_ITEM_V1;
    runtime_banked = &ctx->runtime.banked.source.banks[bank];

    input_cycle_charge = &runtime_banked->inputs[CALCDATA_RELATION_INPUT_SOURCE_CALC_CYCLE_CHARGE].value;
    input_inj_afr = &runtime_banked->outputs[CALCDATA_OUTPUT_AIR_TO_FUEL_RATIO].variants[output_variant];

    input_inj_phase_run = &runtime_banked->outputs[CALCDATA_OUTPUT_INJECTION_PHASE].variants[output_variant];
    input_inj_phase_start = &runtime_banked->outputs[CALCDATA_OUTPUT_STARTUP_INJECTION_PHASE].variants[output_variant];
    runup_inj_duration = &runtime_banked->outputs[CALCDATA_OUTPUT_STARTUP_RUNUP_INJ_DURATION].variants[output_variant];

    runup_inj_small_charge = &runtime_banked->outputs[CALCDATA_OUTPUT_STARTUP_SMALL_INJ_CHARGE].variants[output_variant];
    runup_inj_large_charge = &runtime_banked->outputs[CALCDATA_OUTPUT_STARTUP_LARGE_INJ_CHARGE].variants[output_variant];

    runup_inj_cold_corr = &runtime_banked->outputs[CALCDATA_OUTPUT_STARTUP_COLD_INJ_CORR].variants[output_variant];
    runup_inj_cold_time = &runtime_banked->outputs[CALCDATA_OUTPUT_STARTUP_COLD_INJ_TIME].variants[output_variant];

    if(startup_revs_counter < startup_large_revs) {
      output_inj_mass_start = runup_inj_large_charge->value;
    } else if(startup_revs_counter >= startup_transition_reset) {
      output_inj_mass_start = runup_inj_large_charge->value;
      startup_revs_counter = 0;
    } else if(startup_revs_counter >= startup_large_revs + startup_large_to_small_trans_revs) {
      output_inj_mass_start = runup_inj_small_charge->value;
    } else {
      time_blending = (startup_revs_counter - startup_large_revs) / startup_large_to_small_trans_revs;
      time_blending = CLAMP(time_blending, 0.0f, 1.0f);
      output_inj_mass_start = BLEND(runup_inj_large_charge->value, runup_inj_small_charge->value, time_blending);
    }

    if(runup_flag) {
      if(input_cycle_charge->valid /* && input_inj_phase->valid && input_inj_afr->valid */) {
        value_cycle_charge = input_cycle_charge->value;
        value_afr = input_inj_afr->value;
        output_inj_mass_run = value_cycle_charge / value_afr;
      } else {
        output_inj_mass_run = 0.0f;
      }

      time_blending = runned_time_overall / runup_inj_cold_time->value;
      if(time_blending == time_blending && time_blending < 1.0f) {
        cold_inj_coff = runup_inj_cold_corr->value * (1.0f - time_blending);
        output_inj_mass_run *= cold_inj_coff + 1.0f;
      }

      time_blending = running_time_current / runup_inj_duration->value;
      if(output_inj_mass_run < runup_inj_small_charge->value && time_blending == time_blending && time_blending < 1.0f) {
        output_inj_phase = BLEND(input_inj_phase_start->value, input_inj_phase_run->value, time_blending);
        output_inj_mass = BLEND(runup_inj_small_charge->value, output_inj_mass_run, time_blending);
      } else {
        output_inj_phase = input_inj_phase_run->value;
        output_inj_mass = output_inj_mass_run;
      }
    } else {
      output_inj_phase = input_inj_phase_start->value;
      output_inj_mass = output_inj_mass_start;
    }


    output_ptr->write[bank].allowed = true;
    output_ptr->write[bank].injection_mass = output_inj_mass;
    output_ptr->write[bank].injection_phase = output_inj_phase;
  }

  ctx->runtime.global.misc.injection_startup_revs_counter = startup_revs_counter;

  output_ptr->write_valid = true;
}
