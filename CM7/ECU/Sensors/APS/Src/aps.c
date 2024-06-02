/*
 * aps.c
 *
 *  Created on: May 14, 2024
 *      Author: VHEMaster
 */

#include "aps.h"
#include "compiler.h"
#include <string.h>
#include <float.h>

error_t aps_init(aps_ctx_t *ctx, const aps_init_ctx_t *init_ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || init_ctx == NULL, err = E_PARAM);

    memset(ctx, 0u, sizeof(aps_ctx_t));
    memcpy(&ctx->init, init_ctx, sizeof(aps_init_ctx_t));

    ctx->ready = true;

  } while(0);

  return err;
}

error_t aps_configure(aps_ctx_t *ctx, const aps_config_t *config)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || config == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);


    BREAK_IF_ACTION(config->signals_used_count > APS_CONFIG_SIGNALS_MAX, err = E_PARAM);
    BREAK_IF_ACTION(config->position_min_clamp >= config->position_max_clamp, err = E_PARAM);

    for(int i = 0; i < config->signals_used_count; i++) {
      BREAK_IF_ACTION(config->signals[i].voltage_low_thr >= config->signals[i].voltage_high_thr, err = E_PARAM);
      BREAK_IF_ACTION(config->signals[i].input_pin >= ECU_IN_MAX, err = E_PARAM);
    }
    BREAK_IF(err != E_OK);

    if(ctx->configured == true) {
      err = aps_reset(ctx);
      BREAK_IF(err != E_OK);
    }

    ctx->configured = false;

    if(&ctx->config != config) {
      memcpy(&ctx->config, config, sizeof(aps_config_t));
    }

    if(ctx->config.enabled == true) {
      for(int i = 0; i < ctx->config.signals_used_count; i++) {
        err = ecu_config_gpio_input_get_id(ctx->config.signals[i].input_pin, &ctx->input_ids[i]);
        BREAK_IF(err != E_OK);

        err = ecu_config_gpio_input_lock(ctx->config.signals[i].input_pin);
        BREAK_IF(err != E_OK);
        ctx->pins_locked[i] = true;

        err = ecu_config_gpio_input_set_mode(ctx->config.signals[i].input_pin, ECU_GPIO_INPUT_TYPE_ANALOG);
        BREAK_IF(err != E_OK);
        err = ecu_config_gpio_input_register_callback(ctx->config.signals[i].input_pin, NULL);
        BREAK_IF(err != E_OK);
      }
      BREAK_IF(err != E_OK);

      ctx->configured = true;
    }

  } while(0);

  if(err != E_OK) {
    (void)aps_reset(ctx);
  }

  return err;
}

error_t aps_reset(aps_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    ctx->configured = false;

    for(int i = 0; i < APS_CONFIG_SIGNALS_MAX; i++) {
      if(ctx->pins_locked[i] == true) {
        (void)ecu_config_gpio_input_unlock(ctx->config.signals[i].input_pin);
        ctx->pins_locked[i] = false;
      }
    }

  } while(0);

  return err;
}

void aps_loop_main(aps_ctx_t *ctx)
{

}

