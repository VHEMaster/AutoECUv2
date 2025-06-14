/*
 * core_timing_injection.c
 *
 *  Created on: Aug 14, 2024
 *      Author: VHEMaster
 */

#include <math_fast.h>
#include "core_timing_common.h"
#include "config_hw.h"
#include "common.h"
#include "interpolation.h"
#include "config_map.h"

ITCM_FUNC void core_timing_signal_update_injection(ecu_core_ctx_t *ctx)
{
  error_t err;

  timing_crankshaft_mode_t crankshaft_mode;
  ecu_config_injection_group_mode_t group_mode;
  timing_data_crankshaft_t *crankshaft_data;
  input_id_t power_voltage_pin;
  input_value_t input_analog_value;
  float power_voltage;

  ecu_core_runtime_cylinder_sequentialed_type_t sequentialed_mode;
  bool needtoclear;
  output_value_t output_value;
  bool output_valid;
  uint32_t banks_count;
  uint32_t cylinders_count;

  sMathInterpolateInput ip_input;
  const ecu_config_injection_t *config;
  const ecu_config_injection_group_setup_t *group_config;
  const ecu_config_injection_group_cylinder_setup_t *cy_config;
  const ecu_config_io_banked_t *banked_config;
  const timing_data_crankshaft_t *crankshaft;

  float signal_prepare_advance;
  ecu_core_runtime_global_injection_ctx_t *runtime;
  ecu_core_runtime_global_injection_group_ctx_t *runtime_gr;
  ecu_core_runtime_group_cylinder_injection_ctx_t *runtime_cy;
  ecu_cylinder_t cy_opposite;
  ecu_config_io_map_t map_type;
  ecu_sensor_map_t map_instances[ECU_BANK_MAX];

  bool input_valid;
  float input_injection_phase;
  float input_injection_mass_b[ECU_BANK_MAX];

  float lag_time_gr;
  float lag_time_cy;
  float pulse_time_cy;
  float degrees_per_cycle;
  float enrichment_late_phase_gr;
  float injection_time_gr_mean;


  float position_cy;
  float injection_phase_gr;
  float injection_phase_gr_requested;
  float injection_phase_gr_accept_vs_requested;
  float injection_phase_gr_adder;
  float injection_phase_gr_add_rpm;
  float injection_phase_cy;
  float injection_phase_cy_add;

  float injection_time_cy;

  float signal_prepare_advance_cy;
  float us_per_degree;
  float us_per_degree_pulsed;
  float us_per_degree_revolution;
  float degrees_before_prepare;
  float degrees_before_inject_cur;
  float degrees_before_inject_prev;

  float crankshaft_rpm;
  float crankshaft_period;
  float crankshaft_signal_delta;
  float phase_slew_rate;

  float dutycycle_cy;
  float dutycycle_gr_max;
  float dutycycle_gr_mean;
  uint32_t dutycycle_gr_count;
  float dutycycle_period;

  time_us_t time_to_activate;
  time_us_t time_to_inject;

  map_data_t map_data_b[ECU_BANK_MAX];
  float ramp_pressure_gr_b[ECU_BANK_MAX];
  float performance_mult_gr_b[ECU_BANK_MAX];
  float performance_initial_us_gr_b[ECU_BANK_MAX];
  float injection_flow_us_gr_b[ECU_BANK_MAX];
  float injection_mass_us_gr_b[ECU_BANK_MAX];
  float injection_mass_gr_b[ECU_BANK_MAX];
  float injection_time_gr_b[ECU_BANK_MAX];

  uint32_t process_update_trigger_counter_gr;
  uint8_t process_update_trigger_counter_gr_1of2;
  bool process_update_trigger = false;

  do {
    banks_count = ctx->runtime.global.banks_count;
    cylinders_count = ctx->runtime.global.cylinders_count;
    banked_config = &ctx->calibration->io.banked;
    config = &ctx->calibration->injection;
    runtime = &ctx->runtime.global.injection;
    crankshaft = &ctx->timing_data.crankshaft;

    err = ecu_config_gpio_input_get_id(config->power_voltage_pin, &power_voltage_pin);
    if(err == E_OK) {
      (void)input_get_value(power_voltage_pin, &input_analog_value, NULL);
      power_voltage = (float)input_analog_value * INPUTS_ANALOG_MULTIPLIER_R;
      runtime->power_voltage = power_voltage;
    } else {
      power_voltage = runtime->power_voltage;
    }

    crankshaft_mode = crankshaft->mode;
    crankshaft_rpm = crankshaft->sensor_data.rpm;
    crankshaft_period = crankshaft->sensor_data.period;
    signal_prepare_advance = config->signal_prepare_advance;
    crankshaft_signal_delta = time_diff(crankshaft->sensor_data.current.timestamp,
        crankshaft->sensor_data.previous.timestamp);

    input_valid = runtime->input.valid;
    input_injection_phase = runtime->input.injection_phase;

    for(ecu_bank_t b = 0; b < banks_count; b++) {
      input_injection_mass_b[b] = runtime->input.injection_mass_banked[b];
    }
    runtime->signal_prepare_advance = signal_prepare_advance;

    for(ecu_config_injection_group_t gr = 0; gr < ECU_CONFIG_INJECTION_GROUP_MAX; gr++) {
      group_config = &config->groups[gr];
      runtime_gr = &runtime->groups[gr];
      if(group_config->enabled && input_valid) {
        process_update_trigger_counter_gr = runtime->process_update_trigger_counter;
        process_update_trigger_counter_gr_1of2 = process_update_trigger_counter_gr & 1;

        if(group_config->process_update_trigger == ECU_CONFIG_INJECTION_GROUP_PROCESS_UPDATE_TRIGGER_ALWAYS) {
          process_update_trigger = true;
        } else if(group_config->process_update_trigger == ECU_CONFIG_INJECTION_GROUP_PROCESS_UPDATE_TRIGGER_1OF2_1ST) {
          process_update_trigger = process_update_trigger_counter_gr_1of2 == 0;
        } else if(group_config->process_update_trigger == ECU_CONFIG_INJECTION_GROUP_PROCESS_UPDATE_TRIGGER_1OF2_2ND) {
          process_update_trigger = process_update_trigger_counter_gr_1of2 == 1;
        }

        if(process_update_trigger == true)
        {
          ip_input = math_interpolate_input(power_voltage, group_config->voltage_to_performance_dynamic.input, group_config->voltage_to_performance_dynamic.items);
          lag_time_gr = math_interpolate_1d(ip_input, group_config->voltage_to_performance_dynamic.output);
          runtime_gr->lag_time = lag_time_gr;

          if(group_config->rpm_to_phase_add.items > 0) {
            ip_input = math_interpolate_input(crankshaft_rpm, group_config->rpm_to_phase_add.input, group_config->rpm_to_phase_add.items);
            injection_phase_gr_add_rpm = math_interpolate_1d(ip_input, group_config->rpm_to_phase_add.output);
          } else {
            injection_phase_gr_add_rpm = 0;
          }

          // TODO: IMPLEMENT THAT FEATURE
          if(group_config->enrichment_late_phase_mode > ECU_CONFIG_INJECTION_GROUP_LATE_PHASE_MODE_DISABLED) {
            ip_input = math_interpolate_input(crankshaft_rpm, group_config->enrichment_late_phase.input, group_config->enrichment_late_phase.items);
            enrichment_late_phase_gr = math_interpolate_1d(ip_input, group_config->rpm_to_phase_add.output);
          } else {
            enrichment_late_phase_gr = 0;
          }

          injection_phase_gr_requested = input_injection_phase + group_config->phase_add + injection_phase_gr_add_rpm;
          runtime_gr->phase_requested = injection_phase_gr_requested;
          runtime_gr->enrichment_late_phase = enrichment_late_phase_gr;

          us_per_degree_pulsed = crankshaft->sensor_data.us_per_degree_pulsed;
          us_per_degree_revolution = crankshaft->sensor_data.us_per_degree_revolution;

          if(runtime_gr->initialized && crankshaft_mode >= TIMING_CRANKSHAFT_MODE_VALID) {
            injection_phase_gr = runtime_gr->phase;
            injection_phase_gr_accept_vs_requested = injection_phase_gr_requested - injection_phase_gr;
            phase_slew_rate = group_config->phase_slew_rate;
            if(injection_phase_gr_accept_vs_requested > 0.0f) {
              injection_phase_gr_adder = phase_slew_rate;
            } else if(injection_phase_gr_accept_vs_requested < 0.0f) {
              injection_phase_gr_adder = -phase_slew_rate;
            } else {
              injection_phase_gr_adder = 0.0f;
            }
            injection_phase_gr_adder *= crankshaft_signal_delta / crankshaft_period;

            if((injection_phase_gr_accept_vs_requested > 0.0f && injection_phase_gr_adder > injection_phase_gr_accept_vs_requested) ||
                (injection_phase_gr_accept_vs_requested < 0.0f && injection_phase_gr_adder < injection_phase_gr_accept_vs_requested)) {
              injection_phase_gr_adder = injection_phase_gr_accept_vs_requested;
            }
            injection_phase_gr += injection_phase_gr_adder;
          } else {
            injection_phase_gr = injection_phase_gr_requested;
          }

          for(ecu_bank_t b = 0; b < banks_count; b++) {
            performance_mult_gr_b[b] = 1.0f;
            ramp_pressure_gr_b[b] = group_config->performance_fuel_pressure;
          }
          map_type = group_config->performance_fuel_pressure_map_type;
          map_instances[ECU_BANK_1] = banked_config->global.common.sensor_map[map_type];
          if(map_instances[ECU_BANK_1] >= ECU_SENSOR_MAP_MAX) {
            for(ecu_bank_t b = 0; b < banks_count; b++) {
              map_instances[b] = banked_config->banks[b].common.sensor_map[map_type];
            }
          } else {
            for(ecu_bank_t b = ECU_BANK_2; b < banks_count; b++) {
              map_instances[b] = map_instances[ECU_BANK_1];
            }
          }
          if(group_config->performance_fuel_pressure_source == ECU_CONFIG_INJECTION_GROUP_PERF_PRESSURE_SOURCE_RELATIVE_MAP) {
            for(ecu_bank_t b = 0; b < banks_count; b++) {
              if(map_instances[b] < ECU_SENSOR_MAP_MAX) {
                err = ecu_sensors_map_get_value(map_instances[b], &map_data_b[b]);
                if(err == E_OK) {
                  if(map_data_b[b].valid) {
                    ramp_pressure_gr_b[b] = group_config->performance_fuel_pressure + map_data_b[b].manifold_air_pressure - 0.986923f;
                    performance_mult_gr_b[b] = fast_sqrt(ramp_pressure_gr_b[b] / group_config->performance_fuel_pressure);
                  }
                } else {
                  // TODO: error flag
                }
              } else {
                // TODO: error flag
              }
            }
          }
          injection_time_gr_mean = 0;
          for(ecu_bank_t b = 0; b < banks_count; b++) {
            performance_initial_us_gr_b[b] = group_config->performance_static;
            performance_initial_us_gr_b[b] *= 1.6666667e-5f;
            performance_initial_us_gr_b[b] *= performance_mult_gr_b[b];

            if(group_config->performance_static_units == ECU_CONFIG_INJECTION_GROUP_PERF_UNITS_CC) {
              injection_flow_us_gr_b[b] = performance_initial_us_gr_b[b];
              injection_mass_us_gr_b[b] = injection_flow_us_gr_b[b] * group_config->performance_fuel_mass_per_cc * 0.001f;
            } else {
              injection_mass_us_gr_b[b] = performance_initial_us_gr_b[b];
              injection_flow_us_gr_b[b] = injection_mass_us_gr_b[b] / group_config->performance_fuel_mass_per_cc * 0.001f;
            }

            injection_mass_gr_b[b] = input_injection_mass_b[b];

            if(injection_mass_gr_b[b] >= group_config->inject_mass_low_threshold) {
              injection_mass_gr_b[b] -= group_config->inject_mass_reduction;
              injection_time_gr_b[b] = injection_mass_gr_b[b] / injection_mass_us_gr_b[b];
            } else {
              injection_time_gr_b[b] = 0.0f;
            }

            if(injection_time_gr_b[b] < 0.0f) {
              injection_time_gr_b[b] = 0.0f;
            }
            injection_time_gr_mean += injection_time_gr_b[b];
          }
          injection_time_gr_mean /= banks_count;

          runtime_gr->time_inject_mean = injection_time_gr_mean;
          runtime_gr->phase = injection_phase_gr;

          runtime_gr->initialized = true;

          switch(config->uspd_source) {
            default:
            case ECU_CONFIG_INJECTION_USPD_SOURCE_PER_SENSOR_PULSE:
              us_per_degree = us_per_degree_pulsed;
              break;
            case ECU_CONFIG_INJECTION_USPD_SOURCE_PER_REVOLUTION:
              us_per_degree = us_per_degree_revolution;
              break;
          }

          needtoclear = false;
          sequentialed_mode = ECU_CORE_RUNTIME_CYLINDER_SEQUENTIALED_NONE;

          group_mode = group_config->mode;
          if(crankshaft_mode >= TIMING_CRANKSHAFT_MODE_VALID) {
            if(crankshaft_mode >= TIMING_CRANKSHAFT_MODE_VALID &&
                group_mode == ECU_CONFIG_INJECTION_GROUP_MODE_SEMISEQUENTIAL_ONLY) {
              sequentialed_mode = ECU_CORE_RUNTIME_CYLINDER_SEMISEQUENTIAL_DISTRIBUTOR;
            } else if(crankshaft_mode == TIMING_CRANKSHAFT_MODE_VALID_PHASED &&
                group_mode == ECU_CONFIG_INJECTION_GROUP_MODE_SEQUENTIAL_ONLY) {
              sequentialed_mode = ECU_CORE_RUNTIME_CYLINDER_SEQUENTIAL;
            } else if(group_mode == ECU_CONFIG_INJECTION_GROUP_MODE_SEQUENTIAL_AND_SEMISEQUENTIAL) {
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

          if(runtime_gr->sequentialed_mode != sequentialed_mode) {
            runtime_gr->sequentialed_mode = sequentialed_mode;
            needtoclear = true;
          }

          dutycycle_gr_max = 0;
          dutycycle_gr_mean = 0;
          dutycycle_gr_count = 0;

          if(!needtoclear) {

            for(ecu_cylinder_t cy = 0; cy < cylinders_count; cy++) {
              cy_config = &group_config->cylinders[cy];
              runtime_cy = &runtime_gr->cylinders[cy];
              if(!cy_config->disabled) {

                injection_phase_cy_add = cy_config->phase_add;

                cy_opposite = ctx->calibration->cylinders.cylinders[cy].cy_opposed;
                if(cy_opposite < ECU_CYLINDER_MAX && cy != cy_opposite &&
                    sequentialed_mode == ECU_CORE_RUNTIME_CYLINDER_SEMISEQUENTIAL_DISTRIBUTOR) {
                  if(cy_opposite > cy && !group_config->cylinders[cy_opposite].disabled) {
                    injection_phase_cy_add = MIN(injection_phase_cy_add, group_config->cylinders[cy_opposite].phase_add);
                  } else if(group_config->cylinders[cy_opposite].output_pin == group_config->cylinders[cy].output_pin) {
                    if(runtime_cy->initialized) {
                      memset(runtime_cy, 0, sizeof(*runtime_cy));
                    }
                    continue;
                  }
                }

                injection_phase_cy = injection_phase_gr + injection_phase_cy_add;
                crankshaft_data = &ctx->runtime.cylinders[cy].sequentialed[sequentialed_mode].crankshaft_data;
                position_cy = crankshaft_data->sensor_data.current_position;
                output_valid = false;

                lag_time_cy = lag_time_gr;
                lag_time_cy *= cy_config->performance_dynamic_mul;
                lag_time_cy += cy_config->performance_dynamic_add;

                injection_time_cy = injection_time_gr_b[ctx->calibration->cylinders.cylinders[cy].bank];
                injection_time_cy *= cy_config->performance_static_mul;
                dutycycle_period = crankshaft->sensor_data.period;

                if(injection_time_cy > 0.0f) {
                  if(sequentialed_mode == ECU_CORE_RUNTIME_CYLINDER_SEQUENTIAL) {
                    injection_time_cy *= group_config->performance_static_seq_mul;
                    injection_time_cy += group_config->performance_static_seq_add;
                    injection_time_cy += cy_config->performance_static_add;
                    dutycycle_period *= 2.0f;
                  } else {
                    injection_time_cy *= group_config->performance_static_semiseq_mul;
                    injection_time_cy += group_config->performance_static_semiseq_add;
                    injection_time_cy += cy_config->performance_static_add;
                    injection_time_cy *= 0.5f;
                  }
                }

                if(injection_time_cy > 0.0f) {
                  pulse_time_cy = lag_time_cy + injection_time_cy;
                  dutycycle_cy = pulse_time_cy / dutycycle_period;
                } else {
                  pulse_time_cy = 0.0f;
                  dutycycle_cy = 0.0f;
                }

                if(dutycycle_cy > dutycycle_gr_max) {
                  dutycycle_gr_max = dutycycle_cy;
                }
                dutycycle_gr_mean += dutycycle_cy;
                dutycycle_gr_count++;

                if(sequentialed_mode == ECU_CORE_RUNTIME_CYLINDER_SEQUENTIAL) {
                  degrees_per_cycle = 720.0f;
                } else {
                  degrees_per_cycle = 360.0f;
                }
                runtime_cy->degrees_per_cycle = degrees_per_cycle;
                signal_prepare_advance_cy = signal_prepare_advance;

                if(group_config->phase_mode == ECU_CONFIG_INJECTION_GROUP_PHASE_MODE_ENDING) {
                  signal_prepare_advance_cy += pulse_time_cy / us_per_degree;
                }

                while(injection_phase_cy > degrees_per_cycle * 0.5f) {
                  injection_phase_cy -= degrees_per_cycle;
                }

                if(position_cy < injection_phase_cy) {
                  degrees_before_inject_cur = -position_cy + injection_phase_cy;
                } else {
                  degrees_before_inject_cur = degrees_per_cycle - position_cy + injection_phase_cy;
                }
                degrees_before_inject_prev = runtime_cy->degrees_before_inject;

                if(runtime_cy->initialized) {
                  if(degrees_before_inject_cur - degrees_before_inject_prev > 0.0f &&
                      degrees_before_inject_cur - degrees_before_inject_prev < 180.0f) {
                    degrees_before_inject_cur = degrees_before_inject_prev;
                  }

                  if(!runtime_cy->scheduled && !runtime_cy->injected) {
                    degrees_before_prepare = degrees_before_inject_cur - signal_prepare_advance_cy;
                    if(degrees_before_prepare < 0.0f) {
                      runtime_cy->phase = injection_phase_cy;
                      runtime_cy->time_lag = lag_time_cy;
                      runtime_cy->time_inject = injection_time_cy;

                      if(dutycycle_cy > group_config->dutycycle_warning) {
                        // TODO: warning!
                      }
                      if(dutycycle_cy > group_config->dutycycle_limit) {
                        runtime_cy->dutycycle_limit_flag = true;
                        // TODO: Limit warning!
                        if(group_config->dutycycle_limit_mode == ECU_CONFIG_INJECTION_GROUP_DUTYCYCLE_LIMIT_MODE_CUTOFF) {
                          pulse_time_cy = 0;
                        } else if(group_config->dutycycle_limit_mode == ECU_CONFIG_INJECTION_GROUP_DUTYCYCLE_LIMIT_MODE_CLAMP) {
                          pulse_time_cy = dutycycle_period * group_config->dutycycle_limit;
                        }
                      } else {
                        runtime_cy->dutycycle_limit_flag = false;
                      }

                      runtime_cy->time_pulse = pulse_time_cy;
                      runtime_cy->dutycycle = dutycycle_cy;

                      if(pulse_time_cy) {
                        time_to_activate = crankshaft_data->sensor_data.current.timestamp +
                            (signal_prepare_advance + degrees_before_prepare) * us_per_degree_pulsed;
                        time_to_inject = time_to_activate + pulse_time_cy;

                        err = core_timing_pulse_schedule(ctx, cy_config->output_pin,
                            time_to_activate, time_to_inject);
                        BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));
                      }

                      runtime_cy->scheduled = true;
                    }
                  } else {
                    if(degrees_before_inject_prev - degrees_before_inject_cur < -90.0f) {
                      if(runtime_cy->scheduled) {
                        runtime_cy->injected = true;
                        runtime_cy->scheduled = false;
                      }
                    } else {
                      runtime_cy->injected = false;
                    }
                  }

                } else {
                  runtime_cy->initialized = true;
                  runtime_cy->scheduled = false;
                  runtime_cy->injected = true;
                }

                runtime_cy->degrees_before_inject = degrees_before_inject_cur;

                if(output_valid) {
                  err = ecu_config_gpio_output_write(cy_config->output_pin, output_value);
                  BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));
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

          if(dutycycle_gr_count > 0) {
            dutycycle_gr_mean /= dutycycle_gr_count;
          }
          runtime_gr->dutycycle_mean = dutycycle_gr_mean;
          runtime_gr->dutycycle_max = dutycycle_gr_max;

          runtime->process_update_trigger_counter = process_update_trigger_counter_gr + 1;
        }
      } else {
        if(runtime_gr->initialized) {
          memset(runtime_gr, 0, sizeof(*runtime_gr));
        }
      }
    }
  } while(0);
}

