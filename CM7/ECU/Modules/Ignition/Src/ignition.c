/*
 * ignition.c
 *
 *  Created on: Jun 21, 2024
 *      Author: VHEMaster
 */

#include "ignition.h"
#include "compiler.h"
#include <string.h>

error_t ignition_init(ignition_ctx_t *ctx, const ignition_init_ctx_t *init_ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || init_ctx == NULL, err = E_PARAM);

    memset(ctx, 0u, sizeof(ignition_ctx_t));
    memcpy(&ctx->init, init_ctx, sizeof(ignition_init_ctx_t));

    ctx->ready = true;

  } while(0);

  return err;
}

error_t ignition_configure(ignition_ctx_t *ctx, const ignition_config_t *config)
{
  error_t err = E_OK;
  time_us_t now;
  bool valid;

  do {
    BREAK_IF_ACTION(ctx == NULL || config == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    if(ctx->configured != false) {
      err = ignition_reset(ctx);
      BREAK_IF(err != E_OK);
    }

    ctx->configured = false;

    if(&ctx->config != config) {
      memcpy(&ctx->config, config, sizeof(ignition_config_t));
    }

    if(ctx->config.enabled == true) {
      err = ecu_config_gpio_input_valid(ctx->config.input_signal_pin, &valid);
      BREAK_IF(err != E_OK);
      BREAK_IF_ACTION(valid != true, err = E_PARAM);

      if(valid) {
        err = ecu_config_gpio_input_lock(ctx->config.input_signal_pin);
        BREAK_IF(err != E_OK);
        ctx->input_signal_pin_locked = true;

        err = ecu_config_gpio_input_set_mode(ctx->config.input_signal_pin, ECU_GPIO_INPUT_TYPE_DIGITAL);
        BREAK_IF(err != E_OK);

        err = ecu_config_gpio_input_get_id(ctx->config.input_signal_pin, &ctx->input_signal_pin);
        BREAK_IF(err != E_OK);
      }

      err = ecu_config_gpio_output_valid(ctx->config.output_relay_pin, &valid);
      BREAK_IF(err != E_OK);

      if(valid) {
        err = ecu_config_gpio_output_lock(ctx->config.output_relay_pin);
        BREAK_IF(err != E_OK);
        ctx->output_relay_pin_locked = true;

        err = ecu_config_gpio_output_set_mode(ctx->config.output_relay_pin, ECU_GPIO_TYPE_DIRECT);
        BREAK_IF(err != E_OK);

        err = ecu_config_gpio_output_get_id(ctx->config.output_relay_pin, &ctx->output_relay_pin);
        BREAK_IF(err != E_OK);
      }

      now = time_get_current_us();
      ctx->status_time = now;
      ctx->output_time = now;
      ctx->operating_time = now;
      ctx->crankshaft_time = now;

      ctx->configured = true;
    }

  } while(0);

  if(err != E_OK) {
    (void)ignition_reset(ctx);
  }

  return err;
}

error_t ignition_reset(ignition_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    if(ctx->input_signal_pin_locked != false) {
      (void)ecu_config_gpio_input_unlock(ctx->config.input_signal_pin);
      ctx->input_signal_pin_locked = false;
    }

    if(ctx->output_relay_pin_locked != false) {
      (void)ecu_config_gpio_output_unlock(ctx->config.output_relay_pin);
      ctx->output_relay_pin_locked = false;
    }
    ctx->configured = false;

  } while(0);

  return err;
}

