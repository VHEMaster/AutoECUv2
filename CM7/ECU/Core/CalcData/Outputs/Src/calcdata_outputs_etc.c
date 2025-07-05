/*
 * calcdata_outputs_etc.c
 *
 *  Created on: Jun 29, 2025
 *      Author: VHEMaster
 */

#include "calcdata_outputs_etc.h"
#include "config_global.h"

#include "calcdata_proc.h"

void calcdata_outputs_etc(ecu_core_ctx_t *ctx)
{
  error_t err;
  const uint32_t banks_count = ctx->runtime.global.banks_count;
  time_float_s_t runned_time_overall = ctx->runtime.global.misc.runned_time_overall;
  time_float_s_t running_time_current = ctx->runtime.global.misc.running_time_current;
  bool runup_flag = ctx->runtime.global.misc.runup_flag;
  bool idle_flag = ctx->runtime.global.misc.idle_flag;

  const ecu_core_runtime_value_ctx_t *input_rpm;
  const ecu_core_runtime_value_ctx_t *input_aps;

  const ecu_core_runtime_value_ctx_t *etc_stopped_pos;
  const ecu_core_runtime_value_ctx_t *etc_etc_startup_pos;
  const ecu_core_runtime_value_ctx_t *etc_idle_target_pos;
  const ecu_core_runtime_value_ctx_t *etc_running_pos;
  const ecu_core_runtime_value_ctx_t *etc_runup_move_time;
  const ecu_core_runtime_value_ctx_t *etc_aps_ignition_ctrl;
  const ecu_core_runtime_value_ctx_t *etc_pid_feedback;

  const ecu_core_runtime_value_ctx_t *input_target_rpm;
  const ecu_core_runtime_value_ctx_t *input_mass_air_flow;
  const ecu_core_runtime_value_ctx_t *input_target_mass_air_flow;

  ecu_module_etc_t etc_instances[ECU_BANK_MAX];
  ecu_sensor_aps_t aps_instance;
  ecu_core_runtime_value_ctx_t output_etc_bank[ECU_BANK_MAX];
  ecu_core_runtime_value_ctx_t output_etc_pos[ECU_MODULE_ETC_MAX];
  uint32_t output_etc_pos_values_count[ECU_MODULE_ETC_MAX];
  bool etc_used = false;
  bool aps_used = false;
  bool etc_ok = true;

  float target_pos;
  float pid_feedback_pos;

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
    for(ecu_bank_t bank = 0; bank < banks_count; bank++) {
      (void)core_calcdata_proc_get_input_ptr(ctx, bank, CALCDATA_RELATION_INPUT_SOURCE_SENSOR_GLOBAL_CKP, &input_rpm);
      (void)core_calcdata_proc_get_input_ptr(ctx, bank, CALCDATA_RELATION_INPUT_SOURCE_SENSOR_GLOBAL_APS, &input_aps);

      (void)core_calcdata_proc_get_output_ptr(ctx, bank, CALCDATA_OUTPUT_ETC_STOPPED_POS, &etc_stopped_pos);
      (void)core_calcdata_proc_get_output_ptr(ctx, bank, CALCDATA_OUTPUT_ETC_STARTUP_POS, &etc_etc_startup_pos);
      (void)core_calcdata_proc_get_output_ptr(ctx, bank, CALCDATA_OUTPUT_ETC_IDLE_TARGET_POS, &etc_idle_target_pos);
      (void)core_calcdata_proc_get_output_ptr(ctx, bank, CALCDATA_OUTPUT_ETC_RUNNING_POS, &etc_running_pos);
      (void)core_calcdata_proc_get_output_ptr(ctx, bank, CALCDATA_OUTPUT_ETC_RUNUP_MOVE_TIME, &etc_runup_move_time);
      (void)core_calcdata_proc_get_output_ptr(ctx, bank, CALCDATA_OUTPUT_ETC_APS_IGNITION_CTRL, &etc_aps_ignition_ctrl);

      (void)core_calcdata_proc_get_output_ptr(ctx, bank, CALCDATA_OUTPUT_IDLE_TARGET_RPM, &input_target_rpm);
      (void)core_calcdata_proc_get_output_ptr(ctx, bank, CALCDATA_OUTPUT_IDLE_TARGET_MASS_AIR_FLOW, &input_target_mass_air_flow);
      (void)core_calcdata_proc_get_input_ptr(ctx, bank, CALCDATA_RELATION_INPUT_SOURCE_CALC_MASS_AIR_FLOW, &input_mass_air_flow);

      ctx->runtime.banked.etc.banks[bank].idle_ignition_influence = *etc_aps_ignition_ctrl;
      etc_pid_feedback = &ctx->runtime.banked.etc.banks[bank].pid_feedback;

      if(runup_flag) {

        if(idle_flag) {
          if(etc_pid_feedback->valid) {
            pid_feedback_pos = ctx->runtime.banked.etc.banks[bank].pid_feedback.value;
          } else {
            pid_feedback_pos = 0.0f;
          }

          target_pos = etc_idle_target_pos->value;
          target_pos += pid_feedback_pos;
        } else {
          target_pos = etc_idle_target_pos->value;
          target_pos += etc_running_pos->value * ((100.0f - etc_etc_startup_pos->value) * 0.01f);
        }

        output_etc_bank[bank].value = target_pos;
        output_etc_bank[bank].valid = true;
      } else {
        target_pos = etc_etc_startup_pos->value;
        target_pos += etc_stopped_pos->value * ((100.0f - etc_etc_startup_pos->value) * 0.01f);

        output_etc_bank[bank].value = target_pos;
        output_etc_bank[bank].valid = true;
      }

      //TODO: IMPLEMENT
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
