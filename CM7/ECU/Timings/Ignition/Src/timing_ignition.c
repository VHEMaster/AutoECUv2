/*
 * timing_ignition.c
 *
 *  Created on: Aug 14, 2024
 *      Author: VHEMaster
 */

#include "config_global.h"
#include "timing_common.h"
#include "config_hw.h"
#include "common.h"
#include "interpolation.h"

error_t ignition_init(ignition_ctx_t *ctx, const ignition_init_ctx_t *init_ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || init_ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(init_ctx->calibration_config == NULL, err = E_PARAM);

    memset(ctx, 0u, sizeof(ignition_ctx_t));
    memcpy(&ctx->init, init_ctx, sizeof(ignition_init_ctx_t));


    ctx->ready = true;

  } while(0);

  return err;
}

error_t ignition_configure(ignition_ctx_t *ctx, const ignition_config_t *config)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || config == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    ctx->configured = false;

    if(&ctx->config != config) {
      memcpy(&ctx->config, config, sizeof(ignition_config_t));
    }

    ctx->configured = true;

  } while(0);

  return err;
}

error_t ignition_reset(ignition_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    ctx->configured = false;

  } while(0);

  return err;
}

error_t ignition_get_runtime_data_ptr(ignition_ctx_t *ctx, ignition_runtime_ctx_t **runtime_data)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(runtime_data == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    *runtime_data = &ctx->runtime;

  } while(0);

  return err;
}

