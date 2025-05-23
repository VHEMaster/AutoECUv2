/*
 * core_timing_ignition.c
 *
 *  Created on: Aug 14, 2024
 *      Author: VHEMaster
 */

#include "core_timing_common.h"
#include "config_hw.h"
#include "common.h"
#include "interpolation.h"

ITCM_FUNC void core_timing_signal_update_ignition(ecu_core_ctx_t *ctx)
{
  error_t err;
  timing_crankshaft_mode_t crankshaft_mode;
  ecu_config_ignition_group_mode_t group_mode;
  timing_data_crankshaft_t *crankshaft_data;
  input_id_t power_voltage_pin;
  input_value_t input_analog_value;
  float power_voltage;
  float input_ignition_advance;

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
  float ignition_advance_gr_requested;
  float ignition_advance_gr_accept_vs_requested;
  float ignition_advance_gr_adder;
  float ignition_advance_cy;
  float ignition_advance_cy_add;
  float signal_prepare_advance_gr;
  float us_per_degree;
  float us_per_degree_pulsed;
  float us_per_degree_revolution;
  float degrees_before_prepare;
  float degrees_before_ignite_cur;
  float degrees_before_ignite_prev;

  float crankshaft_period;
  float crankshaft_signal_delta;

  time_us_t time_to_saturate;
  time_us_t time_to_ignite;

  uint32_t process_update_trigger_counter_gr;
  uint8_t process_update_trigger_counter_gr_1of3;
  bool process_update_trigger = false;

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
    crankshaft_period = ctx->timing_data.crankshaft.sensor_data.period;
    signal_prepare_advance = ctx->calibration->ignition.signal_prepare_advance;
    crankshaft_signal_delta = time_diff(ctx->timing_data.crankshaft.sensor_data.current.timestamp,
        ctx->timing_data.crankshaft.sensor_data.previous.timestamp);

    input_ignition_advance = ctx->runtime.global.ignition.input_ignition_advance;
    ctx->runtime.global.ignition.signal_prepare_advance = signal_prepare_advance;

    for(ecu_config_ignition_group_t gr = 0; gr < ECU_CONFIG_IGNITION_GROUP_MAX; gr++) {
      group_config = &ctx->calibration->ignition.groups[gr];
      runtime_gr = &ctx->runtime.global.ignition.groups[gr];
      if(group_config->enabled) {
        process_update_trigger_counter_gr = ctx->runtime.global.ignition.process_update_trigger_counter;
        process_update_trigger_counter_gr_1of3 = process_update_trigger_counter_gr % 3;

        if(group_config->process_update_trigger == ECU_CONFIG_IGNITION_GROUP_PROCESS_UPDATE_TRIGGER_ALWAYS) {
          process_update_trigger = true;
        } else if(group_config->process_update_trigger == ECU_CONFIG_IGNITION_GROUP_PROCESS_UPDATE_TRIGGER_1OF3_1ST) {
          process_update_trigger = process_update_trigger_counter_gr_1of3 == 0;
        } else if(group_config->process_update_trigger == ECU_CONFIG_IGNITION_GROUP_PROCESS_UPDATE_TRIGGER_1OF3_2ND) {
          process_update_trigger = process_update_trigger_counter_gr_1of3 == 1;
        } else if(group_config->process_update_trigger == ECU_CONFIG_IGNITION_GROUP_PROCESS_UPDATE_TRIGGER_1OF3_3RD) {
          process_update_trigger = process_update_trigger_counter_gr_1of3 == 2;
        }

        if(process_update_trigger == true)
        {
          ip_input = math_interpolate_input(power_voltage, group_config->voltage_to_saturation_time.input, group_config->voltage_to_saturation_time.items);
          saturation_time_table = math_interpolate_1d(ip_input, group_config->voltage_to_saturation_time.output);
          runtime_gr->saturation_time_table = saturation_time_table;

          ip_input = math_interpolate_input(ctx->timing_data.crankshaft.sensor_data.rpm, group_config->rpm_to_saturation_mult.input, group_config->rpm_to_saturation_mult.items);
          saturation_rpm_mult_table = math_interpolate_1d(ip_input, group_config->rpm_to_saturation_mult.output);
          runtime_gr->saturation_rpm_mult_table = saturation_rpm_mult_table;

          saturation_time = saturation_time_table * saturation_rpm_mult_table;
          runtime_gr->saturation_time = saturation_time;

          ignition_advance_gr_requested = input_ignition_advance + group_config->advance_add;
          runtime_gr->advance_requested = ignition_advance_gr_requested;

          us_per_degree_pulsed = ctx->timing_data.crankshaft.sensor_data.us_per_degree_pulsed;
          us_per_degree_revolution = ctx->timing_data.crankshaft.sensor_data.us_per_degree_revolution;

          if(runtime_gr->initialized) {
            ignition_advance_gr = runtime_gr->advance;
            ignition_advance_gr_accept_vs_requested = ignition_advance_gr_requested - ignition_advance_gr;
            if(ignition_advance_gr_accept_vs_requested > 0.0f) {
              ignition_advance_gr_adder = group_config->advance_slew_rate_earlier;
            } else if(ignition_advance_gr_accept_vs_requested < 0.0f) {
              ignition_advance_gr_adder = -group_config->advance_slew_rate_later;
            } else {
              ignition_advance_gr_adder = 0.0f;
            }
            ignition_advance_gr_adder *= crankshaft_signal_delta / crankshaft_period;

            if((ignition_advance_gr_accept_vs_requested > 0.0f && ignition_advance_gr_adder > ignition_advance_gr_accept_vs_requested) ||
                (ignition_advance_gr_accept_vs_requested < 0.0f && ignition_advance_gr_adder < ignition_advance_gr_accept_vs_requested)) {
              ignition_advance_gr_adder = ignition_advance_gr_accept_vs_requested;
            }
            ignition_advance_gr += ignition_advance_gr_adder;
          } else {
            ignition_advance_gr = ignition_advance_gr_requested;
          }

          runtime_gr->advance = ignition_advance_gr;

          runtime_gr->initialized = true;

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
          if(distributor) {
            needtoclear = true;
          }

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
                  } else if(group_config->cylinders[cy_opposite].output_pin == group_config->cylinders[cy].output_pin) {
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

                      err = core_timing_pulse_schedule(ctx, group_config->cylinders[cy].output_pin,
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
        }
        ctx->runtime.global.ignition.process_update_trigger_counter = process_update_trigger_counter_gr + 1;
      } else {
        if(runtime_gr->initialized) {
          memset(runtime_gr, 0, sizeof(*runtime_gr));
        }
      }
    }
  } while(0);
}
