/*
 * core_timing.c
 *
 *  Created on: Jun 12, 2024
 *      Author: VHEMaster
 */

#include "core_timing.h"
#include "config_hw.h"
#include "common.h"
#include "queuedpulses.h"
#include "interpolation.h"

volatile float DEBUG_IGN_ADVANCE = 0.0f;

#define CORE_IGNITION_OUTPUT_ACTIVE       1
#define CORE_IGNITION_OUTPUT_INACTIVE     0

ITCM_FUNC static error_t core_timing_ignition_schedule(ecu_core_ctx_t *ctx, ecu_gpio_output_pin_t output_pin, time_us_t pulse_start, time_us_t pulse_end)
{
  time_us_t now;
  time_us_t time_mask, time_mask_2;
  output_id_t pin;
  error_t err;
  error_t ret;
  time_delta_us_t delta;

  do {
    ret = E_OK;
    time_mask = time_mask_us();
    time_mask_2 = time_mask >> 1;

    err = ecu_config_gpio_output_get_id(output_pin, &pin);
    BREAK_IF_ACTION(ret == E_OK && err != E_OK, ret = err);

    err = E_OK;
    now = time_now_us();
    delta = time_diff(pulse_start, now);
    if(delta < time_mask_2) {
      err = queuedpulses_enqueue_ex(pin, delta, CORE_IGNITION_OUTPUT_INACTIVE);
    } else {
      err = output_set_value(pin, CORE_IGNITION_OUTPUT_ACTIVE);
    }
    if(ret == E_OK && err != E_OK) {
      (void)output_set_value(pin, CORE_IGNITION_OUTPUT_INACTIVE);
      ret = err;
      break;
    }

    err = E_OK;
    delta = time_diff(pulse_end, pulse_start);
    if(delta < time_mask_2) {
      err = queuedpulses_enqueue_ex(pin, delta, CORE_IGNITION_OUTPUT_ACTIVE);
    } else {
      err = output_set_value(pin, CORE_IGNITION_OUTPUT_INACTIVE);
    }
    if(ret == E_OK && err != E_OK) {
      (void)output_set_value(pin, CORE_IGNITION_OUTPUT_INACTIVE);
      ret = err;
      break;
    }
  } while(0);

  return ret;
}

