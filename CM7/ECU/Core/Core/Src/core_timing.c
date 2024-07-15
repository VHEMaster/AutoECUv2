/*
 * core_timing.c
 *
 *  Created on: Jun 12, 2024
 *      Author: VHEMaster
 */

#include "core_timing.h"
#include "config_hw.h"
#include "common.h"

volatile float DEBUG_IGN_ADVANCE = 0.0f;

ITCM_FUNC static void core_timing_signal_update_ignition(ecu_core_ctx_t *ctx)
{
  error_t err;
  float ignition_advance = DEBUG_IGN_ADVANCE;
  float saturation_advance = 10;
  timing_crankshaft_mode_t crankshaft_mode;
  ecu_config_ignition_group_mode_t group_mode;
  timing_data_crankshaft_t *crankshaft_data;

  ecu_core_runtime_cylinder_sequentialed_type_t sequentialed_mode;
  bool distributor = false;
  bool needtoclear = false;
  output_value_t output_value;

  do {
    crankshaft_mode = ctx->timing_data.crankshaft.mode;

    for(ecu_config_ignition_group_t gr = 0; gr < ECU_CONFIG_IGNITION_GROUP_MAX; gr++) {
      if(ctx->calibration->ignition.groups[gr].enabled) {
        group_mode = ctx->calibration->ignition.groups[gr].mode;
        if(group_mode == ECU_CONFIG_IGNITION_GROUP_MODE_DISTRIBUTOR) {
          distributor = true;
          sequentialed_mode = ECU_CORE_RUNTIME_CYLINDER_SEMISEQUENTIAL_DISTRIBUTOR;
        } else if(crankshaft_mode >= TIMING_CRANKSHAFT_MODE_VALID &&
            group_mode == ECU_CONFIG_IGNITION_GROUP_MODE_SEMISEQUENTIAL_ONLY) {
          sequentialed_mode = ECU_CORE_RUNTIME_CYLINDER_SEMISEQUENTIAL_DISTRIBUTOR;
        } else if(crankshaft_mode == TIMING_CRANKSHAFT_MODE_VALID_PHASED &&
            group_mode == ECU_CONFIG_IGNITION_GROUP_MODE_SEQUENTIAL_ONLY) {
          sequentialed_mode = ECU_CORE_RUNTIME_CYLINDER_SEQUENTIAL;
        } else if(group_mode == ECU_CONFIG_IGNITION_GROUP_MODE_SEQUENTIAL_AND_SEMISEQUENTIAL) {
          if(crankshaft_mode == TIMING_CRANKSHAFT_MODE_VALID_PHASED) {
            sequentialed_mode = ECU_CORE_RUNTIME_CYLINDER_SEQUENTIAL;
          } else {
            sequentialed_mode = ECU_CORE_RUNTIME_CYLINDER_SEMISEQUENTIAL_DISTRIBUTOR;
          }
        } else {
          needtoclear = true;
        }

        if(!needtoclear) {
          for(ecu_cylinder_t cy = 0; cy < ctx->runtime.global.cylinders_count; cy++) {
            crankshaft_data = &ctx->runtime.cylinders[cy].sequentialed[sequentialed_mode].crankshaft_data;
            if(!distributor) {
              if(crankshaft_data->pos_phased >= ignition_advance - saturation_advance &&
                  crankshaft_data->pos_phased < ignition_advance) {
                output_value = 1;
              } else {
                output_value = 0;
              }
            } else {
              // TODO: implement
              output_value = 0;
            }
            err = ecu_config_gpio_output_write(ctx->calibration->ignition.groups[gr].cylinders[cy].output_pin, output_value);
            BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));
          }
        } else {
          for(ecu_cylinder_t cy = 0; cy < ctx->runtime.global.cylinders_count; cy++) {
            memset(&ctx->runtime.cylinders[cy].ignition, 0, sizeof(ecu_core_runtime_global_ignition_ctx_t));
          }
        }
      }
    }
  } while(0);
}

ITCM_FUNC static void core_timing_signal_update_injection(ecu_core_ctx_t *ctx)
{

}

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

      core_timing_signal_update_ignition(ctx);
      core_timing_signal_update_injection(ctx);
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

}
