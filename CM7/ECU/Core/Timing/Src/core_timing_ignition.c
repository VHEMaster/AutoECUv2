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

OPTIMIZE_FAST
ITCM_FUNC void core_timing_signal_update_ignition(ecu_core_ctx_t *ctx)
{
  error_t err;
  timing_crankshaft_mode_t crankshaft_mode;
  ecu_config_ignition_group_mode_t group_mode;
  timing_data_crankshaft_t *crankshaft_data;
  input_id_t power_voltage_pin;
  input_value_t input_analog_value;
  float power_voltage;
  uint32_t banks_count;
  uint32_t cylinders_count;

  ecu_core_runtime_cylinder_sequentialed_type_t sequentialed_mode;
  bool distributor;
  bool needtoclear;
  output_value_t output_value;
  bool output_valid;
  ecu_bank_t bank_cy;

  sMathInterpolateInput ip_input;
  const ecu_config_ignition_t *config;
  const ecu_config_ignition_group_setup_t *group_config;
  const timing_data_crankshaft_t *crankshaft;

  bool input_valid;
  bool input_allowed_b[ECU_BANK_MAX];
  bool use_ignition_acceptance;

  float signal_prepare_advance;
  ecu_core_runtime_global_ignition_ctx_t *runtime;
  ecu_core_runtime_global_ignition_group_ctx_t *runtime_gr;
  ecu_core_runtime_group_cylinder_ignition_ctx_t *runtime_cy;
  ecu_core_runtime_cylinder_ignition_acceptance_ctx_t *ignition_acceptance_cy;
  ecu_core_runtime_group_cylinder_ignition_acceptance_ctx_t *ignition_acceptance_gr_cy;
  ecu_cylinder_t cy_opposite;

  float saturation_time_table;
  float saturation_rpm_mult_table;
  float saturation_time;

  float position_cy;
  float ignition_advance_gr_requested_mean;
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

  float ignition_advance_gr_cy[ECU_CYLINDER_MAX];
  float ignition_advance_gr_requested_cy[ECU_CYLINDER_MAX];

  time_us_t time_to_saturate;
  time_us_t time_to_ignite;

  uint32_t process_update_trigger_counter_gr;
  uint8_t process_update_trigger_counter_gr_1of3;
  bool process_update_trigger = false;
  bool slew_adder_valid;
  bool runup_flag;

  do {
    banks_count = ctx->runtime.global.banks_count;
    cylinders_count = ctx->runtime.global.cylinders_count;
    config = &ctx->calibration->ignition;
    runtime = &ctx->runtime.global.ignition;
    crankshaft = &ctx->timing_data.crankshaft;
    runup_flag = ctx->runtime.global.misc.runup_flag;

    err = ecu_config_gpio_input_get_id(config->power_voltage_pin, &power_voltage_pin);
    if(err == E_OK) {
      (void)input_get_value(power_voltage_pin, &input_analog_value, NULL);
      power_voltage = (float)input_analog_value * INPUTS_ANALOG_MULTIPLIER_R;
      runtime->power_voltage = power_voltage;
    } else {
      power_voltage = runtime->power_voltage;
    }

    crankshaft_mode = crankshaft->mode;
    crankshaft_period = crankshaft->sensor_data.period;
    signal_prepare_advance = config->signal_prepare_advance;
    crankshaft_signal_delta = time_diff(crankshaft->sensor_data.current.timestamp,
        crankshaft->sensor_data.previous.timestamp);

    input_valid = runtime->input_valid;
    for(ecu_bank_t b = 0; b < banks_count; b++) {
      input_allowed_b[b]= runtime->input_banked[b].allowed;
    }
    use_ignition_acceptance = config->use_ignition_acceptance;
    runtime->signal_prepare_advance = signal_prepare_advance;

    for(ecu_cylinder_t cy = 0; cy < cylinders_count; cy++) {
      bank_cy = ctx->engine_config->calibration.cylinders.cylinders[cy].bank;
      ignition_acceptance_cy = &ctx->runtime.cylinders[cy].ignition_acceptance;
      if(use_ignition_acceptance) {
        if(ignition_acceptance_cy->valid) {
          for(ecu_config_ignition_group_t gr = 0; gr < ECU_CONFIG_IGNITION_GROUP_MAX; gr++) {
            runtime_gr = &runtime->groups[gr];
            runtime_gr->advance_input_cy[cy] = ignition_acceptance_cy->ignition_advance;
            ignition_acceptance_gr_cy = &runtime_gr->cylinders[cy].ignition_acceptance;
            ignition_acceptance_gr_cy->ignition_advance = ignition_acceptance_cy->ignition_advance;
            ignition_acceptance_gr_cy->valid = true;
          }
          ignition_acceptance_cy->valid = false;
        } else {
          for(ecu_config_ignition_group_t gr = 0; gr < ECU_CONFIG_IGNITION_GROUP_MAX; gr++) {
            runtime_gr = &runtime->groups[gr];
            ignition_acceptance_gr_cy = &runtime_gr->cylinders[cy].ignition_acceptance;
            if(ignition_acceptance_gr_cy->valid) {
              runtime_gr->advance_input_cy[cy] = ignition_acceptance_gr_cy->ignition_advance;
            } else {
              runtime_gr->advance_input_cy[cy] = runtime->input_banked[bank_cy].ignition_advance;
            }
          }
        }
      } else {
        for(ecu_config_ignition_group_t gr = 0; gr < ECU_CONFIG_IGNITION_GROUP_MAX; gr++) {
          runtime_gr = &runtime->groups[gr];
          runtime_gr->advance_input_cy[cy] = runtime->input_banked[bank_cy].ignition_advance;
        }
      }
    }

    for(ecu_config_ignition_group_t gr = 0; gr < ECU_CONFIG_IGNITION_GROUP_MAX; gr++) {
      group_config = &config->groups[gr];
      runtime_gr = &runtime->groups[gr];
      if(group_config->enabled && input_valid) {
        process_update_trigger_counter_gr = runtime->process_update_trigger_counter;
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

          ip_input = math_interpolate_input(crankshaft->sensor_data.rpm, group_config->rpm_to_saturation_mult.input, group_config->rpm_to_saturation_mult.items);
          saturation_rpm_mult_table = math_interpolate_1d(ip_input, group_config->rpm_to_saturation_mult.output);
          runtime_gr->saturation_rpm_mult_table = saturation_rpm_mult_table;

          saturation_time = saturation_time_table * saturation_rpm_mult_table;
          runtime_gr->saturation_time = saturation_time;

          ignition_advance_gr_requested_mean = 0;
          for(ecu_cylinder_t cy = 0; cy < cylinders_count; cy++) {
            ignition_advance_gr_requested_cy[cy] = runtime_gr->advance_input_cy[cy] + group_config->advance_add;
            ignition_advance_gr_requested_mean += ignition_advance_gr_requested_cy[cy];
            runtime_gr->advance_requested_cy[cy] = ignition_advance_gr_requested_cy[cy];
          }
          ignition_advance_gr_requested_mean /= cylinders_count;
          runtime_gr->advance_requested_mean = ignition_advance_gr_requested_mean;

          us_per_degree_pulsed = crankshaft->sensor_data.us_per_degree_pulsed;
          us_per_degree_revolution = crankshaft->sensor_data.us_per_degree_revolution;

          for(ecu_cylinder_t cy = 0; cy < cylinders_count; cy++) {
            slew_adder_valid = false;
            if(runtime_gr->initialized && crankshaft_mode >= TIMING_CRANKSHAFT_MODE_VALID && runup_flag) {
              ignition_advance_gr_cy[cy] = runtime_gr->advance_cy[cy];
              ignition_advance_gr_accept_vs_requested = ignition_advance_gr_requested_cy[cy] - ignition_advance_gr_cy[cy];
              if(ignition_advance_gr_accept_vs_requested > 0.0f) {
                ignition_advance_gr_adder = group_config->advance_slew_rate_earlier;
              } else if(ignition_advance_gr_accept_vs_requested < 0.0f) {
                ignition_advance_gr_adder = -group_config->advance_slew_rate_later;
              } else {
                ignition_advance_gr_adder = 0.0f;
              }
              ignition_advance_gr_adder *= crankshaft_signal_delta / crankshaft_period;

              if(ignition_advance_gr_adder) {
                if((ignition_advance_gr_accept_vs_requested > 0.0f && ignition_advance_gr_adder > ignition_advance_gr_accept_vs_requested) ||
                    (ignition_advance_gr_accept_vs_requested < 0.0f && ignition_advance_gr_adder < ignition_advance_gr_accept_vs_requested)) {
                  ignition_advance_gr_adder = ignition_advance_gr_accept_vs_requested;
                }
              }
              slew_adder_valid = true;
            }

            if(slew_adder_valid) {
              ignition_advance_gr_cy[cy] += ignition_advance_gr_adder;
            } else {
              ignition_advance_gr_cy[cy] = ignition_advance_gr_requested_cy[cy];
            }

            runtime_gr->advance_cy[cy] = ignition_advance_gr_cy[cy];
          }

          runtime_gr->initialized = true;

          switch(config->uspd_source) {
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
          if(crankshaft_mode >= TIMING_CRANKSHAFT_MODE_VALID) {
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
            for(ecu_cylinder_t cy = 0; cy < cylinders_count; cy++) {
              bank_cy = ctx->engine_config->calibration.cylinders.cylinders[cy].bank;
              runtime_cy = &runtime_gr->cylinders[cy];
              ignition_acceptance_gr_cy = &runtime_cy->ignition_acceptance;
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

                ignition_advance_cy = ignition_advance_gr_cy[cy];
                ignition_advance_cy += ignition_advance_cy_add;
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

                      if(input_allowed_b[bank_cy]) {
                        err = core_timing_pulse_schedule(ctx, group_config->cylinders[cy].output_pin,
                            time_to_saturate, time_to_ignite);
                        BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));
                      }
                      ignition_acceptance_gr_cy->valid = false;
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
                  ignition_acceptance_gr_cy->valid = false;
                }
              } else {
                if(runtime_cy->initialized) {
                  memset(runtime_cy, 0, sizeof(*runtime_cy));
                }
              }
            }
          } else {
            for(ecu_cylinder_t cy = 0; cy < cylinders_count; cy++) {
              runtime_cy = &runtime_gr->cylinders[cy];
              memset(runtime_cy, 0, sizeof(*runtime_cy));
            }
          }
        }
        runtime->process_update_trigger_counter = process_update_trigger_counter_gr + 1;
      } else {
        if(runtime_gr->initialized) {
          memset(runtime_gr, 0, sizeof(*runtime_gr));
        }
      }
    }
  } while(0);
}
