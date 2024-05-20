/*
 * ckp.c
 *
 *  Created on: May 14, 2024
 *      Author: VHEMaster
 */

#include "ckp.h"
#include "compiler.h"
#include "interpolation.h"
#include <string.h>

error_t ckp_init(ckp_ctx_t *ctx, const ckp_init_ctx_t *init_ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || init_ctx == NULL, err = E_PARAM);

    memset(ctx, 0u, sizeof(ckp_ctx_t));
    memcpy(&ctx->init, init_ctx, sizeof(ckp_init_ctx_t));

    ctx->ready = true;

  } while(0);

  return err;
}

static void ckp_gpio_input_cb(ecu_gpio_input_pin_t pin, ecu_gpio_input_level_t level, void *usrdata)
{
  ckp_ctx_t *ctx = (ckp_ctx_t *)usrdata;
  time_us_t now = time_get_current_us();
  time_delta_us_t delta;

  if(ctx != NULL && ctx->configured != false) {

  }
}

error_t ckp_configure(ckp_ctx_t *ctx, const ckp_config_t *config)
{
  error_t err = E_OK;
  bool support = false;

  do {
    BREAK_IF_ACTION(ctx == NULL || config == NULL, err = E_PARAM);

    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    if(ctx->configured == true) {
      err = ckp_reset(ctx);
      BREAK_IF(err != E_OK);
    }

    ctx->configured = false;

    if(&ctx->config != config) {
      memcpy(&ctx->config, config, sizeof(ckp_config_t));
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
      err = ecu_config_gpio_input_register_callback(ctx->config.input_pin, ckp_gpio_input_cb);
      BREAK_IF(err != E_OK);

      ctx->configured = true;
    }

  } while(0);

  if(err != E_OK) {
    (void)ckp_reset(ctx);
  }

  return err;
}

error_t ckp_reset(ckp_ctx_t *ctx)
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

void ckp_loop_main(ckp_ctx_t *ctx)
{

}

void ckp_loop_slow(ckp_ctx_t *ctx)
{
  time_us_t now = time_get_current_us();

  do {
    BREAK_IF(ctx == NULL);
    if(ctx->configured == true) {
      if(ctx->started == true) {

      } else if(time_diff(now, ctx->startup_time) > ctx->config.boot_time) {
        ctx->started = true;
      }
    } else {
      ctx->startup_time = now;
    }
  } while(0);
}

ITCM_FUNC void ckp_loop_fast(ckp_ctx_t *ctx)
{

}

error_t ckp_get_value(ckp_ctx_t *ctx, ckp_data_t *data)
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

error_t ckp_get_diag(ckp_ctx_t *ctx, ckp_diag_t *diag)
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
