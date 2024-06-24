/*
 * vss.c
 *
 *  Created on: Jun 2, 2024
 *      Author: VHEMaster
 */

#include "vss.h"
#include "compiler.h"
#include "interpolation.h"
#include <string.h>

error_t vss_init(vss_ctx_t *ctx, const vss_init_ctx_t *init_ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || init_ctx == NULL, err = E_PARAM);

    memset(ctx, 0u, sizeof(vss_ctx_t));
    memcpy(&ctx->init, init_ctx, sizeof(vss_init_ctx_t));

    ctx->ready = true;

  } while(0);

  return err;
}

static void vss_gpio_input_cb(ecu_gpio_input_pin_t pin, ecu_gpio_input_level_t level, void *usrdata)
{
  vss_ctx_t *ctx = (vss_ctx_t *)usrdata;
  time_us_t now = time_get_current_us();
  time_delta_us_t delta;
  float freq;
  uint8_t index, freq_cnt;

  if(ctx != NULL && ctx->configured != false) {
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
        if(ctx->input_freq_count < VSS_FREQ_ITEMS_COUNT) {
          ctx->input_freq_count++;
        }
      }
    }
    ctx->input_freq_times[index] = now;

    freq = 0;
    freq_cnt = ctx->input_freq_count;
    if(freq_cnt) {
      for(int i = 0; i < freq_cnt; i++) {
        freq += ctx->input_freq_values[i];
      }
      freq /= freq_cnt;
    }
    ctx->data.pulses_per_second = freq;

    if(++ctx->input_freq_index >= VSS_FREQ_ITEMS_COUNT) {
      ctx->input_freq_index = 0;
    }

    ctx->freq_last_time = now;
  }
}

error_t vss_configure(vss_ctx_t *ctx, const vss_config_t *config)
{
  error_t err = E_OK;
  bool support = false;

  do {
    BREAK_IF_ACTION(ctx == NULL || config == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    if(ctx->configured == true) {
      err = vss_reset(ctx);
      BREAK_IF(err != E_OK);
    }

    ctx->configured = false;

    if(&ctx->config != config) {
      memcpy(&ctx->config, config, sizeof(vss_config_t));
    }

    if(ctx->config.enabled == true) {
      err = ecu_config_gpio_input_get_id(ctx->config.input_pin, &ctx->input_id);
      BREAK_IF(err != E_OK);

      err = ecu_config_gpio_input_lock(ctx->config.input_pin);
      BREAK_IF(err != E_OK);
      ctx->pin_locked = true;

      err = ecu_config_gpio_input_has_mode_support(ctx->config.input_pin, ECU_GPIO_INPUT_TYPE_CAPTURE, &support);
      BREAK_IF(err != E_OK);
      if(support != false) {
        err = ecu_config_gpio_input_set_mode(ctx->config.input_pin, ECU_GPIO_INPUT_TYPE_CAPTURE);
        BREAK_IF(err != E_OK);
      } else {
        err = ecu_config_gpio_input_has_mode_support(ctx->config.input_pin, ECU_GPIO_INPUT_TYPE_DIGITAL, &support);
        BREAK_IF(err != E_OK);
        if(support != false) {
          err = ecu_config_gpio_input_set_mode(ctx->config.input_pin, ECU_GPIO_INPUT_TYPE_DIGITAL);
          BREAK_IF(err != E_OK);
        } else {
          err = E_NOTSUPPORT;
        }
      }
      BREAK_IF(err != E_OK);

      err = ecu_config_gpio_input_set_capture_edge(ctx->config.input_pin, ECU_IN_CAPTURE_EDGE_BOTH);
      BREAK_IF(err != E_OK);
      err = ecu_config_gpio_input_set_usrdata(ctx->config.input_pin, ctx);
      BREAK_IF(err != E_OK);
      err = ecu_config_gpio_input_register_callback(ctx->config.input_pin, vss_gpio_input_cb);
      BREAK_IF(err != E_OK);

      ctx->configured = true;
    }

  } while(0);

  if(err != E_OK) {
    (void)vss_reset(ctx);
  }

  return err;
}

error_t vss_reset(vss_ctx_t *ctx)
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

void vss_loop_slow(vss_ctx_t *ctx)
{
  time_us_t last_time;
  time_us_t now;
  time_delta_us_t time_delta;
  bool cleanup_trigger = false;

  do {
    BREAK_IF(ctx == NULL);

    last_time = ctx->freq_last_time;
    now = time_get_current_us();

    if(ctx->configured == true) {
      if(ctx->started == true) {
        if(ctx->freq_last_time != 0 && ctx->data.pulses_per_second > 0) {
          time_delta = time_diff(now, last_time);
          if(time_delta > VSS_FREQ_TIMEOUT_US) {
            cleanup_trigger = true;
          }

          ctx->data.speed = ctx->data.pulses_per_second / ctx->config.pulses_per_meter;
          ctx->data.speed *= 3.6f;
          ctx->data.speed *= ctx->config.input_correction;

          if(ctx->data.speed > ctx->config.speed_max) {
            ctx->diag.bits.speed_too_high = true;
          } else {
            ctx->diag.bits.speed_too_high = false;
          }
        } else {
          ctx->data.pulses_per_second = 0.0f;
          ctx->data.speed = 0.0f;
        }
        ctx->data.valid = true;
      } else if(time_diff(now, ctx->startup_time) > ctx->config.boot_time) {
        ctx->started = true;
      }
    } else {
      cleanup_trigger = true;
      ctx->startup_time = now;
    }

    if(cleanup_trigger) {
      ctx->freq_last_time = 0;
      ctx->input_freq_count = 0;
      ctx->input_freq_index = 0;

      ctx->data.pulses_per_second = 0;
      ctx->data.speed = 0;

      memset(ctx->input_freq_times, 0, sizeof(ctx->input_freq_times));
      memset(ctx->input_freq_values, 0, sizeof(ctx->input_freq_values));
    }
  } while(0);
}

error_t vss_get_value(vss_ctx_t *ctx, vss_data_t *data)
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

error_t vss_get_diag(vss_ctx_t *ctx, vss_diag_t *diag)
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
