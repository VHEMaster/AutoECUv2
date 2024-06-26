/*
 * indication.c
 *
 *  Created on: Jun 24, 2024
 *      Author: VHEMaster
 */

#include "indication.h"
#include "compiler.h"
#include <string.h>

error_t indication_init(indication_ctx_t *ctx, const indication_init_ctx_t *init_ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || init_ctx == NULL, err = E_PARAM);

    memset(ctx, 0u, sizeof(indication_ctx_t));
    memcpy(&ctx->init, init_ctx, sizeof(indication_init_ctx_t));

    ctx->ready = true;

  } while(0);

  return err;
}

error_t indication_configure(indication_ctx_t *ctx, const indication_config_t *config)
{
  error_t err = E_OK;
  bool valid;
  time_us_t now;

  do {
    BREAK_IF_ACTION(ctx == NULL || config == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    if(ctx->configured != false) {
      err = indication_reset(ctx);
      BREAK_IF(err != E_OK);
    }

    ctx->configured = false;

    if(&ctx->config != config) {
      memcpy(&ctx->config, config, sizeof(indication_config_t));
    }

    if(ctx->config.enabled == true) {
      err = ecu_config_gpio_output_valid(ctx->config.output_pin, &valid);
      BREAK_IF(err != E_OK);

      if(valid) {
        err = ecu_config_gpio_output_lock(ctx->config.output_pin);
        BREAK_IF(err != E_OK);
        ctx->output_pin_locked = true;

        err = ecu_config_gpio_output_set_mode(ctx->config.output_pin, ECU_GPIO_TYPE_DIRECT);
        BREAK_IF(err != E_OK);

        err = ecu_config_gpio_output_get_id(ctx->config.output_pin, &ctx->output_pin);
        BREAK_IF(err != E_OK);
      }

      now = time_now_us();
      ctx->periodic_last = now;
      ctx->configured = true;
    }

  } while(0);

  if(err != E_OK) {
    (void)indication_reset(ctx);
  }

  return err;
}

error_t indication_reset(indication_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    if(ctx->output_pin_locked != false) {
      (void)ecu_config_gpio_output_unlock(ctx->config.output_pin);
      ctx->output_pin_locked = false;
    }
    ctx->configured = false;

  } while(0);

  return err;
}

void indication_loop_slow(indication_ctx_t *ctx)
{
  error_t err = E_OK;
  time_us_t now;
  time_delta_us_t periodic_time;

  if(ctx->ready) {
    if(ctx->configured) {
      now = time_now_us();

      if(ctx->config.trigger_source == INDICATION_CONFIG_TRIGGER_CRANKSHAFT && ctx->data.ckp_triggered) {
        ctx->data.indication_set = true;
      } else {
        ctx->data.indication_set = false;
      }

      if(ctx->data.manual_engaged) {
        if(ctx->data.manual_enabled == INDICATION_ENABLED_ON) {
          ctx->data.indication_set = true;
          ctx->data.periodic_status = true;
          ctx->periodic_last = now;
        } else if(ctx->data.manual_enabled == INDICATION_ENABLED_PERIODIC) {
          if(ctx->data.periodic_status) {
            periodic_time = ctx->data.periodic_on_time;
          } else {
            periodic_time = ctx->data.periodic_off_time;
          }
          if(time_diff(now, ctx->periodic_last) >= periodic_time) {
            ctx->periodic_last = now;
            ctx->data.periodic_status ^= true;
          }
          ctx->data.indication_set = ctx->data.periodic_status;
        } else {
          ctx->data.indication_set = false;
          ctx->data.periodic_status = false;
          ctx->data.periodic_status = true;
          ctx->periodic_last = now;
        }
      }

      if(ctx->data.force_engaged) {
        ctx->data.indication_set = ctx->data.force_enabled;
      }

      if(!ctx->data.ignpower_on && ctx->config.ignpower_off_disables) {
        ctx->data.indication_set = false;
      }

      if(ctx->indication_set_prev != ctx->data.indication_set) {
        if(ctx->output_pin_locked) {
          err = output_set_value(ctx->output_pin, ctx->data.indication_set);
          if(err == E_OK) {
            ctx->indication_set_prev = ctx->data.indication_set;
          } else {
            ctx->diag.bits.output_drive_error = true;
          }
        } else {
          ctx->indication_set_prev = ctx->data.indication_set;
        }
      }
    }
  }
}

ITCM_FUNC void indication_ckp_signal_update(indication_ctx_t *ctx, const ckp_data_t *data, const ckp_diag_t *diag)
{
  do {
    BREAK_IF(ctx == NULL);
    BREAK_IF(ctx->configured == false);

    if(data->validity >= CKP_DATA_DETECTED) {
      ctx->ckp_rpm = data->rpm;
    } else {
      ctx->ckp_rpm = 0;
    }

    if(ctx->ckp_rpm >= ctx->config.ckp_rpm_min && ctx->ckp_rpm < ctx->config.ckp_rpm_max) {
      ctx->data.ckp_triggered = true;
    } else {
      ctx->data.ckp_triggered = false;
    }

  } while(0);
}

void indication_ignpower_update(indication_ctx_t *ctx, bool ignpower_on)
{

  do {
    BREAK_IF(ctx == NULL);
    BREAK_IF(ctx->configured == false);

    ctx->data.ignpower_on = ignpower_on;

  } while(0);
}

error_t indication_get_data(indication_ctx_t *ctx, indication_data_t *data)
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

error_t indication_get_diag(indication_ctx_t *ctx, indication_diag_t *diag)
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

error_t indication_manual_reset(indication_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->configured == false, err = E_NOTRDY);

    ctx->data.manual_engaged = false;
    ctx->data.manual_enabled = false;

  } while(0);

  return err;
}

error_t indication_manual_set(indication_ctx_t *ctx, bool manual_enabled)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->configured == false, err = E_NOTRDY);

    ctx->data.manual_engaged = true;
    ctx->data.manual_enabled = manual_enabled ? INDICATION_ENABLED_ON : INDICATION_ENABLED_OFF;

  } while(0);

  return err;
}

error_t indication_manual_set_periodic(indication_ctx_t *ctx, time_delta_us_t time_on, time_delta_us_t time_off)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->configured == false, err = E_NOTRDY);

    ctx->data.manual_engaged = true;
    ctx->data.manual_enabled = INDICATION_ENABLED_PERIODIC;
    ctx->data.periodic_on_time = time_on;
    ctx->data.periodic_off_time = time_off;

  } while(0);

  return err;
}

error_t indication_force_reset(indication_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->configured == false, err = E_NOTRDY);

    ctx->data.force_engaged = false;
    ctx->data.force_enabled = false;

  } while(0);

  return err;
}

error_t indication_force_set(indication_ctx_t *ctx, bool force_enabled)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->configured == false, err = E_NOTRDY);

    ctx->data.force_engaged = true;
    ctx->data.force_enabled = force_enabled;

  } while(0);

  return err;
}
