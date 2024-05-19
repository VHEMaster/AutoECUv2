/*
 * maf.c
 *
 *  Created on: May 14, 2024
 *      Author: VHEMaster
 */

#include "maf.h"
#include "compiler.h"
#include "interpolation.h"
#include <string.h>

error_t maf_init(maf_ctx_t *ctx, const maf_init_ctx_t *init_ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || init_ctx == NULL, err = E_PARAM);

    memset(ctx, 0u, sizeof(maf_ctx_t));
    memcpy(&ctx->init, init_ctx, sizeof(maf_init_ctx_t));

    ctx->ready = true;

  } while(0);

  return err;
}

static void maf_gpio_input_cb(ecu_gpio_input_pin_t pin, ecu_gpio_input_level_t level, void *usrdata)
{
  maf_ctx_t *ctx = (maf_ctx_t *)usrdata;
  time_us_t now = time_get_current_us();
  time_delta_us_t delta;
  float freq;
  uint8_t index;

  if(ctx != NULL && ctx->configured != false) {
    if(ctx->config.signal_mode == MAF_SIGNAL_MODE_FREQUENCY) {
      if(level == ECU_IN_LEVEL_LOW) {
        index = 0;
      } else if(level == ECU_IN_LEVEL_HIGH) {
        index = 1;
      } else {
        index = ctx->input_freq_index;
      }

      if(ctx->input_freq_times[index] > 0) {
        delta = time_diff(now, ctx->input_freq_times[index]);
        if(delta > 0) {
          freq = 1000000.0f / (float)delta;
          ctx->input_freq_values[index] = freq;
        }
      }
      ctx->input_freq_times[index] = now;
    }

    if(++ctx->input_freq_index >= MAF_FREQ_ITEMS_COUNT) {
      ctx->input_freq_index = 0;
    }
  }
}

error_t maf_configure(maf_ctx_t *ctx, const maf_config_t *config)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || config == NULL, err = E_PARAM);
    BREAK_IF_ACTION(config->signal_mode >= MAF_SIGNAL_MODE_MAX, err = E_PARAM);
    BREAK_IF_ACTION(config->calc_mode >= MAF_CALC_MODE_MAX, err = E_PARAM);
    BREAK_IF_ACTION(config->signal_voltage_to_value.table.items > MAF_RELATION_ITEMS_MAX, err = E_PARAM);
    BREAK_IF_ACTION(config->signal_frequency_to_value.table.items > MAF_RELATION_ITEMS_MAX, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    if(ctx->configured == true) {
      err = maf_reset(ctx);
      BREAK_IF(err != E_OK);
    }

    ctx->configured = false;

    if(&ctx->config != config) {
      memcpy(&ctx->config, config, sizeof(maf_config_t));
    }

    if(ctx->config.enabled == true) {
      err = ecu_config_gpio_input_get_id(ctx->config.input_pin, &ctx->input_id);
      BREAK_IF(err != E_OK);

      err = ecu_config_gpio_input_lock(ctx->config.input_pin);
      BREAK_IF(err != E_OK);
      ctx->pin_locked = true;

      switch(ctx->config.signal_mode) {
        case MAF_SIGNAL_MODE_ANALOG:
          err = ecu_config_gpio_input_set_mode(ctx->config.input_pin, ECU_GPIO_INPUT_TYPE_ANALOG);
          BREAK_IF(err != E_OK);
          err = ecu_config_gpio_input_register_callback(ctx->config.input_pin, NULL);
          BREAK_IF(err != E_OK);
          break;
        case MAF_SIGNAL_MODE_FREQUENCY:
          err = ecu_config_gpio_input_set_mode(ctx->config.input_pin, ECU_GPIO_INPUT_TYPE_CAPTURE);
          BREAK_IF(err != E_OK);
          err = ecu_config_gpio_input_set_capture_edge(ctx->config.input_pin, ECU_IN_CAPTURE_EDGE_BOTH);
          BREAK_IF(err != E_OK);
          err = ecu_config_gpio_input_set_usrdata(ctx->config.input_pin, ctx);
          BREAK_IF(err != E_OK);
          err = ecu_config_gpio_input_register_callback(ctx->config.input_pin, maf_gpio_input_cb);
          BREAK_IF(err != E_OK);
          break;
        default:
          err = E_PARAM;
      }
      BREAK_IF(err != E_OK);

      ctx->configured = true;
    }

  } while(0);

  if(err != E_OK) {
    (void)maf_reset(ctx);
  }

  return err;
}

error_t maf_reset(maf_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    ctx->configured = false;

    if(ctx->pin_locked == true) {
      (void)ecu_config_gpio_input_unlock(ctx->config.input_pin);
      ctx->pin_locked = false;
    }

  } while(0);

  return err;
}

void maf_loop_main(maf_ctx_t *ctx)
{

}

