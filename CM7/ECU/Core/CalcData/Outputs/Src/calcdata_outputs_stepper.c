/*
 * calcdata_outputs_stepper.c
 *
 *  Created on: Aug 16, 2025
 *      Author: VHEMaster
 */

#include "calcdata_outputs_stepper.h"
#include "config_global.h"

#include "calcdata_proc.h"

#include "pid.h"

typedef enum {
  STEPPER_PID_P = 0,
  STEPPER_PID_I,
  STEPPER_PID_D,
  STEPPER_PID_MAX,
}stepper_pid_t;

void calcdata_outputs_stepper(ecu_core_ctx_t *ctx)
{
  error_t err;
  time_us_t now = time_now_us();
  const uint32_t banks_count = ctx->runtime.global.banks_count;
  time_float_s_t running_time_current = ctx->runtime.global.misc.running_time_current;
  bool runup_flag = ctx->runtime.global.misc.runup_flag;
  bool idle_flag = ctx->runtime.global.misc.idle_flag;

  const ecu_config_calcdata_setup_idle_t *setup_idle;
  ecu_core_runtime_banked_stepper_bank_ctx_t *stepper_bank_ctx;

  const ecu_core_runtime_value_ctx_t *input_rpm;

  const ecu_core_runtime_value_ctx_t *stepper_stepper_startup_pos;
  const ecu_core_runtime_value_ctx_t *stepper_idle_target_pos;
  const ecu_core_runtime_value_ctx_t *stepper_runup_move_time;

  float *stepper_pid_maf_feedback;
  float *stepper_pid_rpm_feedback;
  float *stepper_pid_feedback;

  const ecu_core_runtime_value_ctx_t *pid_rpm_ka[STEPPER_PID_MAX];
  const ecu_core_runtime_value_ctx_t *pid_maf_ka[STEPPER_PID_MAX];
  math_pid_koffs_t pid_rpm_k;
  math_pid_koffs_t pid_maf_k;

  math_pid_ctx_t *pid_rpm;
  math_pid_ctx_t *pid_maf;

  const ecu_core_runtime_value_ctx_t *input_target_rpm;
  const ecu_core_runtime_value_ctx_t *input_maf;
  const ecu_core_runtime_value_ctx_t *input_target_maf;

  ecu_device_stepper_t stepper_instances[ECU_BANK_MAX];
  ecu_core_runtime_value_ctx_t output_stepper_bank[ECU_BANK_MAX];
  ecu_core_runtime_value_ctx_t output_stepper_pos[ECU_DEVICE_STEPPER_MAX];
  uint32_t output_stepper_pos_values_count[ECU_DEVICE_STEPPER_MAX];
  bool stepper_idlevalve_used = false;
  bool stepper_idlevalve_ok = true;
  bool runup_transition_moving;

  float target_pos;
  float pid_rpm_feedback_pos;
  float pid_maf_feedback_pos;
  float pid_feedback_pos;
  float pos_startup;
  float pos_startup_blending;
  float runup_move_time;

  memset(output_stepper_pos, 0, sizeof(output_stepper_pos));
  memset(output_stepper_bank, 0, sizeof(output_stepper_bank));
  memset(output_stepper_pos_values_count, 0, sizeof(output_stepper_pos_values_count));

  stepper_instances[ECU_BANK_1] = ctx->calibration->io.banked.global.common.device_stepper[ECU_CONFIG_IO_STEPPER_IDLEVALVE];
  if(stepper_instances[ECU_BANK_1] >= ECU_DEVICE_STEPPER_MAX) {
    for(ecu_bank_t bank = 0; bank < banks_count; bank++) {
      stepper_instances[bank] = ctx->calibration->io.banked.banks[bank].common.device_stepper[ECU_CONFIG_IO_STEPPER_IDLEVALVE];
      if(stepper_instances[bank] >= ECU_DEVICE_STEPPER_MAX) {
        stepper_idlevalve_ok = false;
      } else {
        stepper_idlevalve_used = true;
        if(!stepper_idlevalve_ok) {
          //TODO: error flag
        }
      }
    }
  } else {
    stepper_idlevalve_used = true;
    for(ecu_bank_t bank = ECU_BANK_2; bank < banks_count; bank++) {
      stepper_instances[bank] = stepper_instances[ECU_BANK_1];
    }
  }

  if(stepper_idlevalve_used) {
    for(ecu_bank_t bank = 0; bank < banks_count; bank++) {
      if(stepper_instances[bank] < ECU_DEVICE_STEPPER_MAX) {
        (void)ecu_devices_stepper_enable(stepper_instances[bank], true);
      }
    }
  }

  if(stepper_idlevalve_used) {
    setup_idle = &ctx->calibration->calcdata.setup.idle;
    for(ecu_bank_t bank = 0; bank < banks_count; bank++) {
      stepper_bank_ctx = &ctx->runtime.banked.misc.banks[bank].stepper;
      pid_rpm = &stepper_bank_ctx->pid_rpm;
      pid_maf = &stepper_bank_ctx->pid_maf;

      (void)core_calcdata_proc_get_input_ptr(ctx, bank, CALCDATA_RELATION_INPUT_SOURCE_SENSOR_GLOBAL_CKP, &input_rpm);

      (void)core_calcdata_proc_get_output_ptr(ctx, bank, CALCDATA_OUTPUT_STEPPER_IDLEVALVE_STARTUP_POS, &stepper_stepper_startup_pos);
      (void)core_calcdata_proc_get_output_ptr(ctx, bank, CALCDATA_OUTPUT_STEPPER_IDLEVALVE_IDLE_TARGET_POS, &stepper_idle_target_pos);
      (void)core_calcdata_proc_get_output_ptr(ctx, bank, CALCDATA_OUTPUT_STEPPER_IDLEVALVE_RUNUP_MOVE_TIME, &stepper_runup_move_time);

      stepper_pid_rpm_feedback = &stepper_bank_ctx->pid_rpm_feedback;
      stepper_pid_maf_feedback = &stepper_bank_ctx->pid_maf_feedback;
      stepper_pid_feedback = &stepper_bank_ctx->pid_feedback;
      pos_startup = stepper_stepper_startup_pos->value;

      runup_transition_moving = false;
      if(runup_flag) {
        if(stepper_runup_move_time->valid) {
          runup_move_time = stepper_runup_move_time->value;
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

      if(setup_idle->regulator_mode == CALCDATA_IDLE_REGULATOR_STEPPER) {
        (void)core_calcdata_proc_get_output_ptr(ctx, bank, CALCDATA_OUTPUT_IDLE_TARGET_RPM, &input_target_rpm);
        (void)core_calcdata_proc_get_output_ptr(ctx, bank, CALCDATA_OUTPUT_IDLE_TARGET_MASS_AIR_FLOW, &input_target_maf);
        (void)core_calcdata_proc_get_input_ptr(ctx, bank, CALCDATA_RELATION_INPUT_SOURCE_CALC_MASS_AIR_FLOW, &input_maf);

        (void)core_calcdata_proc_get_output_ptr(ctx, bank, CALCDATA_OUTPUT_IDLE_REG_STEPPER_PID_RPM_P, &pid_rpm_ka[STEPPER_PID_P]);
        (void)core_calcdata_proc_get_output_ptr(ctx, bank, CALCDATA_OUTPUT_IDLE_REG_STEPPER_PID_RPM_I, &pid_rpm_ka[STEPPER_PID_I]);
        (void)core_calcdata_proc_get_output_ptr(ctx, bank, CALCDATA_OUTPUT_IDLE_REG_STEPPER_PID_RPM_D, &pid_rpm_ka[STEPPER_PID_D]);
        (void)core_calcdata_proc_get_output_ptr(ctx, bank, CALCDATA_OUTPUT_IDLE_REG_STEPPER_PID_MAF_P, &pid_maf_ka[STEPPER_PID_P]);
        (void)core_calcdata_proc_get_output_ptr(ctx, bank, CALCDATA_OUTPUT_IDLE_REG_STEPPER_PID_MAF_I, &pid_maf_ka[STEPPER_PID_I]);
        (void)core_calcdata_proc_get_output_ptr(ctx, bank, CALCDATA_OUTPUT_IDLE_REG_STEPPER_PID_MAF_D, &pid_maf_ka[STEPPER_PID_D]);

        pid_rpm_k.Kp = pid_rpm_ka[STEPPER_PID_P]->valid ? pid_rpm_ka[STEPPER_PID_P]->value : 0.0f;
        pid_maf_k.Kp = pid_maf_ka[STEPPER_PID_P]->valid ? pid_maf_ka[STEPPER_PID_P]->value : 0.0f;

        if(idle_flag && !runup_transition_moving) {
          pid_rpm_k.Ki = pid_rpm_ka[STEPPER_PID_I]->valid ? pid_rpm_ka[STEPPER_PID_I]->value : 0.0f;
          pid_rpm_k.Kd = pid_rpm_ka[STEPPER_PID_D]->valid ? pid_rpm_ka[STEPPER_PID_D]->value : 0.0f;
          pid_maf_k.Ki = pid_maf_ka[STEPPER_PID_I]->valid ? pid_maf_ka[STEPPER_PID_I]->value : 0.0f;
          pid_maf_k.Kd = pid_maf_ka[STEPPER_PID_D]->valid ? pid_maf_ka[STEPPER_PID_D]->value : 0.0f;
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
        target_pos = stepper_idle_target_pos->value;
        target_pos += pid_feedback_pos;

        if(runup_transition_moving) {
          target_pos = MAX(target_pos, pos_startup);
        }

        output_stepper_bank[bank].value = target_pos;
        output_stepper_bank[bank].valid = true;
      } else {
        target_pos = pos_startup;

        output_stepper_bank[bank].value = target_pos;
        output_stepper_bank[bank].valid = true;
      }

      *stepper_pid_rpm_feedback = pid_rpm_feedback_pos;
      *stepper_pid_maf_feedback = pid_maf_feedback_pos;
      *stepper_pid_feedback = pid_feedback_pos;
    }
  }

  for(ecu_bank_t bank = 0; bank < banks_count; bank++) {
    if(output_stepper_bank[bank].valid) {
      output_stepper_pos[stepper_instances[bank]].valid = true;
      output_stepper_pos[stepper_instances[bank]].value += output_stepper_bank[bank].value;
      output_stepper_pos_values_count[stepper_instances[bank]]++;
    }
  }

  for(ecu_device_stepper_t device = 0; device < ECU_DEVICE_STEPPER_MAX; device++) {
    if(output_stepper_pos[device].valid && output_stepper_pos_values_count[device] > 0) {
      err = ecu_devices_stepper_enable(device, true);
      if(err != E_OK) {
        //TODO: error flag
      }

      target_pos = output_stepper_pos[device].value;
      target_pos /= output_stepper_pos_values_count[device];
      err = ecu_devices_stepper_set_target(device, target_pos);
      if(err != E_OK) {
        //TODO: error flag
      }

    } else {
      (void)ecu_devices_stepper_enable(device, false);
    }
  }

}
