/*
 * ots.c
 *
 *  Created on: Jun 24, 2024
 *      Author: VHEMaster
 */

#include "ots.h"
#include "compiler.h"
#include "interpolation.h"
#include <math.h>
#include <string.h>

error_t ots_init(ots_ctx_t *ctx, const ots_init_ctx_t *init_ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || init_ctx == NULL, err = E_PARAM);

    memset(ctx, 0u, sizeof(ots_ctx_t));
    memcpy(&ctx->init, init_ctx, sizeof(ots_init_ctx_t));

    ctx->ready = true;

  } while(0);

  return err;
}

error_t ots_configure(ots_ctx_t *ctx, const ots_config_t *config)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || config == NULL, err = E_PARAM);
    BREAK_IF_ACTION(config->signal_mode >= OTS_SIGNAL_MODE_MAX, err = E_PARAM);
    BREAK_IF_ACTION(config->calc_mode >= OTS_CALC_MODE_MAX, err = E_PARAM);

    if(config->calc_mode == OTS_CALC_MODE_TABLE_REF_VALUE || config->calc_mode == OTS_CALC_MODE_TABLE_VALUE) {
      BREAK_IF_ACTION(config->signal_voltage_to_value.table.items > OTS_RELATION_ITEMS_MAX, err = E_PARAM);
      BREAK_IF_ACTION(config->signal_resistance_to_value.table.items > OTS_RELATION_ITEMS_MAX, err = E_PARAM);
      BREAK_IF_ACTION(config->signal_mode == OTS_SIGNAL_MODE_VOLTAGE && config->signal_voltage_to_value.table.items < OTS_RELATION_ITEMS_MIN, err = E_PARAM);
      BREAK_IF_ACTION(config->signal_mode == OTS_SIGNAL_MODE_RESISTANCE && config->signal_resistance_to_value.table.items < OTS_RELATION_ITEMS_MIN, err = E_PARAM);
    } else if(config->calc_mode == OTS_CALC_MODE_RESISTANCE_CALCULATED) {
      BREAK_IF_ACTION(config->signal_mode != OTS_SIGNAL_MODE_RESISTANCE, err = E_PARAM);
    }

    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    if(ctx->configured == true) {
      err = ots_reset(ctx);
      BREAK_IF(err != E_OK);
    }

    ctx->configured = false;

    if(&ctx->config != config) {
      memcpy(&ctx->config, config, sizeof(ots_config_t));
    }

    if(ctx->config.enabled == true) {
      err = ecu_config_gpio_input_get_id(ctx->config.input_pin, &ctx->input_id);
      BREAK_IF(err != E_OK);

      err = ecu_config_gpio_input_lock(ctx->config.input_pin);
      BREAK_IF(err != E_OK);
      ctx->pin_locked = true;

      err = ecu_config_gpio_input_set_mode(ctx->config.input_pin, ECU_GPIO_INPUT_TYPE_ANALOG);
      BREAK_IF(err != E_OK);

      err = ecu_config_gpio_input_register_callback(ctx->config.input_pin, NULL);
      BREAK_IF(err != E_OK);

      ctx->configured = true;
    }

  } while(0);

  if(err != E_OK) {
    (void)ots_reset(ctx);
  }

  return err;
}

error_t ots_reset(ots_ctx_t *ctx)
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

void ots_loop_slow(ots_ctx_t *ctx)
{
  error_t err = E_OK;
  input_value_t input_analog_value;
  sMathInterpolateInput interpolate_input = {0};
  const ots_config_signal_mode_cfg_t *signal_mode_cfg = NULL;
  float value_div_step, value, value_old, allowed_rate, value_diff;
  time_us_t now = time_now_us();
  time_delta_us_t time_delta;
  bool data_valid = false;

  do {
    BREAK_IF(ctx == NULL);
    if(ctx->configured == true) {
      if(ctx->started == true) {
        data_valid = true;

        time_delta = time_diff(now, ctx->poll_time);

        err = input_get_value(ctx->input_id, &input_analog_value, NULL);
        BREAK_IF(err != E_OK && err != E_AGAIN);

        ctx->data.input_value = (float)input_analog_value * INPUTS_ANALOG_MULTIPLIER_R;
        switch(ctx->config.signal_mode) {
          case OTS_SIGNAL_MODE_VOLTAGE:
            signal_mode_cfg = &ctx->config.signal_voltage_to_value;
            break;
          case OTS_SIGNAL_MODE_RESISTANCE:
            signal_mode_cfg = &ctx->config.signal_resistance_to_value;
            break;
          default:
            break;
        }

        if(ctx->data.input_value > signal_mode_cfg->input_high) {
          ctx->diag.bits.level_high = true;
          data_valid = false;
        }

        if(ctx->data.input_value < signal_mode_cfg->input_low) {
          ctx->diag.bits.level_low = true;
          data_valid = false;
        }

        if(signal_mode_cfg != NULL) {
          if(ctx->config.calc_mode == OTS_CALC_MODE_TABLE_REF_VALUE) {
            interpolate_input = math_interpolate_input(ctx->data.input_value, signal_mode_cfg->table.input, signal_mode_cfg->table.items);
            value = math_interpolate_1d(interpolate_input, signal_mode_cfg->table.output);
          } else if(ctx->config.calc_mode == OTS_CALC_MODE_TABLE_VALUE) {
            value_div_step = ctx->data.input_value / signal_mode_cfg->input_step;
            interpolate_input.input = ctx->data.input_value;
            interpolate_input.indexes[0] = floorf(value_div_step);
            interpolate_input.indexes[1] = interpolate_input.indexes[0] + 1;
            interpolate_input.mult = value_div_step - interpolate_input.indexes[0];
            interpolate_input.indexes[0] = CLAMP(interpolate_input.indexes[0], 0, signal_mode_cfg->table.items - 1);
            interpolate_input.indexes[1] = CLAMP(interpolate_input.indexes[1], 0, signal_mode_cfg->table.items - 1);

            value = math_interpolate_1d(interpolate_input, signal_mode_cfg->table.output);
          } else if(ctx->config.calc_mode == OTS_CALC_MODE_RESISTANCE_CALCULATED) {
            value = 1.0f / ctx->config.signal_resistance_calculated.thermistor_beta;
            value *= logf(ctx->data.input_value / ctx->config.signal_resistance_calculated.calibration_resistance);
            value += 1.0f / (ctx->config.signal_resistance_calculated.calibration_temperature + 273.0f);
            value = 1.0f / value;
            value -= 273.0f;

          } else {
            data_valid = false;
            value = 0;
          }
        } else {
          data_valid = false;
          value = 0;
        }


        if(ctx->poll_time != 0) {
          value_old = ctx->data.temperature;
          allowed_rate = ctx->config.slew_rate * ((float)time_delta * 0.000001f);
          value_diff = value - value_old;
          value_diff = CLAMP(value_diff, -allowed_rate, allowed_rate);
          value = value_old + value_diff;
        }

        ctx->data.temperature = value;
        ctx->data.valid = data_valid;
        ctx->poll_delta = time_delta;

        ctx->poll_time = now;
      } else if(time_diff(now, ctx->startup_time) > ctx->config.boot_time) {
        ctx->started = true;
      }
    } else {
      ctx->startup_time = now;
      ctx->poll_time = 0;
      ctx->data.input_value = 0;
      ctx->data.temperature = 0;
    }
  } while(0);
}

error_t ots_get_value(ots_ctx_t *ctx, ots_data_t *data)
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

error_t ots_get_diag(ots_ctx_t *ctx, ots_diag_t *diag)
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
