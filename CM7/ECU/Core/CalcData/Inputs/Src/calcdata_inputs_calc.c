/*
 * calcdata_inputs_calc.c
 *
 *  Created on: Jun 6, 2025
 *      Author: VHEMaster
 */

#include "calcdata_inputs_calc.h"
#include "config_global.h"

#include "calcdata_proc.h"
#include "math_misc.h"

void calcdata_inputs_calc_runup_flag(ecu_core_ctx_t *ctx)
{
  const ecu_core_runtime_banked_source_ctx_t *ctx_source = &ctx->runtime.banked.source;
  const ecu_config_calcdata_t *ctx_cal_calcdata = &ctx->calibration->calcdata;
  ecu_core_runtime_global_ctx_t *ctx_rt_global = &ctx->runtime.global;

  time_us_t now = time_now_us();
  time_us_t time_delta = time_diff(now, ctx_rt_global->misc.calc_tick_last);
  time_float_s_t runned_time_overall = ctx_rt_global->misc.runned_time_overall;
  time_float_s_t running_time_current = ctx_rt_global->misc.running_time_current;

  const uint32_t banks_count = ctx_rt_global->banks_count;
  const ckp_data_t *ckp_data = &ctx->timing_data.crankshaft.sensor_data;
  float runup_rpm_th_l = ctx_cal_calcdata->setup.runup_rpm_threshold_low;
  float runup_rpm_th_h = ctx_cal_calcdata->setup.runup_rpm_threshold_high;
  const ecu_core_runtime_value_ctx_t *idle_pos_value[ECU_BANK_MAX];
  const ecu_core_runtime_value_ctx_t *rpm_thr_flag_l;
  const ecu_core_runtime_value_ctx_t *rpm_thr_flag_h;
  const ecu_core_runtime_value_ctx_t *rpm_val;

  bool *rpm_idle_flag_ptr = &ctx_rt_global->misc.rpm_idle_flag;
  bool *pos_idle_flag_ptr = &ctx_rt_global->misc.pos_idle_flag;
  bool *runup_flag_ptr = &ctx_rt_global->misc.runup_flag;
  bool *idle_flag_ptr = &ctx_rt_global->misc.idle_flag;
  bool *econ_idle_flag_ptr = &ctx_rt_global->misc.econ_idle_flag;

  bool rpm_idle_flag = *rpm_idle_flag_ptr;
  bool pos_idle_flag = *pos_idle_flag_ptr;
  bool runup_flag = *runup_flag_ptr;
  bool idle_flag = *idle_flag_ptr;
  bool econ_idle_flag = *econ_idle_flag_ptr;
  bool t_pos_idle_flag;
  bool t_rpm_idle_flag;
  float pos_idle_flag_cmp_value;
  float rpm_idle_flag_cmp_value;

  if(ckp_data->validity >= CKP_DATA_VALID) {
    if(ckp_data->rpm > runup_rpm_th_h) {
      runup_flag = true;
    } else if(ckp_data->rpm < runup_rpm_th_l) {
      runup_flag = false;
    }
  } else {
    runup_flag = false;
  }

  if(runup_flag) {
    running_time_current += time_delta * TIME_S_IN_US;
    runned_time_overall += time_delta * TIME_S_IN_US;
  } else {
    running_time_current = 0.0f;
    idle_flag = false;
    rpm_idle_flag = false;
    econ_idle_flag = false;
  }

  switch(ctx_cal_calcdata->setup.idle.pos_flag_mode) {
    case CALCDATA_IDLE_POS_FLAG_MODE_TPS:
      for(ecu_bank_t b = 0; b < banks_count; b++) {
        idle_pos_value[b] = &ctx_source->banks[b].inputs[CALCDATA_RELATION_INPUT_SOURCE_SENSOR_BANKED_TPS].value;
      }
      break;
    case CALCDATA_IDLE_POS_FLAG_MODE_APS:
      for(ecu_bank_t b = 0; b < banks_count; b++) {
        idle_pos_value[b] = &ctx_source->banks[b].inputs[CALCDATA_RELATION_INPUT_SOURCE_SENSOR_GLOBAL_APS].value;
      }
      break;
    case CALCDATA_IDLE_POS_FLAG_MODE_MAP:
      for(ecu_bank_t b = 0; b < banks_count; b++) {
        idle_pos_value[b] = &ctx_source->banks[b].inputs[CALCDATA_RELATION_INPUT_SOURCE_SENSOR_BANKED_MAP].value;
      }
      break;
    case CALCDATA_IDLE_POS_FLAG_MODE_MAF:
      for(ecu_bank_t b = 0; b < banks_count; b++) {
        idle_pos_value[b] = &ctx_source->banks[b].inputs[CALCDATA_RELATION_INPUT_SOURCE_SENSOR_BANKED_MAF].value;
      }
      break;
    case CALCDATA_IDLE_POS_FLAG_MODE_NONE:
    default:
      memset(idle_pos_value, 0, sizeof(idle_pos_value));
      break;
  }

  t_pos_idle_flag = true;
  for(ecu_bank_t b = 0; b < banks_count; b++) {
    if(idle_pos_value[b] != NULL && idle_pos_value[b]->valid) {
      if(pos_idle_flag) {
        pos_idle_flag_cmp_value = ctx_cal_calcdata->setup.idle.pos_flag_thr_h;
      } else {
        pos_idle_flag_cmp_value = ctx_cal_calcdata->setup.idle.pos_flag_thr_l;
      }
      if(idle_pos_value[b]->value > pos_idle_flag_cmp_value) {
        t_pos_idle_flag = false;
        break;
      }
    } else {
      t_pos_idle_flag = false;
      break;
    }
  }
  pos_idle_flag = t_pos_idle_flag;

  t_rpm_idle_flag = true;
  for(ecu_bank_t b = 0; b < banks_count; b++) {
    rpm_val = &ctx_source->banks[b].inputs[CALCDATA_RELATION_INPUT_SOURCE_SENSOR_GLOBAL_CKP].value;
    rpm_thr_flag_l = NULL;
    rpm_thr_flag_h = NULL;
    for(ecu_config_calcdata_output_varianted_index_t v = 0;
        v < ctx_cal_calcdata->output_data.items[CALCDATA_OUTPUT_IDLE_RPM_THR_FLAG_L].variations; v++) {
      rpm_thr_flag_l = &ctx_source->banks[b].outputs[CALCDATA_OUTPUT_IDLE_RPM_THR_FLAG_L].variants[v];
      if(rpm_thr_flag_l->valid) {
        break;
      } else {
        rpm_thr_flag_l = NULL;
      }
    }
    for(ecu_config_calcdata_output_varianted_index_t v = 0;
        v < ctx_cal_calcdata->output_data.items[CALCDATA_OUTPUT_IDLE_RPM_THR_FLAG_H].variations; v++) {
      rpm_thr_flag_h = &ctx_source->banks[b].outputs[CALCDATA_OUTPUT_IDLE_RPM_THR_FLAG_H].variants[v];
      if(rpm_thr_flag_h->valid) {
        break;
      } else {
        rpm_thr_flag_h = NULL;
      }
    }

    if(rpm_val->valid == false || rpm_thr_flag_l == NULL || rpm_thr_flag_h == NULL) {
      t_rpm_idle_flag = false;
      break;
    }

    if(rpm_idle_flag) {
      rpm_idle_flag_cmp_value = rpm_thr_flag_h->value;
    } else {
      rpm_idle_flag_cmp_value = rpm_thr_flag_l->value;
    }
    if(rpm_val->value > rpm_idle_flag_cmp_value) {
      t_rpm_idle_flag = false;
      break;
    }
  }
  rpm_idle_flag = t_rpm_idle_flag;

  idle_flag = runup_flag && pos_idle_flag && rpm_idle_flag;
  econ_idle_flag = runup_flag && pos_idle_flag && !rpm_idle_flag;

  ctx_rt_global->misc.runned_time_overall = runned_time_overall;
  ctx_rt_global->misc.running_time_current = running_time_current;
  ctx_rt_global->misc.calc_tick_last = now;

  *idle_flag_ptr = idle_flag;
  *runup_flag_ptr = runup_flag;
  *pos_idle_flag_ptr = pos_idle_flag;
  *rpm_idle_flag_ptr = rpm_idle_flag;
  *econ_idle_flag_ptr = econ_idle_flag;
}