void maf_loop_slow(maf_ctx_t *ctx)
{
  time_us_t now = time_get_current_us();
  error_t err = E_OK;
  input_value_t input_analog_value;
  uint8_t freq_cnt;
  float freq, freq_res, value_div_step;
  sMathInterpolateInput interpolate_input = {0};
  const maf_config_signal_mode_cfg_t *signal_mode_cfg = NULL;

  do {
    BREAK_IF(ctx == NULL);
    if(ctx->configured == true) {
      if(ctx->started == true) {
        if(ctx->config.signal_mode == MAF_SIGNAL_MODE_ANALOG) {
          err = input_get_value(ctx->input_id, &input_analog_value, NULL);
          BREAK_IF(err != E_OK && err != E_AGAIN);

          ctx->data.input_value = (float)input_analog_value * INPUTS_ANALOG_MULTIPLIER_R;
          signal_mode_cfg = &ctx->config.signal_voltage_to_value;

          if(ctx->data.input_value > signal_mode_cfg->input_high) {
            ctx->diag.level_high = true;
          } else {
            ctx->diag.level_high = false;
          }

          if(ctx->data.input_value < signal_mode_cfg->input_low) {
            ctx->diag.level_low = true;
          } else {
            ctx->diag.level_low = false;
          }
        } else if(ctx->config.signal_mode == MAF_SIGNAL_MODE_FREQUENCY) {
          signal_mode_cfg = &ctx->config.signal_frequency_to_value;
          freq_cnt = 0;
          freq_res = 0;
          for(int i = 0; i < MAF_FREQ_ITEMS_COUNT; i++) {
            if(ctx->input_freq_values[i] > 0) {
              freq = ctx->input_freq_values[i];
              freq_res += freq;
              freq_cnt++;

              if(freq > ctx->config.signal_frequency_to_value.input_high) {
                ctx->diag.freq_high = true;
              } else {
                ctx->diag.freq_high = false;
              }
              if(freq < ctx->config.signal_frequency_to_value.input_low) {
                ctx->diag.freq_low = true;
              } else {
                ctx->diag.freq_low = false;
              }
            }
          }

          if(freq_cnt > 0) {
            freq_res /= freq_cnt;
            ctx->diag.freq_no_signal = false;
          } else {
            ctx->diag.freq_no_signal = true;
          }
          ctx->data.input_value = freq_res;

          ctx->data.output_value = 000;
        }

        if(signal_mode_cfg != NULL) {
          if(ctx->config.calc_mode == MAF_CALC_MODE_TABLE_REF_VALUE) {
            interpolate_input = math_interpolate_input(ctx->data.input_value, signal_mode_cfg->table.input, signal_mode_cfg->table.items);
            ctx->data.output_value = math_interpolate_1d(interpolate_input, signal_mode_cfg->table.output);
          } else if(ctx->config.calc_mode == MAF_CALC_MODE_TABLE_VALUE) {
            value_div_step = ctx->data.input_value / signal_mode_cfg->input_step;
            interpolate_input.input = ctx->data.input_value;
            interpolate_input.indexes[0] = floorf(value_div_step);
            interpolate_input.indexes[1] = interpolate_input.indexes[0] + 1;
            interpolate_input.mult = value_div_step - interpolate_input.indexes[0];
            interpolate_input.indexes[0] = CLAMP(interpolate_input.indexes[0], 0, signal_mode_cfg->table.items - 1);
            interpolate_input.indexes[1] = CLAMP(interpolate_input.indexes[1], 0, signal_mode_cfg->table.items - 1);

            ctx->data.output_value = math_interpolate_1d(interpolate_input, signal_mode_cfg->table.output);
          } else if(ctx->config.calc_mode == MAF_CALC_MODE_OFFSET_GAIN) {
            ctx->data.output_value = ctx->data.input_value * signal_mode_cfg->gain + signal_mode_cfg->offset;
          } else {
            ctx->data.output_value = 0;
          }
        } else {
          ctx->data.output_value = 0;
        }

      } else if(time_diff(now, ctx->startup_time) > ctx->config.boot_time) {
        ctx->started = true;
      }
    } else {
      ctx->startup_time = now;
      ctx->data.input_value = 0;
      ctx->data.output_value = 0;
      memset(ctx->input_freq_times, 0, sizeof(ctx->input_freq_times));
      memset(ctx->input_freq_values, 0, sizeof(ctx->input_freq_times));
    }
  } while(0);
}

ITCM_FUNC void maf_loop_fast(maf_ctx_t *ctx)
{

}

error_t maf_get_value(maf_ctx_t *ctx, maf_data_t *data)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(data == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);
    BREAK_IF_ACTION(ctx->configured == false, err = E_NOTRDY);

    *data = ctx->data;

  } while(0);

  return err;
}

error_t maf_get_diag(maf_ctx_t *ctx, maf_diag_t *diag)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(diag == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);
    BREAK_IF_ACTION(ctx->configured == false, err = E_NOTRDY);

    *diag = ctx->diag;

  } while(0);

  return err;
}
