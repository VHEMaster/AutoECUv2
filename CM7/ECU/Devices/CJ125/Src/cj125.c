/*
 * cj125.c
 *
 *  Created on: Apr 6, 2024
 *      Author: VHEMaster
 */

#include <string.h>
#include "cj125.h"
#include "cj125_fsm.h"
#include "cj125_heater.h"
#include "cj125_internal.h"
#include "compiler.h"

static void cj125_cplt_cb(spi_slave_t *spi_slave, error_t errorcode)
{
  cj125_ctx_t *ctx = (cj125_ctx_t *)spi_slave->usrdata;

  //TODO: is it really needed?
  (void)ctx;
}

error_t cj125_init(cj125_ctx_t *ctx, const cj125_init_ctx_t *init_ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || init_ctx == NULL || init_ctx->spi_slave == NULL, err = E_PARAM);

    memset(ctx, 0u, sizeof(cj125_ctx_t));
    memcpy(&ctx->init, init_ctx, sizeof(cj125_init_ctx_t));
    ctx->init.spi_slave->usrdata = ctx;
    ctx->config.pid_cb_period = CJ125_PID_CB_DEFAULT_PERIOD_US;

    if(gpio_valid(&ctx->init.nrst_pin)) {
      gpio_reset(&ctx->init.nrst_pin);
    }

    math_pid_init(&ctx->heater_pid);

    err = spi_slave_configure_datasize(ctx->init.spi_slave, 16);
    BREAK_IF(err != E_OK);

    err = spi_slave_configure_mode(ctx->init.spi_slave, CJ125_SPI_MODE);
    BREAK_IF(err != E_OK);

    err = spi_slave_configure_callback(ctx->init.spi_slave, cj125_cplt_cb);
    BREAK_IF(err != E_OK);

    ctx->data.ampfactor = CJ125_AF_17;
    ctx->ampfactor_req_data = ctx->data.ampfactor;

    ctx->ready = true;

  } while(0);

  return err;
}

error_t cj125_heater_init(cj125_ctx_t *ctx, const cj125_heater_t *heater_cfg)
{
  error_t err = E_OK;;
  HAL_StatusTypeDef status;

  do {
    BREAK_IF_ACTION(ctx == NULL || heater_cfg == NULL, err = E_PARAM);
    BREAK_IF_ACTION(heater_cfg->heater_pwm == NULL || heater_cfg->tim_base_freq == 0, err = E_PARAM);

    memcpy(&ctx->heater, heater_cfg, sizeof(cj125_heater_t));

    if(gpio_valid(&ctx->heater.heater_en_pin)) {
      gpio_reset(&ctx->heater.heater_en_pin);
    }
    if(gpio_valid(&ctx->heater.heater_nen_pin)) {
      gpio_set(&ctx->heater.heater_nen_pin);
    }

    ctx->heater.tim_pulse = NULL;
    switch(ctx->heater.heater_pwm_ch) {
      case TIM_CHANNEL_1:
        ctx->heater.tim_pulse = &ctx->heater.heater_pwm->Instance->CCR1;
        break;
      case TIM_CHANNEL_2:
        ctx->heater.tim_pulse = &ctx->heater.heater_pwm->Instance->CCR2;
        break;
      case TIM_CHANNEL_3:
        ctx->heater.tim_pulse = &ctx->heater.heater_pwm->Instance->CCR3;
        break;
      case TIM_CHANNEL_4:
        ctx->heater.tim_pulse = &ctx->heater.heater_pwm->Instance->CCR4;
        break;
      case TIM_CHANNEL_5:
        ctx->heater.tim_pulse = &ctx->heater.heater_pwm->Instance->CCR5;
        break;
      case TIM_CHANNEL_6:
        ctx->heater.tim_pulse = &ctx->heater.heater_pwm->Instance->CCR6;
        break;
      default:
        break;
    }
    BREAK_IF_ACTION(ctx->heater.tim_pulse == NULL, err = E_FAULT);

    *ctx->heater.tim_pulse = 0;

    ctx->heater.tim_prescaler = ctx->heater.tim_base_freq / 1000000;
    ctx->heater.tim_period = 1000000 / CJ125_HEATER_FREQ;
    if(ctx->heater.heater_pwm->Init.Prescaler != (ctx->heater.tim_prescaler - 1) ||
        ctx->heater.heater_pwm->Init.Period != (ctx->heater.tim_period - 1)) {
      ctx->heater.heater_pwm->Init.Prescaler = ctx->heater.tim_prescaler - 1;
      ctx->heater.heater_pwm->Init.Period = ctx->heater.tim_period - 1;
      status = HAL_TIM_Base_Init(ctx->heater.heater_pwm);
      BREAK_IF_ACTION(status != HAL_OK, err = E_HAL);
    }

    status = HAL_TIM_PWM_Start(ctx->heater.heater_pwm, ctx->heater.heater_pwm_ch);
    BREAK_IF_ACTION(status != HAL_OK, err = E_HAL);

    ctx->heater_ready = true;

  } while(0);

  return err;
}

void cj125_loop_main(cj125_ctx_t *ctx)
{

}