void calcdata_inputs_calc_iat_manifold(ecu_core_ctx_t *ctx)
{
  const uint32_t banks_count = ctx->runtime.global.banks_count;
  ecu_core_runtime_value_ctx_t output_value;
  const ecu_core_runtime_value_ctx_t *iat_alpha_blending;

  const ecu_core_runtime_banked_source_bank_ctx_t *bank_ctx;
  ecu_core_runtime_banked_source_bank_iat_ctx_t *data_iat[ECU_BANK_MAX];
  const ecu_config_calcdata_setup_iat_t *iat_config_base;
  const ecu_config_calcdata_setup_iat_sensor_t *iat_config[ECU_BANK_MAX];
  const ecu_core_runtime_value_ctx_t *ect_value[ECU_BANK_MAX];
  const ecu_core_runtime_value_ctx_t *iat_value[ECU_BANK_MAX];
  ecu_config_io_iat_t iat_type[ECU_BANK_MAX];
  ecu_config_calcdata_output_varianted_index_t calc_variant;
  const ckp_data_t *crankshaft;
  float blending, blending_volume_sum, lpf_value;
  bool data_iat_valid;

  iat_config_base = &ctx->calibration->calcdata.setup.iat_model;
  crankshaft = &ctx->timing_data.crankshaft.sensor_data;

  for(ecu_bank_t b = 0; b < banks_count; b++) {
    ect_value[b] = &ctx->runtime.banked.source.banks[b].inputs[CALCDATA_RELATION_INPUT_SOURCE_SENSOR_GLOBAL_ECT].value;
    data_iat[b] = &ctx->runtime.banked.source.banks[b].data_iat;

    iat_type[b] = data_iat[b]->active_type;
    iat_value[b] = ctx->runtime.banked.raw.banks[b].sensors_iat[iat_type[b]];
    iat_config[b] = &iat_config_base->sensors[iat_type[b]];
  }

  for(ecu_bank_t b = 0; b < banks_count; b++) {
    data_iat_valid = false;
    if(iat_config[b] != NULL) {

      bank_ctx = &ctx->runtime.banked.source.banks[b];
      calc_variant = iat_config[b]->alpha_blending.calc_variant;
      iat_alpha_blending = &bank_ctx->outputs[CALCDATA_OUTPUT_IAT_ALPHA_BLENDING].variants[calc_variant];

      if(iat_alpha_blending->valid && ect_value[b]->valid && crankshaft->validity >= CKP_DATA_VALID) {
        blending = CLAMP(iat_alpha_blending->value, 0.0f, 1.0f);
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
            data_iat[b]->blending[f].filtered = BLEND(data_iat[b]->blending[f].filtered,
                data_iat[b]->blending[f].unfiltered, lpf_value);
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

        output_value.value = BLEND(ect_value[b]->value, iat_value[b]->value, blending);
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

void calcdata_inputs_calc_cycle_charge(ecu_core_ctx_t *ctx)
{
  const uint32_t banks_count = ctx->runtime.global.banks_count;
  const uint32_t cylinders_count = ctx->runtime.global.cylinders_count;
  const ecu_core_runtime_banked_source_bank_ctx_t *bank_ctx;
  const ecu_config_calcdata_setup_air_calc_model_source_t *source_ptr;
  const ecu_config_calcdata_setup_air_calc_model_t *air_calc_model;
  const ecu_config_cylinders_t *cy_config;
  ecu_core_runtime_banked_source_bank_air_calc_ctx_t *air_calc_ctx;
  const ecu_core_runtime_value_ctx_t *iat_value;
  const ecu_core_runtime_value_ctx_t *input_value;
  const ecu_core_runtime_value_ctx_t *iat_influence;
  ecu_core_runtime_value_ctx_t output_value;
  float air_density;
  float air_density_norm;
  float cy_displacement_cc;

  cy_config = &ctx->calibration->cylinders;
  cy_displacement_cc = cy_config->engine_displacement / cylinders_count;

  for(ecu_bank_t b = 0; b < banks_count; b++) {
    memset(&output_value, 0, sizeof(output_value));

    for(ecu_config_calcdata_setup_air_calc_model_index_t i = 0;
        i < CALCDATA_AIR_CALC_MODEL_INDEX_MAX; i++) {

      air_calc_model = &ctx->calibration->calcdata.setup.air_calc_model;
      source_ptr = &air_calc_model->sources[i];
      air_calc_ctx = &ctx->runtime.banked.source.banks[b].data_air_calc;

      if(source_ptr->model != CALCDATA_AIR_CALC_MODEL_SOURCE_NONE &&
          source_ptr->model < CALCDATA_AIR_CALC_MODEL_SOURCE_MAX) {

        bank_ctx = &ctx->runtime.banked.source.banks[b];
        iat_value = &bank_ctx->inputs[CALCDATA_RELATION_INPUT_SOURCE_CALC_IAT_MANIFOLD].value;
        input_value = &bank_ctx->outputs[CALCDATA_OUTPUT_VOLUMETRIC_EFFICIENCY].variants[source_ptr->ve_out_variant];

        air_density_norm = math_calc_air_density_mgcc(air_calc_model->normal_conditions.air_temperature,
            air_calc_model->normal_conditions.absolute_pressure);

        switch(source_ptr->iat_influence.direct_type) {
          case CALCDATA_IAT_DIRECT_INFLUENCE_TYPE_RECALC:
            if(iat_value->valid) {
              air_density = math_calc_air_density_mgcc(iat_value->value,
                  air_calc_model->normal_conditions.absolute_pressure);
            } else {
              air_density = air_density_norm;
            }
            break;
          case CALCDATA_IAT_DIRECT_INFLUENCE_TYPE_TABLE:
            air_density = air_density_norm;
            iat_influence = &bank_ctx->outputs[CALCDATA_OUTPUT_IAT_DIRECT_INFLUENCE].variants[source_ptr->iat_influence.direct_table_variant];
            if(iat_influence->valid) {
              air_density *= iat_influence->value;
            }
            break;
          case CALCDATA_IAT_DIRECT_INFLUENCE_TYPE_NONE:
          default:
            air_density = air_density_norm;
            break;
        }

        if(input_value->valid) {
          switch(source_ptr->model) {
            case CALCDATA_AIR_CALC_MODEL_SOURCE_MAP:
            case CALCDATA_AIR_CALC_MODEL_SOURCE_TPS:
            case CALCDATA_AIR_CALC_MODEL_SOURCE_MAF:
              output_value.value = input_value->value * air_density * cy_displacement_cc;
              output_value.valid = true;

              air_calc_ctx->active_index = i;
              air_calc_ctx->active_model = source_ptr->model;
              break;
            default:
              continue;
          }
          break;
        }
      } else {
        break;
      }
    }

    if(!output_value.valid) {
      air_calc_ctx->active_index = CALCDATA_AIR_CALC_MODEL_INDEX_MAX;
      air_calc_ctx->active_model = CALCDATA_AIR_CALC_MODEL_SOURCE_NONE;
    }

    (void)core_calcdata_proc_set_input(ctx, b,
        CALCDATA_RELATION_INPUT_SOURCE_CALC_CYCLE_CHARGE, &output_value);
  }
}

void calcdata_inputs_calc_mass_air_flow(ecu_core_ctx_t *ctx)
{
  const uint32_t cylinders_count = ctx->runtime.global.cylinders_count;
  const uint32_t banks_count = ctx->runtime.global.banks_count;
  ecu_core_runtime_value_ctx_t output_value;
  const ecu_core_runtime_value_ctx_t *input_ckp_rpm;
  const ecu_core_runtime_value_ctx_t *input_cycle_charge;
  const ecu_core_runtime_banked_source_bank_input_ctx_t *inputs;
  uint32_t cylinders_per_bank;

  for(ecu_bank_t b = 0; b < banks_count; b++) {
    memset(&output_value, 0, sizeof(output_value));

    cylinders_per_bank = 0;
    for(uint32_t c = 0; c < cylinders_count; c++) {
      if(ctx->calibration->cylinders.cylinders[c].bank == b) {
        cylinders_per_bank++;
      }
    }

    inputs = ctx->runtime.banked.source.banks[b].inputs;

    input_ckp_rpm = &inputs[CALCDATA_RELATION_INPUT_SOURCE_SENSOR_GLOBAL_CKP].value;
    input_cycle_charge = &inputs[CALCDATA_RELATION_INPUT_SOURCE_CALC_CYCLE_CHARGE].value;

    if(input_ckp_rpm->valid && input_cycle_charge->valid) {
      output_value.value = input_cycle_charge->value;
      output_value.value *= input_ckp_rpm->value * 0.5f;
      output_value.value *= cylinders_per_bank;
      output_value.value *= 0.00006f;
      output_value.valid = true;
    } else {
      memset(&output_value, 0, sizeof(output_value));
    }

    (void)core_calcdata_proc_set_input(ctx, b,
        CALCDATA_RELATION_INPUT_SOURCE_CALC_MASS_AIR_FLOW, &output_value);
  }
}

void calcdata_inputs_calc_idle_target_rpm_deviation(ecu_core_ctx_t *ctx)
{
  const uint32_t banks_count = ctx->runtime.global.banks_count;
  ecu_core_runtime_value_ctx_t output_value;
  const ecu_core_runtime_value_ctx_t *input_rpm;
  const ecu_core_runtime_value_ctx_t *input_idle_target_rpm;
  float value;

  for(ecu_bank_t b = 0; b < banks_count; b++) {
    (void)core_calcdata_proc_get_input_ptr(ctx, b, CALCDATA_RELATION_INPUT_SOURCE_SENSOR_GLOBAL_CKP, &input_rpm);
    (void)core_calcdata_proc_get_output_ptr(ctx, b, CALCDATA_OUTPUT_IDLE_TARGET_RPM, &input_idle_target_rpm);

    if(input_idle_target_rpm->valid && input_rpm->valid) {
      value = input_rpm->value / input_idle_target_rpm->value;
      if(value == value) {
        output_value.value = value;
        output_value.valid = true;
      } else {
        output_value.value = 1.0f;
        output_value.valid = true;
      }
    } else {
      output_value.value = 1.0f;
      output_value.valid = true;
    }

    (void)core_calcdata_proc_set_input(ctx, b,
        CALCDATA_RELATION_INPUT_SOURCE_CALC_IDLE_TARGET_RPM_DEVIATION, &output_value);
  }
}
