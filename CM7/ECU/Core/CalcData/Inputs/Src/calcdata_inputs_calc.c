/*
 * calcdata_inputs_calc.c
 *
 *  Created on: Jun 6, 2025
 *      Author: VHEMaster
 */

#include "calcdata_inputs_calc.h"
#include "config_global.h"

#include "calcdata_proc.h"

static void calcdata_inputs_calc_iat_manifold(ecu_core_ctx_t *ctx);

time_msmnt_item_t timing_iat;

void core_calcdata_inputs_calc_process(ecu_core_ctx_t *ctx)
{
  uint32_t prim = EnterCritical();
  time_msmt_start(&timing_iat);
  calcdata_inputs_calc_iat_manifold(ctx);
  time_msmt_stop(&timing_iat);
  ExitCritical(prim);


}

STATIC_INLINE void calcdata_inputs_calc_iat_manifold(ecu_core_ctx_t *ctx)
{
  const uint32_t banks_count = ctx->runtime.global.banks_count;
  ecu_core_runtime_value_ctx_t output_value;
  ecu_core_runtime_value_ctx_t iat_alpha_blending;

  ecu_core_runtime_banked_source_bank_iat_ctx_t *data_iat[ECU_BANK_MAX];
  const ecu_config_calcdata_setup_iat_t *iat_config_base;
  const ecu_config_calcdata_setup_iat_sensor_t *iat_config[ECU_BANK_MAX];
  const ecu_core_runtime_value_ctx_t *ect_value[ECU_BANK_MAX];
  const ecu_core_runtime_value_ctx_t *iat_value[ECU_BANK_MAX];
  ecu_config_io_iat_t iat_type[ECU_BANK_MAX];
  const ckp_data_t *crankshaft;
  float blending, blending_volume_sum, lpf_value;
  bool data_iat_valid;

  iat_config_base = &ctx->calibration->calcdata.setup.iat_model;
  crankshaft = &ctx->timing_data.crankshaft.sensor_data;

  for(ecu_bank_t b = 0; b < banks_count; b++) {
    ect_value[b] = &ctx->runtime.banked.source.banks[b].inputs[CALCDATA_RELATION_INPUT_SOURCE_SENSOR_GLOBAL_ECT].value;
    data_iat[b] = &ctx->runtime.banked.source.banks[b].data_iat;
    iat_config[b] = NULL;

    for(iat_type[b] = 0; iat_type[b] < ECU_CONFIG_IO_IAT_MAX; iat_type[b]++) {
      iat_value[b] = ctx->runtime.banked.raw.banks[b].sensors_iat[iat_type[b]];
      if(iat_value[b] != NULL && iat_value[b]->valid) {
        iat_config[b] = &iat_config_base->sensors[iat_type[b]];
        break;
      }
    }
  }

  for(ecu_bank_t b = 0; b < banks_count; b++) {
    data_iat_valid = false;
    if(iat_config[b] != NULL) {
      (void)core_calcdata_proc_get_output(ctx, b, CALCDATA_OUTPUT_IAT_ALPHA_BLENDING,
          iat_config[b]->alpha_blending.calc_variant, &iat_alpha_blending);
      if(iat_alpha_blending.valid && ect_value[b]->valid && crankshaft->validity >= CKP_DATA_VALID) {
        blending = CLAMP(iat_alpha_blending.value, 0.0f, 1.0f);
        data_iat_valid = true;
        blending_volume_sum = 0;
        for(ecu_config_calcdata_setup_iat_sensor_alpha_blending_filter_number_t f = 0;
            f < CALCDATA_IAT_ALPHA_BLENDING_FILTER_MAX; f++) {
          data_iat[b]->blending[f].unfiltered = blending;
          blending_volume_sum += iat_config[b]->alpha_blending.filters[f].volume;
        }
        if(blending_volume_sum > 0.0f) {
          blending_volume_sum = 1.0f / blending_volume_sum;
        } else {
          data_iat_valid = false;
          data_iat[b]->blending_valid = false;
          for(ecu_config_calcdata_setup_iat_sensor_alpha_blending_filter_number_t f = 0;
              f < CALCDATA_IAT_ALPHA_BLENDING_FILTER_MAX; f++) {
            data_iat[b]->blending[f].filtered = data_iat[b]->blending[f].unfiltered;
          }
        }

        if(data_iat[b]->blending_valid) {
          for(ecu_config_calcdata_setup_iat_sensor_alpha_blending_filter_number_t f = 0;
              f < CALCDATA_IAT_ALPHA_BLENDING_FILTER_MAX; f++) {
            lpf_value = iat_config[b]->alpha_blending.filters[f].low_pass;
            lpf_value *= 1000.0f / crankshaft->period;
            data_iat[b]->blending[f].filtered = BLEND(data_iat[b]->blending[f].unfiltered,
                data_iat[b]->blending[f].filtered, lpf_value);
          }

          blending = 0.0f;
          for(ecu_config_calcdata_setup_iat_sensor_alpha_blending_filter_number_t f = 0;
              f < CALCDATA_IAT_ALPHA_BLENDING_FILTER_MAX; f++) {
            blending += data_iat[b]->blending[f].filtered * iat_config[b]->alpha_blending.filters[f].volume * blending_volume_sum;
          }
        } else {
          for(ecu_config_calcdata_setup_iat_sensor_alpha_blending_filter_number_t f = 0;
              f < CALCDATA_IAT_ALPHA_BLENDING_FILTER_MAX; f++) {
            data_iat[b]->blending[f].filtered = data_iat[b]->blending[f].unfiltered;
          }
        }

        output_value.value = BLEND(iat_value[b]->value, ect_value[b]->value, blending);
        output_value.valid = true;
      } else {
        memcpy(&output_value, iat_value[b], sizeof(output_value));
      }
    } else {
      memcpy(&output_value, iat_value[b], sizeof(output_value));
    }
    data_iat[b]->blending_valid = data_iat_valid;

    (void)core_calcdata_proc_set_input(ctx, b,
        CALCDATA_RELATION_INPUT_SOURCE_CALC_IAT_MANIFOLD, &output_value);
  }

}
