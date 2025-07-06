/*
 * calcdata_outputs_tps.c
 *
 *  Created on: Jul 6, 2025
 *      Author: VHEMaster
 */

#include "calcdata_outputs_tps.h"
#include "config_global.h"

#include "calcdata_proc.h"


void calcdata_outputs_tps(ecu_core_ctx_t *ctx)
{
  const uint32_t banks_count = ctx->runtime.global.banks_count;

  const ecu_core_runtime_value_ctx_t *input_tps;
  const ecu_core_runtime_value_ctx_t *tps_ignition_ctrl;

  ecu_core_runtime_value_ctx_t *tps_idle_ignition_influence;

  ecu_sensor_tps_t tps_instances[ECU_BANK_MAX];
  bool tps_used = false;
  bool tps_ok = true;

  float value_tps, value_ign_ctrl;

  if(ctx->calibration->calcdata.setup.idle.ignition_ctrl_source == CALCDATA_IDLE_IGN_CTRL_SRC_TPS) {
    tps_instances[ECU_BANK_1] = ctx->calibration->io.banked.global.common.sensor_tps[ECU_CONFIG_IO_TPS_PRIMARY];
    if(tps_instances[ECU_BANK_1] >= ECU_SENSOR_TPS_MAX) {
      for(ecu_bank_t bank = 0; bank < banks_count; bank++) {
        tps_instances[bank] = ctx->calibration->io.banked.banks[bank].common.sensor_tps[ECU_CONFIG_IO_TPS_PRIMARY];
        if(tps_instances[bank] >= ECU_SENSOR_TPS_MAX) {
          tps_ok = false;
        } else {
          tps_used = true;
          if(!tps_ok) {
            //TODO: error flag
          }
        }
      }
    } else {
      tps_used = true;
      for(ecu_bank_t bank = ECU_BANK_2; bank < banks_count; bank++) {
        tps_instances[bank] = tps_instances[ECU_BANK_1];
      }
    }

    for(ecu_bank_t bank = 0; bank < banks_count; bank++) {
      tps_idle_ignition_influence = &ctx->runtime.banked.misc.banks[bank].ignition.idle_tps_influence;

      if(tps_used) {
        (void)core_calcdata_proc_get_input_ptr(ctx, bank, CALCDATA_RELATION_INPUT_SOURCE_SENSOR_BANKED_TPS, &input_tps);
        (void)core_calcdata_proc_get_output_ptr(ctx, bank, CALCDATA_OUTPUT_IDLE_TPS_IGNITION_CTRL, &tps_ignition_ctrl);

        if(input_tps->valid && tps_ignition_ctrl->valid) {
          value_tps = input_tps->value;
          value_ign_ctrl = tps_ignition_ctrl->value;
          if(value_ign_ctrl > 0.0f && value_tps < value_ign_ctrl) {
            tps_idle_ignition_influence->value = 1.0f - (value_tps / value_ign_ctrl);
          } else {
            tps_idle_ignition_influence->value = 0.0f;
          }
          tps_idle_ignition_influence->valid = true;
        } else {
          memset(tps_idle_ignition_influence, 0, sizeof(*tps_idle_ignition_influence));
        }
      } else {
        memset(tps_idle_ignition_influence, 0, sizeof(*tps_idle_ignition_influence));
      }
    }
  } else {
    for(ecu_bank_t bank = 0; bank < banks_count; bank++) {
      tps_idle_ignition_influence = &ctx->runtime.banked.misc.banks[bank].ignition.idle_tps_influence;
      memset(tps_idle_ignition_influence, 0, sizeof(*tps_idle_ignition_influence));
    }
  }
}
