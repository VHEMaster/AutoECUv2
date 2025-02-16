/*
 * core_timing.c
 *
 *  Created on: Jun 12, 2024
 *      Author: VHEMaster
 */

#include "core_timing_ignition.h"
#include "core_timing_injection.h"
#include "core_timing.h"
#include "config_hw.h"
#include "common.h"


volatile float DEBUG_INJ_PHASE = 280;
volatile float DEBUG_INJ_MASS = 32; //mg
volatile float DEBUG_IGN_ADVANCE = 0.0f;

ITCM_FUNC void core_timing_signal_update_cb(void *usrdata, const timing_data_t *data, const timing_diag_t *diag)
{
  ecu_core_ctx_t *ctx = (ecu_core_ctx_t *)usrdata;
  ecu_core_runtime_ctx_t *runtime = &ctx->runtime;
  timing_ctx_t *timing_ctx;
  ecu_core_runtime_cylinder_sequentialed_t *sequentialed;
  float pos_offset;
  error_t err;
  ecu_config_ignition_group_mode_t group_mode_ign;
  ecu_config_injection_group_mode_t group_mode_inj;
  bool semisequential_needed = false;
  bool sequential_needed = false;
  bool ignition_update_trigger = false;
  bool injection_update_trigger = false;
  uint32_t process_update_trigger_counter = ctx->process_update_trigger_counter;
  uint8_t process_update_trigger_counter_1of2 = process_update_trigger_counter & 1;

  runtime->global.injection.input_injection_phase = DEBUG_INJ_PHASE;
  runtime->global.injection.input_injection_mass = DEBUG_INJ_MASS;
  runtime->global.ignition.input_ignition_advance = DEBUG_IGN_ADVANCE;

  do {

    err = ecu_modules_get_timing_ctx(ctx->calibration->io.banked.global.module_timing[ECU_CONFIG_IO_TIMING_PRIMARY], &timing_ctx);
    BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

    ctx->timing_data = *data;
    ctx->timing_diag = *diag;

    if(ctx->timing_data.crankshaft.mode >= TIMING_CRANKSHAFT_MODE_VALID) {
      ecu_config_set_ignition_enabled(true);
      runtime->global.cylinders_count = ctx->calibration->cylinders.cylinders_count;

      for(ecu_config_ignition_group_t gr = 0; gr < ECU_CONFIG_IGNITION_GROUP_MAX; gr++) {
        if(!semisequential_needed || !sequential_needed) {
          if(ctx->calibration->ignition.groups[gr].enabled) {
            group_mode_ign = ctx->calibration->ignition.groups[gr].mode;
            if(!semisequential_needed) {
              if(group_mode_ign == ECU_CONFIG_IGNITION_GROUP_MODE_DISTRIBUTOR ||
                  group_mode_ign == ECU_CONFIG_IGNITION_GROUP_MODE_SEMISEQUENTIAL_ONLY ||
                  group_mode_ign == ECU_CONFIG_IGNITION_GROUP_MODE_SEQUENTIAL_AND_SEMISEQUENTIAL) {
                semisequential_needed = true;
              }
            }
            if(!sequential_needed) {
              if(group_mode_ign == ECU_CONFIG_IGNITION_GROUP_MODE_SEQUENTIAL_ONLY ||
                  group_mode_ign == ECU_CONFIG_IGNITION_GROUP_MODE_SEQUENTIAL_AND_SEMISEQUENTIAL) {
                sequential_needed = true;
              }
            }
          }
        } else {
          break;
        }
      }

      for(ecu_config_injection_group_t gr = 0; gr < ECU_CONFIG_INJECTION_GROUP_MAX; gr++) {
        if(!semisequential_needed || !sequential_needed) {
          if(ctx->calibration->injection.groups[gr].enabled) {
            group_mode_inj = ctx->calibration->injection.groups[gr].mode;
            if(!semisequential_needed) {
              if(group_mode_inj == ECU_CONFIG_INJECTION_GROUP_MODE_MONO ||
                  group_mode_inj == ECU_CONFIG_INJECTION_GROUP_MODE_SEMISEQUENTIAL_ONLY ||
                  group_mode_inj == ECU_CONFIG_INJECTION_GROUP_MODE_SEQUENTIAL_AND_SEMISEQUENTIAL) {
                semisequential_needed = true;
              }
            }
            if(!sequential_needed) {
              if(group_mode_inj == ECU_CONFIG_INJECTION_GROUP_MODE_SEQUENTIAL_ONLY ||
                  group_mode_inj == ECU_CONFIG_INJECTION_GROUP_MODE_SEQUENTIAL_AND_SEMISEQUENTIAL) {
                sequential_needed = true;
              }
            }
          }
        } else {
          break;
        }
      }

      for(ecu_cylinder_t cy = 0; cy < runtime->global.cylinders_count; cy++) {
        pos_offset = ctx->calibration->cylinders.cylinders[cy].pos_offset;

        if(sequential_needed) {
          sequentialed = &runtime->cylinders[cy].sequentialed[ECU_CORE_RUNTIME_CYLINDER_SEQUENTIAL];
          err = timing_calculate_offset_position(timing_ctx,
              pos_offset, true,
              &sequentialed->timing_req,
              &sequentialed->crankshaft_data);
          BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));
        }

        if(semisequential_needed) {
          sequentialed = &runtime->cylinders[cy].sequentialed[ECU_CORE_RUNTIME_CYLINDER_SEMISEQUENTIAL_DISTRIBUTOR];
          err = timing_calculate_offset_position(timing_ctx,
              pos_offset, false,
              &sequentialed->timing_req,
              &sequentialed->crankshaft_data);
          BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));
        }
      }

      if(ctx->calibration->ignition.process_update_trigger == ECU_CONFIG_IGNITION_PROCESS_UPDATE_TRIGGER_ALWAYS) {
        ignition_update_trigger = true;
      } else if(ctx->calibration->ignition.process_update_trigger == ECU_CONFIG_IGNITION_PROCESS_UPDATE_TRIGGER_1OF2_1ST) {
        ignition_update_trigger = !process_update_trigger_counter_1of2;
      } else if(ctx->calibration->ignition.process_update_trigger == ECU_CONFIG_IGNITION_PROCESS_UPDATE_TRIGGER_1OF2_2ND) {
        ignition_update_trigger = process_update_trigger_counter_1of2;
      }

      if(ctx->calibration->injection.process_update_trigger == ECU_CONFIG_INJECTION_PROCESS_UPDATE_TRIGGER_ALWAYS) {
        injection_update_trigger = true;
      } else if(ctx->calibration->injection.process_update_trigger == ECU_CONFIG_INJECTION_PROCESS_UPDATE_TRIGGER_1OF2_1ST) {
        injection_update_trigger = !process_update_trigger_counter_1of2;
      } else if(ctx->calibration->injection.process_update_trigger == ECU_CONFIG_INJECTION_PROCESS_UPDATE_TRIGGER_1OF2_2ND) {
        injection_update_trigger = process_update_trigger_counter_1of2;
      }

      if(ignition_update_trigger) {
        core_timing_signal_update_ignition(ctx);
      }
      if(injection_update_trigger) {
        core_timing_signal_update_injection(ctx);
      }
    } else {
      ecu_config_set_ignition_enabled(false);

      for(ecu_config_ignition_group_t gr = 0; gr < ECU_CONFIG_IGNITION_GROUP_MAX; gr++) {
        if(ctx->calibration->ignition.groups[gr].enabled) {
          for(ecu_cylinder_t cy = 0; cy < ctx->runtime.global.cylinders_count; cy++) {
            err = ecu_config_gpio_output_write(ctx->calibration->ignition.groups[gr].cylinders[cy].output_pin, 0);
            BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));
          }
        }
      }

      for(ecu_config_injection_group_t gr = 0; gr < ECU_CONFIG_INJECTION_GROUP_MAX; gr++) {
        if(ctx->calibration->injection.groups[gr].enabled) {
          for(ecu_cylinder_t cy = 0; cy < ctx->runtime.global.cylinders_count; cy++) {
            err = ecu_config_gpio_output_write(ctx->calibration->injection.groups[gr].cylinders[cy].output_pin, 0);
            BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));
          }
        }
      }

      memset(runtime, 0, sizeof(ecu_core_runtime_ctx_t));
    }
  } while(0);

  ctx->process_update_trigger_counter = process_update_trigger_counter + 1;
}
