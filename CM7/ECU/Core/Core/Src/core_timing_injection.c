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

volatile float DEBUG_INJ_PHASE = 280;
volatile float DEBUG_INJ_MASS = 32; //mg

ITCM_FUNC void core_timing_signal_update_injection(ecu_core_ctx_t *ctx)
{
  error_t err;
  float injection_phase = DEBUG_INJ_PHASE;
  float injection_mass = DEBUG_INJ_MASS;

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

  sMathInterpolateInput ip_input;
  const ecu_config_injection_group_setup_t *group_config;
  const ecu_config_injection_group_cylinder_setup_t *cy_config;

  float signal_prepare_advance;
  ecu_core_runtime_global_injection_group_ctx_t *runtime_gr;
  ecu_core_runtime_group_cylinder_injection_ctx_t *runtime_cy;
  ecu_cylinder_t cy_opposite;

  float lag_time_gr;
  float lag_time_cy;
  float pulse_time_cy;
  float injection_flow_us_gr;
  float injection_mass_us_gr;
  float injection_time_gr;
  float degrees_per_cycle;
  float performance_initial_us_gr;
  float performance_mult_gr;
  float ramp_pressure_gr;

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

  map_data_t map_data;

  do {
    err = ecu_config_gpio_input_get_id(ctx->calibration->injection.power_voltage_pin, &power_voltage_pin);
    if(err == E_OK) {
      (void)input_get_value(power_voltage_pin, &input_analog_value, NULL);
      power_voltage = (float)input_analog_value * INPUTS_ANALOG_MULTIPLIER_R;
      ctx->runtime.global.injection.power_voltage = power_voltage;
    } else {
      power_voltage = ctx->runtime.global.injection.power_voltage;
    }

    crankshaft_mode = ctx->timing_data.crankshaft.mode;
    crankshaft_period = ctx->timing_data.crankshaft.sensor_data.period;
    signal_prepare_advance = ctx->calibration->injection.signal_prepare_advance;
    crankshaft_signal_delta = time_diff(ctx->timing_data.crankshaft.sensor_data.current.timestamp,
        ctx->timing_data.crankshaft.sensor_data.previous.timestamp);

    ctx->runtime.global.injection.injection_phase = injection_phase;
    ctx->runtime.global.injection.signal_prepare_advance = signal_prepare_advance;

    for(ecu_config_injection_group_t gr = 0; gr < ECU_CONFIG_INJECTION_GROUP_MAX; gr++) {
      group_config = &ctx->calibration->injection.groups[gr];
      runtime_gr = &ctx->runtime.global.injection.groups[gr];
      if(group_config->enabled) {
        ip_input = math_interpolate_input(power_voltage, group_config->voltage_to_performance_dynamic.input, group_config->voltage_to_performance_dynamic.items);
        lag_time_gr = math_interpolate_1d(ip_input, group_config->voltage_to_performance_dynamic.output);
        runtime_gr->lag_time = lag_time_gr;

        ip_input = math_interpolate_input(power_voltage, group_config->rpm_to_phase_add.input, group_config->rpm_to_phase_add.items);
        injection_phase_gr_add_rpm = math_interpolate_1d(ip_input, group_config->voltage_to_performance_dynamic.output);

        injection_phase_gr_requested = injection_phase + group_config->phase_add + injection_phase_gr_add_rpm;
        runtime_gr->phase_requested = injection_phase_gr_requested;

        us_per_degree_pulsed = ctx->timing_data.crankshaft.sensor_data.us_per_degree_pulsed;
        us_per_degree_revolution = ctx->timing_data.crankshaft.sensor_data.us_per_degree_revolution;

        if(runtime_gr->initialized) {
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


        performance_mult_gr = 1.0f;
        ramp_pressure_gr = group_config->performance_fuel_pressure;
        if(group_config->performance_fuel_pressure_source == ECU_CONFIG_INJECTION_GROUP_PERF_PRESSURE_SOURCE_RELATIVE_MAP) {
          err = ecu_sensors_map_get_value(group_config->performance_fuel_pressure_map, &map_data);
          if(err == E_OK) {
            if(map_data.valid) {
              ramp_pressure_gr = group_config->performance_fuel_pressure + map_data.manifold_air_pressure - 0.986923f;
              performance_mult_gr = fast_rsqrt(group_config->performance_fuel_pressure / ramp_pressure_gr);
            }
          } else {
            // TODO: error flag
          }
        }

        performance_initial_us_gr = group_config->performance_static;
        performance_initial_us_gr *= 1.6666667e-5;
        performance_initial_us_gr *= performance_mult_gr;

        if(group_config->performance_fuel_mass_units == ECU_CONFIG_INJECTION_GROUP_PERF_UNITS_CC) {
          injection_flow_us_gr = performance_initial_us_gr;
          injection_mass_us_gr = injection_flow_us_gr * group_config->performance_fuel_mass_per_cc * 0.001f;
        } else {
          injection_mass_us_gr = performance_initial_us_gr;
          injection_flow_us_gr = injection_mass_us_gr / group_config->performance_fuel_mass_per_cc * 0.001f;
        }
        injection_time_gr = injection_mass / injection_mass_us_gr;

        runtime_gr->phase = injection_phase_gr;
        runtime_gr->time_inject = injection_time_gr;

        runtime_gr->initialized = true;

        switch(ctx->calibration->injection.uspd_source) {
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

        if(runtime_gr->sequentialed_mode != sequentialed_mode) {
          runtime_gr->sequentialed_mode = sequentialed_mode;
          needtoclear = true;
        }
        if(!needtoclear) {
          dutycycle_gr_max = 0;
          dutycycle_gr_mean = 0;
          dutycycle_gr_count = 0;

          for(ecu_cylinder_t cy = 0; cy < ctx->runtime.global.cylinders_count; cy++) {
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

              injection_time_cy = injection_time_gr;
              injection_time_cy *= cy_config->performance_static_mul;
              dutycycle_period = ctx->timing_data.crankshaft.sensor_data.period;

              if(sequentialed_mode == ECU_CORE_RUNTIME_CYLINDER_SEQUENTIAL) {
                injection_time_cy *= group_config->performance_static_seq_mul;
                injection_time_cy += group_config->performance_static_seq_add;
                injection_time_cy += cy_config->performance_static_add;
              } else {
                injection_time_cy *= group_config->performance_static_semiseq_mul;
                injection_time_cy += group_config->performance_static_semiseq_add;
                injection_time_cy += cy_config->performance_static_add;
                injection_time_cy *= 0.5f;
                dutycycle_period *= 0.5f;
              }

              pulse_time_cy = lag_time_cy + injection_time_cy;
              dutycycle_cy = pulse_time_cy / dutycycle_period;

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
          runtime_gr->dutycycle_mean = dutycycle_gr_mean / dutycycle_gr_count;
          runtime_gr->dutycycle_max = dutycycle_gr_max;
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

