/*
 * maf.c
 *
 *  Created on: May 14, 2024
 *      Author: VHEMaster
 */

#include "maf.h"
#include "compiler.h"
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
  float delta_flt;
  uint8_t index;

  if(ctx->config.signal_mode == MAF_SIGNAL_MODE_FREQUENCY) {
    if(level == ECU_IN_LEVEL_LOW) {
      index = 0;
    } else if(level == ECU_IN_LEVEL_HIGH) {
      index = 1;
    } else {
      index = ctx->input_freq_index;
    }

    delta = time_diff(now, ctx->input_freq_times[index]);
    ctx->input_freq_times[index] = now;
    delta_flt = delta;

    index ^= 1;

    delta = time_diff(now, ctx->input_freq_times[index]);
    ctx->input_freq_times[index] = now;
    delta_flt += delta;

    delta_flt *= 0.5f;
    ctx->input_value = 1000000.0f / delta_flt;

    if(ctx->input_value > ctx->config.signal_frequency_to_value.input_high) {
      ctx->diag_value |= MAF_DIAG_FREQ_HIGH;
    } else {
      ctx->diag_value &= ~MAF_DIAG_FREQ_HIGH;
    }
    if(ctx->input_value < ctx->config.signal_frequency_to_value.input_low) {
      ctx->diag_value |= MAF_DIAG_FREQ_LOW;
    } else {
      ctx->diag_value &= ~MAF_DIAG_FREQ_LOW;
    }

  }

  if(++ctx->input_freq_index >= 2) {
    ctx->input_freq_index = 0;
  }
}

error_t maf_configure(maf_ctx_t *ctx, const maf_config_t *config)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || config == NULL, err = E_PARAM);
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
  time_delta_us_t time_delta;
  float freq;

  do {
    BREAK_IF(ctx == NULL);
    if(ctx->configured == true) {
      if(ctx->started == true) {
        if(ctx->config.signal_mode == MAF_SIGNAL_MODE_ANALOG) {
          err = input_get_value(ctx->input_id, &input_analog_value, NULL);
          BREAK_IF(err != E_OK && err != E_AGAIN);

          ctx->input_value = (float)input_analog_value * INPUTS_ANALOG_MULTIPLIER_R;

          if(ctx->input_value > ctx->config.signal_voltage_to_value.input_high) {
            ctx->diag_value |= MAF_DIAG_LEVEL_HIGH;
          } else {
            ctx->diag_value &= ~MAF_DIAG_LEVEL_HIGH;
          }

          if(ctx->input_value < ctx->config.signal_voltage_to_value.input_low) {
            ctx->diag_value |= MAF_DIAG_LEVEL_LOW;
          } else {
            ctx->diag_value &= ~MAF_DIAG_LEVEL_LOW;
          }

          ctx->output_value = 000;

        } else if(ctx->config.signal_mode == MAF_SIGNAL_MODE_FREQUENCY) {
          for(int i = 0; i < 2; i++) {
            time_delta = time_diff(now, ctx->input_freq_times[i]);
            if(time_delta > 0) {
              freq = 1000000.0f / time_delta;

              if(freq < ctx->config.signal_frequency_to_value.input_low) {
                ctx->diag_value |= MAF_DIAG_FREQ_LOW;
              } else {
                ctx->diag_value &= ~MAF_DIAG_FREQ_LOW;
              }
            }
          }

          ctx->output_value = 000;
        }
      } else if(time_diff(now, ctx->startup_time) > ctx->config.boot_time) {
        ctx->started = true;
      }
    } else {
      ctx->startup_time = now;
      ctx->input_value = 0;
      ctx->output_value = 0;
      ctx->input_freq_times[0] = 0;
      ctx->input_freq_times[1] = 0;
    }
  } while(0);
}

ITCM_FUNC void maf_loop_fast(maf_ctx_t *ctx)
{

}