ITCM_FUNC static void core_timing_signal_update_ignition(ecu_core_ctx_t *ctx)
{
  error_t err;
  float ignition_advance = DEBUG_IGN_ADVANCE;
  timing_crankshaft_mode_t crankshaft_mode;
  ecu_config_ignition_group_mode_t group_mode;
  timing_data_crankshaft_t *crankshaft_data;
  input_id_t power_voltage_pin;
  input_value_t input_analog_value;
  float power_voltage;

  ecu_core_runtime_cylinder_sequentialed_type_t sequentialed_mode;
  bool distributor;
  bool needtoclear;
  output_value_t output_value;
  bool output_valid;

  sMathInterpolateInput ip_input;
  const ecu_config_ignition_group_setup_t *group_config;

  float signal_prepare_advance;
  ecu_core_runtime_global_ignition_group_ctx_t *runtime_gr;
  ecu_core_runtime_group_cylinder_ignition_ctx_t *runtime_cy;
  ecu_cylinder_t cy_opposite;

  float saturation_time_table;
  float saturation_rpm_mult_table;
  float saturation_time;

  float position_cy;
  float ignition_advance_gr;
  float ignition_advance_cy;
  float ignition_advance_cy_add;
  float signal_prepare_advance_gr;
  float us_per_degree;
  float us_per_degree_pulsed;
  float us_per_degree_revolution;
  float degrees_before_prepare;
  float degrees_before_ignite_cur;
  float degrees_before_ignite_prev;

  time_us_t time_to_saturate;
  time_us_t time_to_ignite;

  do {
    err = ecu_config_gpio_input_get_id(ctx->calibration->ignition.power_voltage_pin, &power_voltage_pin);
    if(err == E_OK) {
      (void)input_get_value(power_voltage_pin, &input_analog_value, NULL);
      power_voltage = (float)input_analog_value * INPUTS_ANALOG_MULTIPLIER_R;
      ctx->runtime.global.ignition.power_voltage = power_voltage;
    } else {
      power_voltage = ctx->runtime.global.ignition.power_voltage;
    }

    crankshaft_mode = ctx->timing_data.crankshaft.mode;
    signal_prepare_advance = ctx->calibration->ignition.signal_prepare_advance;

    ctx->runtime.global.ignition.ignition_advance = ignition_advance;
    ctx->runtime.global.ignition.signal_prepare_advance = signal_prepare_advance;

    for(ecu_config_ignition_group_t gr = 0; gr < ECU_CONFIG_IGNITION_GROUP_MAX; gr++) {
      group_config = &ctx->calibration->ignition.groups[gr];
      runtime_gr = &ctx->runtime.global.ignition.groups[gr];
      if(group_config->enabled) {
        runtime_gr->initialized = true;
        ip_input = math_interpolate_input(power_voltage, group_config->voltage_to_saturation_time.input, group_config->voltage_to_saturation_time.items);
        saturation_time_table = math_interpolate_1d(ip_input, group_config->voltage_to_saturation_time.output);
        runtime_gr->saturation_time_table = saturation_time_table;

        ip_input = math_interpolate_input(ctx->timing_data.crankshaft.sensor_data.rpm, group_config->rpm_to_saturation_mult.input, group_config->rpm_to_saturation_mult.items);
        saturation_rpm_mult_table = math_interpolate_1d(ip_input, group_config->rpm_to_saturation_mult.output);
        runtime_gr->saturation_rpm_mult_table = saturation_rpm_mult_table;

        saturation_time = saturation_time_table * saturation_rpm_mult_table;
        runtime_gr->saturation_time = saturation_time;

        ignition_advance_gr = ignition_advance + group_config->advance_add;

        us_per_degree_pulsed = ctx->timing_data.crankshaft.sensor_data.us_per_degree_pulsed;
        us_per_degree_revolution = ctx->timing_data.crankshaft.sensor_data.us_per_degree_revolution;

        switch(ctx->calibration->ignition.uspd_source) {
          default:
          case ECU_CONFIG_IGNITION_USPD_SOURCE_PER_SENSOR_PULSE:
            us_per_degree = us_per_degree_pulsed;
            break;
          case ECU_CONFIG_IGNITION_USPD_SOURCE_PER_REVOLUTION:
            us_per_degree = us_per_degree_revolution;
            break;
        }

        signal_prepare_advance_gr = signal_prepare_advance;
        signal_prepare_advance_gr += saturation_time / us_per_degree;

        needtoclear = false;
        distributor = false;
        sequentialed_mode = ECU_CORE_RUNTIME_CYLINDER_SEQUENTIALED_NONE;

        group_mode = group_config->mode;
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

        if(runtime_gr->sequentialed_mode != sequentialed_mode ||
            runtime_gr->distributor != distributor) {
          runtime_gr->sequentialed_mode = sequentialed_mode;
          runtime_gr->distributor = distributor;
          needtoclear = true;
        }

        //TODO: IMPLEMENT THE DISTRIBUTOR

        if(!needtoclear) {
          for(ecu_cylinder_t cy = 0; cy < ctx->runtime.global.cylinders_count; cy++) {
            runtime_cy = &runtime_gr->cylinders[cy];
            if(!group_config->cylinders[cy].disabled) {
              ignition_advance_cy_add = group_config->cylinders[cy].advance_add;

              cy_opposite = ctx->calibration->cylinders.cylinders[cy].cy_opposed;
              if(cy_opposite < ECU_CYLINDER_MAX && cy != cy_opposite &&
                  sequentialed_mode == ECU_CORE_RUNTIME_CYLINDER_SEMISEQUENTIAL_DISTRIBUTOR) {
                if(cy_opposite > cy && !group_config->cylinders[cy_opposite].disabled) {
                  ignition_advance_cy_add = MIN(ignition_advance_cy_add, group_config->cylinders[cy_opposite].advance_add);
                } else {
                  if(runtime_cy->initialized) {
                    memset(runtime_cy, 0, sizeof(*runtime_cy));
                  }
                  continue;
                }
              }

              ignition_advance_cy = ignition_advance_gr + ignition_advance_cy_add;
              crankshaft_data = &ctx->runtime.cylinders[cy].sequentialed[sequentialed_mode].crankshaft_data;
              position_cy = crankshaft_data->sensor_data.current_position;
              output_valid = false;

              runtime_cy->position = position_cy;
              runtime_cy->advance = ignition_advance_cy;
              if(sequentialed_mode == ECU_CORE_RUNTIME_CYLINDER_SEQUENTIAL) {
                runtime_cy->degrees_per_cycle = 720.0f;
              } else {
                runtime_cy->degrees_per_cycle = 360.0f;
              }

              if(position_cy < -ignition_advance_cy) {
                degrees_before_ignite_cur = -position_cy - ignition_advance_cy;
              } else {
                degrees_before_ignite_cur = runtime_cy->degrees_per_cycle - position_cy - ignition_advance_cy;
              }
              degrees_before_ignite_prev = runtime_cy->degrees_before_ignite;

              if(runtime_cy->initialized) {
                if(degrees_before_ignite_cur - degrees_before_ignite_prev > 0.0f &&
                    degrees_before_ignite_cur - degrees_before_ignite_prev < 180.0f) {
                  degrees_before_ignite_cur = degrees_before_ignite_prev;
                }

                if(!runtime_cy->scheduled && !runtime_cy->ignited) {
                  degrees_before_prepare = degrees_before_ignite_cur - signal_prepare_advance_gr;
                  if(degrees_before_prepare < 0.0f) {
                    time_to_saturate = crankshaft_data->sensor_data.current.timestamp +
                        (signal_prepare_advance + degrees_before_prepare) * us_per_degree_pulsed;
                    time_to_ignite = time_to_saturate + saturation_time;

                    err = core_timing_ignition_schedule(ctx, group_config->cylinders[cy].output_pin,
                        time_to_saturate, time_to_ignite);
                    BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

                    runtime_cy->scheduled = true;
                  }
                } else {
                  if(degrees_before_ignite_prev - degrees_before_ignite_cur < -90.0f) {
                    if(runtime_cy->scheduled) {
                      runtime_cy->ignited = true;
                      runtime_cy->scheduled = false;
                    }
                  } else {
                    runtime_cy->ignited = false;
                  }
                }

              } else {
                runtime_cy->initialized = true;
                runtime_cy->scheduled = false;
                runtime_cy->ignited = true;
              }

              runtime_cy->degrees_before_ignite = degrees_before_ignite_cur;

              if(output_valid) {
                err = ecu_config_gpio_output_write(group_config->cylinders[cy].output_pin, output_value);
                BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));
              }
            } else {
              if(runtime_cy->initialized) {
                memset(runtime_cy, 0, sizeof(*runtime_cy));
              }
            }
          }
        } else {
          for(ecu_cylinder_t cy = 0; cy < ctx->runtime.global.cylinders_count; cy++) {
            runtime_cy = &runtime_gr->cylinders[cy];
            memset(runtime_cy, 0, sizeof(*runtime_cy));
          }
        }
      } else {
        if(runtime_gr->initialized) {
          memset(runtime_gr, 0, sizeof(*runtime_gr));
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
