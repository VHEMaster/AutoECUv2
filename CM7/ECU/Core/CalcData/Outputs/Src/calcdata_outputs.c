/*
 * calcdata_outputs.c
 *
 *  Created on: Jun 8, 2025
 *      Author: VHEMaster
 */

#include "calcdata_outputs.h"
#include "calcdata_proc.h"
#include "config_global.h"

#include "calcdata_outputs_ignition.h"
#include "calcdata_outputs_injection.h"
#include "calcdata_outputs_idle.h"
#include "calcdata_outputs_etc.h"

void core_calcdata_outputs_process(ecu_core_ctx_t *ctx, ecu_core_calcdata_outputs_stage_t stage)
{
  switch(stage) {
    case CORE_CALCDATA_OUTPUTS_STAGE_1:
      (void)core_calcdata_proc_calc_output(ctx, CALCDATA_OUTPUT_IAT_ALPHA_BLENDING, CALCDATA_OUTPUT_VARIANTED_ITEM_MAX, NULL);
      (void)core_calcdata_proc_calc_output(ctx, CALCDATA_OUTPUT_IAT_DIRECT_INFLUENCE, CALCDATA_OUTPUT_VARIANTED_ITEM_MAX, NULL);

      (void)core_calcdata_proc_calc_output(ctx, CALCDATA_OUTPUT_STARTUP_IGNITION_ADVANCE, CALCDATA_OUTPUT_VARIANTED_ITEM_MAX, NULL);
      (void)core_calcdata_proc_calc_output(ctx, CALCDATA_OUTPUT_STARTUP_INJECTION_PHASE, CALCDATA_OUTPUT_VARIANTED_ITEM_MAX, NULL);
      (void)core_calcdata_proc_calc_output(ctx, CALCDATA_OUTPUT_STARTUP_TPS_INJ_INFLUENCE, CALCDATA_OUTPUT_VARIANTED_ITEM_MAX, NULL);
      (void)core_calcdata_proc_calc_output(ctx, CALCDATA_OUTPUT_STARTUP_LARGE_INJ_CHARGE, CALCDATA_OUTPUT_VARIANTED_ITEM_MAX, NULL);
      (void)core_calcdata_proc_calc_output(ctx, CALCDATA_OUTPUT_STARTUP_SMALL_INJ_CHARGE, CALCDATA_OUTPUT_VARIANTED_ITEM_MAX, NULL);
      (void)core_calcdata_proc_calc_output(ctx, CALCDATA_OUTPUT_STARTUP_RUNUP_INJ_DURATION, CALCDATA_OUTPUT_VARIANTED_ITEM_MAX, NULL);
      (void)core_calcdata_proc_calc_output(ctx, CALCDATA_OUTPUT_STARTUP_RUNUP_IGN_DURATION, CALCDATA_OUTPUT_VARIANTED_ITEM_MAX, NULL);
      (void)core_calcdata_proc_calc_output(ctx, CALCDATA_OUTPUT_STARTUP_COLD_INJ_CORR, CALCDATA_OUTPUT_VARIANTED_ITEM_MAX, NULL);
      (void)core_calcdata_proc_calc_output(ctx, CALCDATA_OUTPUT_STARTUP_COLD_INJ_TIME, CALCDATA_OUTPUT_VARIANTED_ITEM_MAX, NULL);
      (void)core_calcdata_proc_calc_output(ctx, CALCDATA_OUTPUT_STARTUP_ECON_BYPASS_TIME, CALCDATA_OUTPUT_VARIANTED_ITEM_MAX, NULL);

      (void)core_calcdata_proc_calc_output(ctx, CALCDATA_OUTPUT_WARMUP_IDLE_INJ_CORR, CALCDATA_OUTPUT_VARIANTED_ITEM_MAX, NULL);

      (void)core_calcdata_proc_calc_output(ctx, CALCDATA_OUTPUT_IDLE_RPM_THR_FLAG_L, CALCDATA_OUTPUT_VARIANTED_ITEM_MAX, NULL);
      (void)core_calcdata_proc_calc_output(ctx, CALCDATA_OUTPUT_IDLE_RPM_THR_FLAG_H, CALCDATA_OUTPUT_VARIANTED_ITEM_MAX, NULL);
      (void)core_calcdata_proc_calc_output(ctx, CALCDATA_OUTPUT_IDLE_TARGET_RPM, CALCDATA_OUTPUT_VARIANTED_ITEM_MAX, NULL);
      (void)core_calcdata_proc_calc_output(ctx, CALCDATA_OUTPUT_IDLE_TARGET_IGNITION_ADVANCE, CALCDATA_OUTPUT_VARIANTED_ITEM_MAX, NULL);
      (void)core_calcdata_proc_calc_output(ctx, CALCDATA_OUTPUT_IDLE_TARGET_MASS_AIR_FLOW, CALCDATA_OUTPUT_VARIANTED_ITEM_MAX, NULL);
      (void)core_calcdata_proc_calc_output(ctx, CALCDATA_OUTPUT_IDLE_INITIAL_IGNITION_ADVANCE, CALCDATA_OUTPUT_VARIANTED_ITEM_MAX, NULL);

      if(ctx->calibration->calcdata.setup.idle.use_reg_pid_rpm) {
        switch(ctx->calibration->calcdata.setup.idle.regulator_mode) {
          case CALCDATA_IDLE_REGULATOR_STEPPER:
            (void)core_calcdata_proc_calc_output(ctx, CALCDATA_OUTPUT_IDLE_REG_STEPPER_PID_RPM_P, CALCDATA_OUTPUT_VARIANTED_ITEM_MAX, NULL);
            (void)core_calcdata_proc_calc_output(ctx, CALCDATA_OUTPUT_IDLE_REG_STEPPER_PID_RPM_I, CALCDATA_OUTPUT_VARIANTED_ITEM_MAX, NULL);
            (void)core_calcdata_proc_calc_output(ctx, CALCDATA_OUTPUT_IDLE_REG_STEPPER_PID_RPM_D, CALCDATA_OUTPUT_VARIANTED_ITEM_MAX, NULL);
            break;
          case CALCDATA_IDLE_REGULATOR_ETC:
            (void)core_calcdata_proc_calc_output(ctx, CALCDATA_OUTPUT_IDLE_REG_ETC_PID_RPM_P, CALCDATA_OUTPUT_VARIANTED_ITEM_MAX, NULL);
            (void)core_calcdata_proc_calc_output(ctx, CALCDATA_OUTPUT_IDLE_REG_ETC_PID_RPM_I, CALCDATA_OUTPUT_VARIANTED_ITEM_MAX, NULL);
            (void)core_calcdata_proc_calc_output(ctx, CALCDATA_OUTPUT_IDLE_REG_ETC_PID_RPM_D, CALCDATA_OUTPUT_VARIANTED_ITEM_MAX, NULL);
            break;
          default:
            break;
        }
      }
      if(ctx->calibration->calcdata.setup.idle.use_reg_pid_maf) {
        switch(ctx->calibration->calcdata.setup.idle.regulator_mode) {
          case CALCDATA_IDLE_REGULATOR_STEPPER:
            (void)core_calcdata_proc_calc_output(ctx, CALCDATA_OUTPUT_IDLE_REG_STEPPER_PID_MAF_P, CALCDATA_OUTPUT_VARIANTED_ITEM_MAX, NULL);
            (void)core_calcdata_proc_calc_output(ctx, CALCDATA_OUTPUT_IDLE_REG_STEPPER_PID_MAF_I, CALCDATA_OUTPUT_VARIANTED_ITEM_MAX, NULL);
            (void)core_calcdata_proc_calc_output(ctx, CALCDATA_OUTPUT_IDLE_REG_STEPPER_PID_MAF_D, CALCDATA_OUTPUT_VARIANTED_ITEM_MAX, NULL);
            break;
          case CALCDATA_IDLE_REGULATOR_ETC:
            (void)core_calcdata_proc_calc_output(ctx, CALCDATA_OUTPUT_IDLE_REG_ETC_PID_MAF_P, CALCDATA_OUTPUT_VARIANTED_ITEM_MAX, NULL);
            (void)core_calcdata_proc_calc_output(ctx, CALCDATA_OUTPUT_IDLE_REG_ETC_PID_MAF_I, CALCDATA_OUTPUT_VARIANTED_ITEM_MAX, NULL);
            (void)core_calcdata_proc_calc_output(ctx, CALCDATA_OUTPUT_IDLE_REG_ETC_PID_MAF_D, CALCDATA_OUTPUT_VARIANTED_ITEM_MAX, NULL);
            break;
          default:
            break;
        }
      }
      if(ctx->calibration->calcdata.setup.idle.use_ign_pid_rpm) {
        (void)core_calcdata_proc_calc_output(ctx, CALCDATA_OUTPUT_IDLE_IGN_ADV_PID_RPM_P, CALCDATA_OUTPUT_VARIANTED_ITEM_MAX, NULL);
        (void)core_calcdata_proc_calc_output(ctx, CALCDATA_OUTPUT_IDLE_IGN_ADV_PID_RPM_I, CALCDATA_OUTPUT_VARIANTED_ITEM_MAX, NULL);
        (void)core_calcdata_proc_calc_output(ctx, CALCDATA_OUTPUT_IDLE_IGN_ADV_PID_RPM_D, CALCDATA_OUTPUT_VARIANTED_ITEM_MAX, NULL);
      }

      break;
    case CORE_CALCDATA_OUTPUTS_STAGE_2:
      (void)core_calcdata_proc_calc_output(ctx, CALCDATA_OUTPUT_VOLUMETRIC_EFFICIENCY, CALCDATA_OUTPUT_VARIANTED_ITEM_MAX, NULL);
      break;
    case CORE_CALCDATA_OUTPUTS_STAGE_3:
      (void)core_calcdata_proc_calc_output(ctx, CALCDATA_OUTPUT_IGNITION_ADVANCE, CALCDATA_OUTPUT_VARIANTED_ITEM_MAX, NULL);
      (void)core_calcdata_proc_calc_output(ctx, CALCDATA_OUTPUT_AIR_TO_FUEL_RATIO, CALCDATA_OUTPUT_VARIANTED_ITEM_MAX, NULL);
      (void)core_calcdata_proc_calc_output(ctx, CALCDATA_OUTPUT_INJECTION_PHASE, CALCDATA_OUTPUT_VARIANTED_ITEM_MAX, NULL);

      calcdata_outputs_idle(ctx);
      break;
    case CORE_CALCDATA_OUTPUTS_STAGE_4:
      calcdata_outputs_etc(ctx);

      calcdata_outputs_ignition(ctx);
      calcdata_outputs_injection(ctx);
      break;
    default:
      break;
  }
}
