/*
 * calcdata_outputs_etc.c
 *
 *  Created on: Jun 29, 2025
 *      Author: VHEMaster
 */

#include "calcdata_outputs_etc.h"
#include "config_global.h"

#include "calcdata_proc.h"

#include "pid.h"

typedef enum {
  ETC_PID_P = 0,
  ETC_PID_I,
  ETC_PID_D,
  ETC_PID_MAX,
}etc_pid_t;

void calcdata_outputs_etc(ecu_core_ctx_t *ctx)
{
  error_t err;
  time_us_t now = time_now_us();
  const uint32_t banks_count = ctx->runtime.global.banks_count;
  time_float_s_t running_time_current = ctx->runtime.global.misc.running_time_current;
  bool runup_flag = ctx->runtime.global.misc.runup_flag;
  bool idle_flag = ctx->runtime.global.misc.idle_flag;

  const ecu_config_calcdata_setup_idle_t *setup_idle;
  ecu_core_runtime_banked_etc_bank_ctx_t *etc_bank_ctx;

  const ecu_core_runtime_value_ctx_t *input_rpm;
  const ecu_core_runtime_value_ctx_t *input_aps;

  const ecu_core_runtime_value_ctx_t *etc_stopped_pos;
  const ecu_core_runtime_value_ctx_t *etc_etc_startup_pos;
  const ecu_core_runtime_value_ctx_t *etc_idle_target_pos;
  const ecu_core_runtime_value_ctx_t *etc_running_pos;
  const ecu_core_runtime_value_ctx_t *etc_runup_move_time;
  const ecu_core_runtime_value_ctx_t *etc_aps_ignition_ctrl;

  ecu_core_runtime_value_ctx_t *etc_idle_ignition_influence;
  float *etc_pid_maf_feedback;
  float *etc_pid_rpm_feedback;
  float *etc_pid_feedback;

  const ecu_core_runtime_value_ctx_t *pid_rpm_ka[ETC_PID_MAX];
  const ecu_core_runtime_value_ctx_t *pid_maf_ka[ETC_PID_MAX];
  math_pid_koffs_t pid_rpm_k;
  math_pid_koffs_t pid_maf_k;

  math_pid_ctx_t *pid_rpm;
  math_pid_ctx_t *pid_maf;

  const ecu_core_runtime_value_ctx_t *input_target_rpm;
  const ecu_core_runtime_value_ctx_t *input_maf;
  const ecu_core_runtime_value_ctx_t *input_target_maf;

  ecu_module_etc_t etc_instances[ECU_BANK_MAX];
  ecu_sensor_aps_t aps_instance;
  ecu_core_runtime_value_ctx_t output_etc_bank[ECU_BANK_MAX];
  ecu_core_runtime_value_ctx_t output_etc_pos[ECU_MODULE_ETC_MAX];
  uint32_t output_etc_pos_values_count[ECU_MODULE_ETC_MAX];
  bool etc_used = false;
  bool aps_used = false;
  bool etc_ok = true;
  bool runup_transition_moving;

  float target_pos;
  float pid_rpm_feedback_pos;
  float pid_maf_feedback_pos;
  float pid_feedback_pos;
  float value_aps, value_ign_ctrl;
  float pos_startup;
  float pos_startup_blending;
  float runup_move_time;

  memset(output_etc_pos, 0, sizeof(output_etc_pos));
  memset(output_etc_bank, 0, sizeof(output_etc_bank));
  memset(output_etc_pos_values_count, 0, sizeof(output_etc_pos_values_count));

  etc_instances[ECU_BANK_1] = ctx->calibration->io.banked.global.common.module_etc[ECU_CONFIG_IO_ETC_PRIMARY];
  if(etc_instances[ECU_BANK_1] >= ECU_MODULE_ETC_MAX) {
    for(ecu_bank_t bank = 0; bank < banks_count; bank++) {
      etc_instances[bank] = ctx->calibration->io.banked.banks[bank].common.module_etc[ECU_CONFIG_IO_ETC_PRIMARY];
      if(etc_instances[bank] >= ECU_MODULE_ETC_MAX) {
        etc_ok = false;
      } else {
        etc_used = true;
        if(!etc_ok) {
          //TODO: error flag
        }
      }
    }
  } else {
    etc_used = true;
    for(ecu_bank_t bank = ECU_BANK_2; bank < banks_count; bank++) {
      etc_instances[bank] = etc_instances[ECU_BANK_1];
    }
  }

  aps_instance = ctx->calibration->io.banked.global.sensor_aps[ECU_CONFIG_IO_APS_ACCELERATOR];
  if(aps_instance < ECU_SENSOR_APS_MAX) {
    aps_used = true;
  }

  if(aps_used != etc_used) {
    //TODO: error flag
  }

  if(etc_used) {
    for(ecu_bank_t bank = 0; bank < banks_count; bank++) {
      if(etc_instances[bank] < ECU_MODULE_ETC_MAX) {
        (void)ecu_modules_etc_set_enabled(etc_instances[bank], true);
      }
    }
  }

  if(etc_used) {
    setup_idle = &ctx->calibration->calcdata.setup.idle;
    for(ecu_bank_t bank = 0; bank < banks_count; bank++) {
      etc_bank_ctx = &ctx->runtime.banked.misc.banks[bank].etc;
      pid_rpm = &etc_bank_ctx->pid_rpm;
      pid_maf = &etc_bank_ctx->pid_maf;

      (void)core_calcdata_proc_get_input_ptr(ctx, bank, CALCDATA_RELATION_INPUT_SOURCE_SENSOR_GLOBAL_CKP, &input_rpm);
      (void)core_calcdata_proc_get_input_ptr(ctx, bank, CALCDATA_RELATION_INPUT_SOURCE_SENSOR_GLOBAL_APS, &input_aps);

      (void)core_calcdata_proc_get_output_ptr(ctx, bank, CALCDATA_OUTPUT_ETC_STOPPED_POS, &etc_stopped_pos);
      (void)core_calcdata_proc_get_output_ptr(ctx, bank, CALCDATA_OUTPUT_ETC_STARTUP_POS, &etc_etc_startup_pos);
      (void)core_calcdata_proc_get_output_ptr(ctx, bank, CALCDATA_OUTPUT_ETC_IDLE_TARGET_POS, &etc_idle_target_pos);
      (void)core_calcdata_proc_get_output_ptr(ctx, bank, CALCDATA_OUTPUT_ETC_RUNNING_POS, &etc_running_pos);
      (void)core_calcdata_proc_get_output_ptr(ctx, bank, CALCDATA_OUTPUT_ETC_RUNUP_MOVE_TIME, &etc_runup_move_time);
      (void)core_calcdata_proc_get_output_ptr(ctx, bank, CALCDATA_OUTPUT_ETC_APS_IGNITION_CTRL, &etc_aps_ignition_ctrl);

      etc_idle_ignition_influence = &ctx->runtime.banked.misc.banks[bank].ignition.idle_aps_influence;
      etc_pid_rpm_feedback = &etc_bank_ctx->pid_rpm_feedback;
      etc_pid_maf_feedback = &etc_bank_ctx->pid_maf_feedback;
      etc_pid_feedback = &etc_bank_ctx->pid_feedback;
      pos_startup = etc_etc_startup_pos->value;

      if(ctx->calibration->calcdata.setup.idle.ignition_ctrl_source == CALCDATA_IDLE_IGN_CTRL_SRC_APS &&
          input_aps->valid && etc_aps_ignition_ctrl->valid) {
        value_aps = input_aps->value;
        value_ign_ctrl = etc_aps_ignition_ctrl->value;
        if(value_ign_ctrl > 0.0f && value_aps < value_ign_ctrl) {
          etc_idle_ignition_influence->value = 1.0f - (value_aps / value_ign_ctrl);
        } else {
          etc_idle_ignition_influence->value = 0.0f;
        }
        etc_idle_ignition_influence->valid = true;
      }

      runup_transition_moving = false;
      if(runup_flag) {
        if(etc_runup_move_time->valid) {
          runup_move_time = etc_runup_move_time->value;
          if(runup_move_time > 0.0f && runup_move_time > running_time_current) {
            pos_startup_blending = running_time_current / runup_move_time;
            if(pos_startup_blending == pos_startup_blending && pos_startup_blending > 0.0f) {
              pos_startup *= 1.0f - pos_startup_blending;
              runup_transition_moving = true;
            }
          }
        }
      } else {
        runup_transition_moving = true;
      }

      if(setup_idle->regulator_mode == CALCDATA_IDLE_REGULATOR_ETC) {
        (void)core_calcdata_proc_get_output_ptr(ctx, bank, CALCDATA_OUTPUT_IDLE_TARGET_RPM, &input_target_rpm);
        (void)core_calcdata_proc_get_output_ptr(ctx, bank, CALCDATA_OUTPUT_IDLE_TARGET_MASS_AIR_FLOW, &input_target_maf);
        (void)core_calcdata_proc_get_input_ptr(ctx, bank, CALCDATA_RELATION_INPUT_SOURCE_CALC_MASS_AIR_FLOW, &input_maf);

        (void)core_calcdata_proc_get_output_ptr(ctx, bank, CALCDATA_OUTPUT_IDLE_REG_ETC_PID_RPM_P, &pid_rpm_ka[ETC_PID_P]);
        (void)core_calcdata_proc_get_output_ptr(ctx, bank, CALCDATA_OUTPUT_IDLE_REG_ETC_PID_RPM_I, &pid_rpm_ka[ETC_PID_I]);
        (void)core_calcdata_proc_get_output_ptr(ctx, bank, CALCDATA_OUTPUT_IDLE_REG_ETC_PID_RPM_D, &pid_rpm_ka[ETC_PID_D]);
        (void)core_calcdata_proc_get_output_ptr(ctx, bank, CALCDATA_OUTPUT_IDLE_REG_ETC_PID_MAF_P, &pid_maf_ka[ETC_PID_P]);
        (void)core_calcdata_proc_get_output_ptr(ctx, bank, CALCDATA_OUTPUT_IDLE_REG_ETC_PID_MAF_I, &pid_maf_ka[ETC_PID_I]);
        (void)core_calcdata_proc_get_output_ptr(ctx, bank, CALCDATA_OUTPUT_IDLE_REG_ETC_PID_MAF_D, &pid_maf_ka[ETC_PID_D]);

        pid_rpm_k.Kp = pid_rpm_ka[ETC_PID_P]->valid ? pid_rpm_ka[ETC_PID_P]->value : 0.0f;
        pid_maf_k.Kp = pid_maf_ka[ETC_PID_P]->valid ? pid_maf_ka[ETC_PID_P]->value : 0.0f;

        if(idle_flag && !runup_transition_moving) {
          pid_rpm_k.Ki = pid_rpm_ka[ETC_PID_I]->valid ? pid_rpm_ka[ETC_PID_I]->value : 0.0f;
          pid_rpm_k.Kd = pid_rpm_ka[ETC_PID_D]->valid ? pid_rpm_ka[ETC_PID_D]->value : 0.0f;
          pid_maf_k.Ki = pid_maf_ka[ETC_PID_I]->valid ? pid_maf_ka[ETC_PID_I]->value : 0.0f;
          pid_maf_k.Kd = pid_maf_ka[ETC_PID_D]->valid ? pid_maf_ka[ETC_PID_D]->value : 0.0f;
        } else {
          pid_rpm_k.Ki = 0.0f;
          pid_rpm_k.Kd = 0.0f;
          pid_maf_k.Ki = 0.0f;
          pid_maf_k.Kd = 0.0f;
        }

        math_pid_set_clamp(pid_rpm, setup_idle->reg_pid_rpm_min, setup_idle->reg_pid_rpm_max);
        math_pid_set_koffs(pid_rpm, &pid_rpm_k);
        if(runup_flag && setup_idle->use_reg_pid_rpm && input_rpm->valid && input_target_rpm->valid) {
          math_pid_set_target(pid_rpm, input_target_rpm->value);
          pid_rpm_feedback_pos = math_pid_update(pid_rpm, input_rpm->value, now);
        } else {
          pid_rpm_feedback_pos = 0.0f;
          math_pid_reset(pid_rpm, now);
        }

        math_pid_set_clamp(pid_maf, setup_idle->reg_pid_maf_min, setup_idle->reg_pid_maf_max);
        math_pid_set_koffs(pid_maf, &pid_maf_k);
        if(runup_flag && setup_idle->use_reg_pid_maf && input_maf->valid && input_target_maf->valid) {
          math_pid_set_target(pid_maf, input_target_maf->value);
          pid_maf_feedback_pos = math_pid_update(pid_maf, input_maf->value, now);
        } else {
          pid_maf_feedback_pos = 0.0f;
          math_pid_reset(pid_maf, now);
        }

        if(!idle_flag) {
          if(pid_rpm_feedback_pos < 0.0f) {
            pid_rpm_feedback_pos = 0.0f;
          }
          if(pid_maf_feedback_pos < 0.0f) {
            pid_maf_feedback_pos = 0.0f;
          }
        }

      } else {
        pid_rpm_feedback_pos = 0.0f;
        pid_maf_feedback_pos = 0.0f;
        math_pid_reset(pid_rpm, now);
        math_pid_reset(pid_maf, now);
      }
      pid_feedback_pos = pid_rpm_feedback_pos + pid_maf_feedback_pos;
      pid_feedback_pos = CLAMP(pid_feedback_pos, setup_idle->reg_pid_min, setup_idle->reg_pid_max);

      if(runup_flag) {
        target_pos = etc_idle_target_pos->value;
        target_pos += pid_feedback_pos;
        target_pos += etc_running_pos->value * ((100.0f - etc_idle_target_pos->value) * 0.01f);

        if(runup_transition_moving) {
          target_pos = MAX(target_pos, pos_startup);
        }

        output_etc_bank[bank].value = target_pos;
        output_etc_bank[bank].valid = true;
      } else {
        target_pos = pos_startup;
        target_pos += etc_stopped_pos->value * ((100.0f - etc_etc_startup_pos->value) * 0.01f);

        output_etc_bank[bank].value = target_pos;
        output_etc_bank[bank].valid = true;
      }

      *etc_pid_rpm_feedback = pid_rpm_feedback_pos;
      *etc_pid_maf_feedback = pid_maf_feedback_pos;
      *etc_pid_feedback = pid_feedback_pos;
    }
  }

  for(ecu_bank_t bank = 0; bank < banks_count; bank++) {
    if(output_etc_bank[bank].valid) {
      output_etc_pos[etc_instances[bank]].valid = true;
      output_etc_pos[etc_instances[bank]].value += output_etc_bank[bank].value;
      output_etc_pos_values_count[etc_instances[bank]]++;
    }
  }

  for(ecu_module_etc_t module = 0; module < ECU_MODULE_ETC_MAX; module++) {
    if(output_etc_pos[module].valid) {
      err = ecu_modules_etc_set_enabled(module, true);
      if(err != E_OK) {
        //TODO: error flag
      }

      target_pos = output_etc_pos[module].value;
      target_pos /= output_etc_pos_values_count[module];
      err = ecu_modules_etc_set_target_position(module, target_pos);
      if(err != E_OK) {
        //TODO: error flag
      }

    } else {
      (void)ecu_modules_etc_set_enabled(module, false);
    }
  }

}