void ignition_loop_slow(ignition_ctx_t *ctx)
{
  time_delta_us_t time_debounce;
  input_value_t input_value;
  bool input_bool;
  time_us_t now;
  error_t err;

  if(ctx->ready) {
    if(ctx->configured) {
      now = time_get_current_us();

      input_bool = ctx->data.ignition_active;
      err = input_get_value(ctx->input_signal_pin, &input_value, NULL);
      if(err == E_OK) {
        input_bool = input_value ? true : false;
        input_bool ^= ctx->config.input_gpio_inverted;
      } else if(err != E_AGAIN) {
        ctx->diag.bits.input_signal_error = true;
      }

      if(input_bool != ctx->data.ignition_active) {
        if(input_bool) {
          time_debounce = ctx->config.input_debounce_off_to_on;
        } else {
          time_debounce = ctx->config.input_debounce_on_to_off;
        }

        if(time_diff(now, ctx->status_time) >= time_debounce) {
          ctx->data.ignition_active = input_bool;
          ctx->status_time = now;
          if(ctx->init.signal_update_cb) {
            ctx->init.signal_update_cb(ctx->init.signal_update_usrdata, ctx->data.ignition_active);
          }
        }
      } else {
        ctx->status_time = now;
      }

      if(ctx->data.ignition_active) {
        ctx->output_time = now;
        ctx->min_delay_elapsed = false;
      } else if(!ctx->min_delay_elapsed) {
        if(time_diff(now, ctx->output_time) >= ctx->config.output_shutdown_min_delay) {
          ctx->min_delay_elapsed = true;
        }
      }

      if(ctx->config.components_operating_trigger) {
        ctx->data.components_trigger = ctx->data.components_operating;
        if(ctx->data.components_trigger) {
          ctx->operating_time = now;
          ctx->min_operation_elapsed = false;
        } else if(!ctx->min_operation_elapsed) {
          if(time_diff(now, ctx->operating_time) >= ctx->config.output_shutdown_operation_delay) {
            ctx->min_operation_elapsed = true;
          }
        }
      } else {
        ctx->operating_time = now;
        ctx->min_operation_elapsed = true;
        ctx->data.components_trigger = false;
      }

      if(ctx->config.crankshaft_operating_trigger) {
        ctx->data.crankshaft_trigger = ctx->data.crankshaft_operating;
        if(ctx->data.crankshaft_trigger) {
          ctx->crankshaft_time = now;
          ctx->min_crankshaft_elapsed = false;
        } else if(!ctx->min_crankshaft_elapsed) {
          if(time_diff(now, ctx->crankshaft_time) >= ctx->config.output_shutdown_crankshaft_delay) {
            ctx->min_crankshaft_elapsed = true;
          }
        }
      } else {
        ctx->crankshaft_time = now;
        ctx->min_crankshaft_elapsed = true;
        ctx->data.crankshaft_trigger = false;
      }

      if(ctx->data.ignition_active || !ctx->min_delay_elapsed ||
          ctx->data.components_trigger || !ctx->min_operation_elapsed ||
          ctx->data.crankshaft_trigger || !ctx->min_crankshaft_elapsed) {
        ctx->data.output_active = true;
      } else {
        ctx->data.output_active = false;
      }

      if(ctx->output_active_prev != ctx->data.output_active) {
        if(ctx->output_relay_pin_locked) {
          err = output_set_value(ctx->output_relay_pin, ctx->data.output_active);
          if(err == E_OK) {
            ctx->output_active_prev = ctx->data.output_active;
          } else {
            ctx->diag.bits.output_relay_error = true;
          }
        } else {
          ctx->output_active_prev = ctx->data.output_active;
        }
      }

    }
  }
}

ITCM_FUNC void ignition_ckp_signal_update(ignition_ctx_t *ctx, const ckp_data_t *data, const ckp_diag_t *diag)
{
  do {
    BREAK_IF(ctx == NULL);
    BREAK_IF(ctx->configured == false);

    if(data->validity < CKP_DATA_DETECTED) {
      ctx->data.crankshaft_operating = false;
    } else if(data->validity >= CKP_DATA_SYNCHRONIZED) {
      ctx->data.crankshaft_operating = true;
    }

  } while(0);
}

error_t ignition_is_active(ignition_ctx_t *ctx, bool *ignition_active)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ignition_active == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    *ignition_active = ctx->data.ignition_active;

  } while(0);

  return err;
}

error_t ignition_get_data(ignition_ctx_t *ctx, ignition_data_t *data)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(data == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    *data = ctx->data;

  } while(0);

  return err;
}

error_t ignition_get_diag(ignition_ctx_t *ctx, ignition_diag_t *diag)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(diag == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    *diag = ctx->diag;

  } while(0);

  return err;
}