void cj125_loop_slow(cj125_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);

    if(ctx->ready && ctx->configured) {
      err = cj125_update_data(ctx, false);
      if(err != E_OK && err != E_AGAIN) {
        //TODO: set error in future
      }
    }

    err = cj125_heater_fsm(ctx);
    if(err != E_OK && err != E_AGAIN) {
      //TODO: set error in future
    }

    err = cj125_update_heater_voltage(ctx);
    if(err != E_OK && err != E_AGAIN) {
      //TODO: set error in future
    }

  } while(0);
}

void cj125_loop_fast(cj125_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);

    if(ctx->ready) {
      err = cj125_fsm(ctx);
    }

  } while(0);

  if(err != E_OK && err != E_AGAIN) {
    //TODO: set error in future
  }
}

error_t cj125_reset(cj125_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    if(ctx->reset_request == false) {
      ctx->reset_errcode = E_AGAIN;
      ctx->reset_request = true;
    } else if(ctx->reset_errcode != E_AGAIN) {
      err = ctx->reset_errcode;
      ctx->reset_request = false;
    } else {
      err = E_AGAIN;
    }

  } while(0);

  return err;
}

error_t cj125_write_config(cj125_ctx_t *ctx, cj125_config_t *config)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || config == NULL, err = E_PARAM);
    BREAK_IF_ACTION(config->curr_to_lambda_relation.items == 0 || config->curr_to_lambda_relation.items >= CJ125_RELATION_ITEMS_MAX, err = E_PARAM);
    BREAK_IF_ACTION(config->res_to_temp_relation.items == 0 || config->res_to_temp_relation.items >= CJ125_RELATION_ITEMS_MAX, err = E_PARAM);
    BREAK_IF_ACTION(config->pump_ref_current >= CJ125_CONFIG_PRC_MAX, err = E_PARAM);
    BREAK_IF_ACTION(config->ampfactor >= CJ125_AF_MAX, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);
    BREAK_IF_ACTION(ctx->initialized == false, err = E_NOTRDY);

    if(ctx->config_request == false) {
      if(&ctx->config != config) {
        memcpy(&ctx->config, config, sizeof(cj125_config_t));
      }
      ctx->config_errcode = E_AGAIN;
      ctx->config_request = true;
    }
    if(ctx->config_errcode != E_AGAIN) {
      err = ctx->config_errcode;
      ctx->config_request = false;
    } else {
      err = E_AGAIN;
    }

  } while(0);

  return err;
}

error_t cj125_set_ampfactor(cj125_ctx_t *ctx, cj125_af_t ampfactor)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);
    BREAK_IF_ACTION(ctx->initialized == false, err = E_NOTRDY);
    BREAK_IF_ACTION(ampfactor >= CJ125_AF_MAX, err = E_PARAM);

    if(ctx->ampfactor_request == false) {
      if(ctx->ampfactor_req_data != ampfactor) {
        ctx->ampfactor_req_data = ampfactor;
        ctx->ampfactor_errcode = E_AGAIN;
        ctx->ampfactor_request = true;
      } else {
        err = E_OK;
      }
    }
    if(ctx->ampfactor_errcode != E_AGAIN) {
      err = ctx->ampfactor_errcode;
      ctx->ampfactor_request = false;
    } else {
      err = E_AGAIN;
    }

  } while(0);

  return err;
}

error_t cj125_update_voltages(cj125_ctx_t *ctx, const cj125_voltages_t *voltages)
{
  error_t err = E_OK;
  time_us_t now;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);

    ctx->data.pwr_voltage = voltages->pwr;
    ctx->data.ref_voltage = voltages->ref;
    ctx->data.ur_voltage = voltages->ur;
    ctx->data.ua_voltage = voltages->ua;
    ctx->voltages_updated = true;

    now = time_get_current_us();
    ctx->voltages_timestamp = now;

  } while(0);

  return err;
}

error_t cj125_get_data(cj125_ctx_t *ctx, cj125_data_t *data)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);
    BREAK_IF_ACTION(ctx->initialized == false, err = E_NOTRDY);
    BREAK_IF_ACTION(ctx->configured == false, err = E_NOTRDY);

    memcpy(data, &ctx->data, sizeof(cj125_data_t));

    BREAK_IF_ACTION(ctx->data_temp_valid && ctx->data_lambda_valid, err = E_AGAIN);

  } while(0);

  return err;
}

error_t cj125_get_version(cj125_ctx_t *ctx, uint8_t *data)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || data == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);
    BREAK_IF_ACTION(ctx->initialized == false, err = E_NOTRDY);

    *data = ctx->regs.ident.data;

  } while(0);

  return err;
}

error_t cj125_get_diag(cj125_ctx_t *ctx, cj125_diag_t *diag)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || diag == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);
    BREAK_IF_ACTION(ctx->diag_valid, err = E_AGAIN);

    memcpy(diag, &ctx->diag, sizeof(cj125_diag_t));

  } while(0);

  return err;
}

error_t cj125_set_heatup(cj125_ctx_t *ctx, cj125_heatup_type_t type)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || type >= CJ125_HEATUP_TYPE_MAX, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    ctx->heatup_type = type;

  } while(0);

  return err;
}
