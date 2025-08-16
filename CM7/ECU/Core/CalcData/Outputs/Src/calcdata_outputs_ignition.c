/*
 * calcdata_outputs_ignition.c
 *
 *  Created on: Jun 22, 2025
 *      Author: VHEMaster
 */

#include "calcdata_outputs_ignition.h"
#include "config_global.h"

#include "calcdata_proc.h"

typedef enum {
  IGN_PID_P = 0,
  IGN_PID_I,
  IGN_PID_D,
  IGN_PID_MAX,
}ign_pid_t;

void calcdata_outputs_ignition(ecu_core_ctx_t *ctx)
{
  time_us_t now = time_now_us();
  const uint32_t banks_count = ctx->runtime.global.banks_count;
  const ecu_config_calcdata_setup_idle_t *setup_idle;

  const ecu_core_runtime_value_ctx_t *input_value_run;
  const ecu_core_runtime_value_ctx_t *input_value_start;
  const ecu_core_runtime_value_ctx_t *runup_ign_duration;
  const ecu_core_runtime_value_ctx_t *runup_ign_advance;

  const ecu_core_runtime_value_ctx_t *input_rpm;
  const ecu_core_runtime_value_ctx_t *input_target_rpm;
  const ecu_core_runtime_value_ctx_t *input_idle_target_ignition_advance;
  const ecu_core_runtime_value_ctx_t *input_idle_ignition_advance;

  const ecu_core_runtime_value_ctx_t *idle_pos_influence;

  ecu_core_runtime_banked_ignition_bank_ctx_t *ign_bank_ctx;

  ecu_core_runtime_value_ctx_t output_value;
  ecu_core_runtime_global_parameters_timing_ignition_ctx_t *output_ptr;
  bool runup_flag = ctx->runtime.global.misc.runup_flag;
  bool idle_flag = ctx->runtime.global.misc.idle_flag;
  bool pos_idle_flag = ctx->runtime.global.misc.pos_idle_flag;
  time_float_s_t running_time_current = ctx->runtime.global.misc.running_time_current;
  float time_blending;
  float input_run_advance;
  float input_idle_initial_advance;
  float input_idle_target_advance;
  float runup_ign_duration_val;
  float pid_idle_rpm_advance;
  const ecu_core_runtime_value_ctx_t *pid_rpm_ka[IGN_PID_MAX];
  math_pid_koffs_t pid_rpm_k;
  math_pid_ctx_t *pid_rpm;

  output_ptr = &ctx->runtime.global.parameters.timings.ignition;
  setup_idle = &ctx->calibration->calcdata.setup.idle;

  for(ecu_bank_t bank = 0; bank < banks_count; bank++) {
    memset(&output_value, 0, sizeof(output_value));
    ign_bank_ctx = &ctx->runtime.banked.misc.banks[bank].ignition;
    pid_rpm = &ign_bank_ctx->pid_rpm;

    (void)core_calcdata_proc_get_output_ptr(ctx, bank, CALCDATA_OUTPUT_IGNITION_ADVANCE, &input_value_run);
    (void)core_calcdata_proc_get_output_ptr(ctx, bank, CALCDATA_OUTPUT_STARTUP_IGNITION_ADVANCE, &input_value_start);
    (void)core_calcdata_proc_get_output_ptr(ctx, bank, CALCDATA_OUTPUT_STARTUP_RUNUP_IGN_DURATION, &runup_ign_duration);
    (void)core_calcdata_proc_get_output_ptr(ctx, bank, CALCDATA_OUTPUT_STARTUP_RUNUP_IGN_ADVANCE, &runup_ign_advance);

    (void)core_calcdata_proc_get_input_ptr(ctx, bank, CALCDATA_RELATION_INPUT_SOURCE_SENSOR_GLOBAL_CKP, &input_rpm);
    (void)core_calcdata_proc_get_output_ptr(ctx, bank, CALCDATA_OUTPUT_IDLE_TARGET_RPM, &input_target_rpm);
    (void)core_calcdata_proc_get_output_ptr(ctx, bank, CALCDATA_OUTPUT_IDLE_TARGET_IGNITION_ADVANCE, &input_idle_target_ignition_advance);
    (void)core_calcdata_proc_get_output_ptr(ctx, bank, CALCDATA_OUTPUT_IDLE_INITIAL_IGNITION_ADVANCE, &input_idle_ignition_advance);

    switch(setup_idle->ignition_ctrl_source) {
      case CALCDATA_IDLE_IGN_CTRL_SRC_APS:
        idle_pos_influence = &ctx->runtime.banked.misc.banks[bank].ignition.idle_aps_influence;
        break;
      case CALCDATA_IDLE_IGN_CTRL_SRC_TPS:
        idle_pos_influence = &ctx->runtime.banked.misc.banks[bank].ignition.idle_tps_influence;
        break;
      default:
        idle_pos_influence = NULL;
        break;
    }

    if(runup_flag) {
      input_run_advance = input_value_run->value;
      if(setup_idle->use_initial_ign_advance) {
        input_idle_initial_advance = input_idle_ignition_advance->value;
      } else {
        input_idle_initial_advance = input_run_advance;
      }
      input_idle_target_advance = input_idle_target_ignition_advance->value;
      if(pos_idle_flag && runup_ign_duration->valid) {
        runup_ign_duration_val = runup_ign_duration->value;
        if(running_time_current < runup_ign_duration_val && runup_ign_duration_val > 0.0f &&
          runup_ign_advance->valid && input_target_rpm->valid) {
          time_blending = running_time_current / runup_ign_duration_val;
          if(time_blending == time_blending && time_blending > 0.0f) {
            output_value.value = BLEND(runup_ign_advance->value, input_target_rpm->value, time_blending);
            output_value.valid = true;
          }
        }
      }

      if(setup_idle->use_ign_pid_rpm) {
        (void)core_calcdata_proc_get_output_ptr(ctx, bank, CALCDATA_OUTPUT_IDLE_IGN_ADV_PID_RPM_P, &pid_rpm_ka[IGN_PID_P]);
        (void)core_calcdata_proc_get_output_ptr(ctx, bank, CALCDATA_OUTPUT_IDLE_IGN_ADV_PID_RPM_I, &pid_rpm_ka[IGN_PID_I]);
        (void)core_calcdata_proc_get_output_ptr(ctx, bank, CALCDATA_OUTPUT_IDLE_IGN_ADV_PID_RPM_D, &pid_rpm_ka[IGN_PID_D]);

        if(idle_flag && !output_value.valid) {
          pid_rpm_k.Kp = pid_rpm_ka[IGN_PID_P]->valid ? pid_rpm_ka[IGN_PID_P]->value : 0.0f;
          pid_rpm_k.Ki = pid_rpm_ka[IGN_PID_I]->valid ? pid_rpm_ka[IGN_PID_I]->value : 0.0f;
          pid_rpm_k.Kd = pid_rpm_ka[IGN_PID_D]->valid ? pid_rpm_ka[IGN_PID_D]->value : 0.0f;
        } else {
          memset(&pid_rpm_k, 0, sizeof(pid_rpm_k));
        }

        math_pid_set_clamp(pid_rpm, setup_idle->ign_pid_rpm_min, setup_idle->ign_pid_rpm_max);
        math_pid_set_koffs(pid_rpm, &pid_rpm_k);

        if(runup_flag && input_rpm->valid && input_target_rpm->valid) {
          math_pid_set_target(pid_rpm, input_target_rpm->value);
          pid_idle_rpm_advance = math_pid_update(pid_rpm, input_rpm->value, now);
        } else {
          pid_idle_rpm_advance = 0.0f;
          math_pid_reset(pid_rpm, now);
        }
      } else {
        pid_idle_rpm_advance = 0.0f;
        math_pid_reset(pid_rpm, now);
      }

      input_idle_target_advance += pid_idle_rpm_advance;

      if(!output_value.valid) {
        if(idle_flag) {
          output_value.value = input_idle_target_advance;
          output_value.valid = true;
        } else if(pos_idle_flag) {
          output_value.value = input_idle_initial_advance;
          output_value.valid = true;
        } else {
          if(idle_pos_influence != NULL && idle_pos_influence->valid) {
            output_value.value = BLEND(input_run_advance,
              input_idle_initial_advance,
              idle_pos_influence->value);
          } else {
            output_value.value = input_run_advance;
          }
          output_value.valid = true;
        }
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