void aps_loop_slow(aps_ctx_t *ctx)
{
  time_us_t now = time_get_current_us();
  error_t err = E_OK;
  input_value_t input_analog_value;
  time_delta_us_t time_delta;
  const aps_config_signal_t *signal_cfg;
  float voltage, pos_raw, imbalance, pos_unfiltered, pos_clamp, pos, allowed_rate, pos_diff;
  float pos_min = FLT_MAX;
  float pos_max = -FLT_MAX;
  bool signal_failed;
  uint8_t signals_ok;

  do {
    BREAK_IF(ctx == NULL);
    if(ctx->configured == true) {
      time_delta = time_diff(now, ctx->signal_poll_time);

      for(int i = 0; i < ctx->config.signals_used_count; i++) {
        (void)input_get_value(ctx->input_ids[i], &input_analog_value, NULL);
        voltage = (float)input_analog_value * INPUTS_ANALOG_MULTIPLIER_R;
        ctx->data.voltages[i] = voltage;
      }

      if(ctx->started == true) {
        ctx->signal_poll_time = now;

        for(int i = 0; i < ctx->config.signals_used_count; i++) {
          signal_cfg = &ctx->config.signals[i];

          err = input_get_value(ctx->input_ids[i], &input_analog_value, NULL);
          BREAK_IF(err != E_OK && err != E_AGAIN);
          err = E_OK;

          signal_failed = false;
          if(ctx->data.voltages[i] > signal_cfg->voltage_high_thr) {
            if(i == 0) ctx->diag.bits.signal1_level_high = true;
            else if(i == 1) ctx->diag.bits.signal2_level_high = true;
            signal_failed = true;
          }

          if(ctx->data.voltages[i] < signal_cfg->voltage_low_thr) {
            if(i == 0) ctx->diag.bits.signal1_level_low = true;
            else if(i == 1) ctx->diag.bits.signal2_level_low = true;
            signal_failed = true;
          }

          pos_raw = ctx->data.voltages[i] - signal_cfg->voltage_pos_min;
          pos_raw /= signal_cfg->voltage_pos_max - signal_cfg->voltage_pos_min;
          pos_raw *= ctx->config.position_max - ctx->config.position_min;
          pos_raw += ctx->config.position_min;

          ctx->data.positions_raw[i] = pos_raw;
          ctx->signal_failed[i] = signal_failed;
          ctx->data.active[i] = !signal_failed;
        }
        BREAK_IF(err != E_OK);

        imbalance = 0;
        pos_unfiltered = 0;
        signals_ok = 0;
        for(int i = 0; i < ctx->config.signals_used_count; i++) {
          if(ctx->signal_failed[i] == false) {
            if(pos_min > ctx->data.positions_raw[i]) {
              pos_min = ctx->data.positions_raw[i];
            }
            if(pos_max < ctx->data.positions_raw[i]) {
              pos_max = ctx->data.positions_raw[i];
            }
            pos_unfiltered += ctx->data.positions_raw[i];
            signals_ok++;
          }
        }
        if(signals_ok > 0) {
          pos_unfiltered /= signals_ok;
          imbalance = pos_max - pos_min;
          if(imbalance > ctx->config.signals_position_imbalance_max) {
            ctx->diag.bits.signals_imbalance = true;
          }
        }
        ctx->data.position_imbalance = imbalance;
        ctx->data.position_unfiltered = pos_unfiltered;

        if(ctx->config.dead_zone > 0) {
          pos_unfiltered = (pos_unfiltered - ctx->config.dead_zone) / ((100.0f - ctx->config.dead_zone) * 0.01f);
        }
        pos_clamp = CLAMP(pos_unfiltered, ctx->config.position_min_clamp, ctx->config.position_max_clamp);
        pos = ctx->data.position;

        allowed_rate = ctx->config.slew_rate * ((float)time_delta * 0.000001f);
        pos_diff = pos_clamp - pos;
        pos_diff = CLAMP(pos_diff, -allowed_rate, allowed_rate);
        pos += pos_diff;

        ctx->data.position = pos;
        ctx->poll_delta = time_delta;
        ctx->data.valid = true;
      } else if(time_diff(now, ctx->startup_time) > ctx->config.boot_time) {
        ctx->started = true;
      }
    } else {
      ctx->data.valid = false;
      ctx->signal_poll_time = now;
      ctx->startup_time = now;
      ctx->data.position_imbalance = 0;
      ctx->data.position_unfiltered = 0;
      ctx->data.position = 0;
    }
  } while(0);
}

ITCM_FUNC void aps_loop_fast(aps_ctx_t *ctx)
{

}

error_t aps_get_value(aps_ctx_t *ctx, aps_data_t *data)
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

error_t aps_get_diag(aps_ctx_t *ctx, aps_diag_t *diag)
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
