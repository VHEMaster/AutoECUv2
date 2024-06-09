/*
 * vvt.c
 *
 *  Created on: Jun 9, 2024
 *      Author: VHEMaster
 */

#include "vvt.h"
#include "compiler.h"
#include <string.h>

error_t vvt_init(vvt_ctx_t *ctx, const vvt_init_ctx_t *init_ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || init_ctx == NULL, err = E_PARAM);

    memset(ctx, 0u, sizeof(vvt_ctx_t));
    memcpy(&ctx->init, init_ctx, sizeof(vvt_init_ctx_t));

    math_pid_init(&ctx->pid_position);
    math_pid_init(&ctx->pid_speed);

    ctx->ready = true;

  } while(0);

  return err;
}

error_t vvt_configure(vvt_ctx_t *ctx, const vvt_config_t *config)
{
  error_t err = E_OK;
  ecu_gpio_output_if_pwm_cfg_t pwm_config;

  do {
    BREAK_IF_ACTION(ctx == NULL || config == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    if(ctx->configured != false) {
      err = vvt_reset(ctx);
      BREAK_IF(err != E_OK);
    }

    ctx->configured = false;

    if(&ctx->config != config) {
      memcpy(&ctx->config, config, sizeof(vvt_config_t));
    }

    if(ctx->config.enabled == true) {
      err = ecu_config_gpio_input_get_id(ctx->config.power_voltage_pin, &ctx->power_voltage_pin);
      BREAK_IF(err != E_OK);

      err = ecu_config_gpio_output_lock(ctx->config.output_pwm_pin);
      BREAK_IF(err != E_OK);
      ctx->pwm_pin_locked = true;

      err = ecu_config_gpio_output_set_mode(ctx->config.output_pwm_pin, ECU_GPIO_TYPE_PWM);
      BREAK_IF(err != E_OK);

      pwm_config.frequency = ctx->config.pwm_freq;
      pwm_config.pwm_period = ECU_VVT_PWM_PERIOD;

      err = ecu_config_gpio_output_pwm_configure(ctx->config.output_pwm_pin, &pwm_config);
      BREAK_IF(err != E_OK);

      ctx->configured = true;
    }

  } while(0);

  if(err != E_OK) {
    (void)vvt_reset(ctx);
  }

  return err;
}

error_t vvt_reset(vvt_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    if(ctx->pwm_pin_locked != false) {
      (void)ecu_config_gpio_output_unlock(ctx->config.output_pwm_pin);
      ctx->pwm_pin_locked = false;
    }
    ctx->configured = false;

  } while(0);

  return err;
}

void vvt_loop_slow(vvt_ctx_t *ctx)
{
  error_t err = E_OK;

  if(ctx->ready) {
    if(ctx->configured) {
      (void)err;
    }
  }
}

ITCM_FUNC void vvt_cmp_signal_update(vvt_ctx_t *ctx, const cmp_data_t *data, const cmp_diag_t *diag)
{

  do {
    BREAK_IF(ctx == NULL);
    BREAK_IF(ctx->configured == false);

  } while(0);
}

error_t vvt_get_data(vvt_ctx_t *ctx, vvt_data_t *data)
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

error_t vvt_get_diag(vvt_ctx_t *ctx, vvt_diag_t *diag)
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

error_t vvt_set_enabled(vvt_ctx_t *ctx, bool enabled)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->configured == false, err = E_NOTRDY);

    ctx->data.enabled = enabled;

  } while(0);

  return err;
}

error_t vvt_set_target_position(vvt_ctx_t *ctx, float position)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->configured == false, err = E_NOTRDY);

    ctx->data.pos_target = position;

  } while(0);

  return err;
}

error_t vvt_set_target_dutycycle(vvt_ctx_t *ctx, float dutycycle)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->configured == false, err = E_NOTRDY);

    ctx->data.dutycycle_target = dutycycle;

  } while(0);

  return err;
}