OPTIMIZE_FAST
ITCM_FUNC void ignition_signal_update_callback(ignition_ctx_t *ctx)
{
  error_t err;
  timing_base_crankshaft_mode_t crankshaft_mode;
  ignition_config_group_mode_t group_mode;
  input_id_t power_voltage_pin;
  input_value_t input_analog_value;
  float power_voltage;
  uint32_t banks_count;
  uint32_t cylinders_count;

  timing_base_runtime_cylinder_sequentialed_type_t sequentialed_mode;
  bool distributor;
  bool needtoclear;
  output_value_t output_value;
  bool output_valid;
  ecu_bank_t bank_cy;

  sMathInterpolateInput ip_input;
  const ignition_config_t *config;
  const ignition_config_group_setup_t *group_config;
  const timing_base_data_crankshaft_t *crankshaft;
  const ecu_config_engine_calibration_t *calibration_config;
  const timing_base_data_crankshaft_t *crankshaft_data;
  const timing_base_data_t *timing_base_data;

  bool input_valid;
  bool input_valid_b[ECU_BANK_MAX];
  bool input_allowed_b[ECU_BANK_MAX];
  bool use_ignition_acceptance;

  float signal_prepare_advance;
  ignition_runtime_ctx_t *runtime;
  ignition_runtime_group_ctx_t *runtime_gr;
  ignition_runtime_group_cylinder_ctx_t *runtime_cy;
  ecu_core_runtime_value_ctx_t *ignition_acceptance_cy;
  ecu_core_runtime_value_ctx_t *ignition_acceptance_gr_cy;
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

  do {
    // TODO: assign proper instance
    err = ecu_timings_base_get_data_ptr(ECU_TIMING_BASE_1, &timing_base_data);
    BREAK_IF_ACTION(err != E_OK, err = E_FAULT);
    BREAK_IF_ACTION(timing_base_data == NULL, err = E_FAULT);

    config = &ctx->config;
    runtime = &ctx->runtime;
    crankshaft = &timing_base_data->crankshaft;

    calibration_config = ctx->init.calibration_config;
    banks_count = calibration_config->cylinders.banks_count;
    cylinders_count = calibration_config->cylinders.cylinders_count;

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

    input_valid = banks_count ? true : false;
    for(ecu_bank_t b = 0; b < banks_count; b++) {
      input_allowed_b[b] = runtime->input_banked[b].allowed.value > ECU_RUNTIME_PARAMETER_FALSE ? true : false;
      input_valid_b[b] = runtime->input_banked[b].allowed.valid;
      input_valid_b[b] &= runtime->input_banked[b].ignition_advance.valid;
      input_valid &= input_valid_b[b];
    }
    use_ignition_acceptance = config->use_ignition_acceptance;
    runtime->signal_prepare_advance = signal_prepare_advance;

    for(ecu_cylinder_t cy = 0; cy < cylinders_count; cy++) {
      bank_cy = calibration_config->cylinders.cylinders[cy].bank;
      ignition_acceptance_cy = &runtime->cylinders[cy].ignition_acceptance;
      if(use_ignition_acceptance) {
        if(ignition_acceptance_cy->valid) {
          for(ignition_config_group_t gr = 0; gr < IGNITION_CONFIG_GROUP_MAX; gr++) {
            runtime_gr = &runtime->groups[gr];
            runtime_gr->advance_input_cy[cy] = ignition_acceptance_cy->value;
            ignition_acceptance_gr_cy = &runtime_gr->cylinders[cy].ignition_acceptance;
            ignition_acceptance_gr_cy->value = ignition_acceptance_cy->value;
            ignition_acceptance_gr_cy->valid = true;
          }
          ignition_acceptance_cy->valid = false;
        } else {
          for(ignition_config_group_t gr = 0; gr < IGNITION_CONFIG_GROUP_MAX; gr++) {
            runtime_gr = &runtime->groups[gr];
            ignition_acceptance_gr_cy = &runtime_gr->cylinders[cy].ignition_acceptance;
            if(ignition_acceptance_gr_cy->valid) {
              runtime_gr->advance_input_cy[cy] = ignition_acceptance_gr_cy->value;
            } else {
              runtime_gr->advance_input_cy[cy] = runtime->input_banked[bank_cy].ignition_advance.value;
            }
          }
        }
      } else {
        for(ignition_config_group_t gr = 0; gr < IGNITION_CONFIG_GROUP_MAX; gr++) {
          runtime_gr = &runtime->groups[gr];
          runtime_gr->advance_input_cy[cy] = runtime->input_banked[bank_cy].ignition_advance.value;
        }
      }
    }

    for(ignition_config_group_t gr = 0; gr < IGNITION_CONFIG_GROUP_MAX; gr++) {
      group_config = &config->groups[gr];
      runtime_gr = &runtime->groups[gr];
      if(group_config->enabled && input_valid) {
        process_update_trigger_counter_gr = runtime->process_update_trigger_counter;
        process_update_trigger_counter_gr_1of3 = process_update_trigger_counter_gr % 3;

        if(group_config->process_update_trigger == IGNITION_CONFIG_GROUP_PROCESS_UPDATE_TRIGGER_ALWAYS) {
          process_update_trigger = true;
        } else if(group_config->process_update_trigger == IGNITION_CONFIG_GROUP_PROCESS_UPDATE_TRIGGER_1OF3_1ST) {
          process_update_trigger = process_update_trigger_counter_gr_1of3 == 0;
        } else if(group_config->process_update_trigger == IGNITION_CONFIG_GROUP_PROCESS_UPDATE_TRIGGER_1OF3_2ND) {
          process_update_trigger = process_update_trigger_counter_gr_1of3 == 1;
        } else if(group_config->process_update_trigger == IGNITION_CONFIG_GROUP_PROCESS_UPDATE_TRIGGER_1OF3_3RD) {
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
            if(runtime_gr->initialized && crankshaft_mode >= TIMING_CRANKSHAFT_MODE_VALID) {
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
            case IGNITION_CONFIG_USPD_SOURCE_PER_SENSOR_PULSE:
              us_per_degree = us_per_degree_pulsed;
              break;
            case IGNITION_CONFIG_USPD_SOURCE_PER_REVOLUTION:
              us_per_degree = us_per_degree_revolution;
              break;
          }

          signal_prepare_advance_gr = signal_prepare_advance;
          signal_prepare_advance_gr += saturation_time / us_per_degree;

          needtoclear = false;
          distributor = false;
          sequentialed_mode = TIMING_RUNTIME_CYLINDER_SEQUENTIALED_NONE;

          group_mode = group_config->mode;
          if(crankshaft_mode >= TIMING_CRANKSHAFT_MODE_VALID) {
            if(group_mode == IGNITION_CONFIG_GROUP_MODE_DISTRIBUTOR) {
              distributor = true;
              sequentialed_mode = TIMING_RUNTIME_CYLINDER_SEMISEQUENTIAL_DISTRIBUTOR;
            } else if(crankshaft_mode >= TIMING_CRANKSHAFT_MODE_VALID &&
                group_mode == IGNITION_CONFIG_GROUP_MODE_SEMISEQUENTIAL_ONLY) {
              sequentialed_mode = TIMING_RUNTIME_CYLINDER_SEMISEQUENTIAL_DISTRIBUTOR;
            } else if(crankshaft_mode == TIMING_CRANKSHAFT_MODE_VALID_PHASED &&
                group_mode == IGNITION_CONFIG_GROUP_MODE_SEQUENTIAL_ONLY) {
              sequentialed_mode = TIMING_RUNTIME_CYLINDER_SEQUENTIAL;
            } else if(group_mode == IGNITION_CONFIG_GROUP_MODE_SEQUENTIAL_AND_SEMISEQUENTIAL) {
              if(crankshaft_mode == TIMING_CRANKSHAFT_MODE_VALID_PHASED) {
                sequentialed_mode = TIMING_RUNTIME_CYLINDER_SEQUENTIAL;
              } else {
                sequentialed_mode = TIMING_RUNTIME_CYLINDER_SEMISEQUENTIAL_DISTRIBUTOR;
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
              bank_cy = calibration_config->cylinders.cylinders[cy].bank;
              runtime_cy = &runtime_gr->cylinders[cy];
              ignition_acceptance_gr_cy = &runtime_cy->ignition_acceptance;
              if(!group_config->cylinders[cy].disabled) {
                ignition_advance_cy_add = group_config->cylinders[cy].advance_add;

                cy_opposite = calibration_config->cylinders.cylinders[cy].cy_opposed;
                if(cy_opposite < ECU_CYLINDER_MAX && cy != cy_opposite &&
                    sequentialed_mode == TIMING_RUNTIME_CYLINDER_SEMISEQUENTIAL_DISTRIBUTOR) {
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
                crankshaft_data = &timing_base_data->sequentialed[sequentialed_mode].cylinders[cy].crankshaft_data;
                position_cy = crankshaft_data->sensor_data.current_position;
                output_valid = false;

                runtime_cy->position = position_cy;
                runtime_cy->advance = ignition_advance_cy;
                if(sequentialed_mode == TIMING_RUNTIME_CYLINDER_SEQUENTIAL) {
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
                        err = timing_pulse_schedule(group_config->cylinders[cy].output_pin,
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
