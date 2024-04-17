/*
 * l9966.c
 *
 *  Created on: Apr 6, 2024
 *      Author: VHEMaster
 */

#include <string.h>
#include "l9966.h"
#include "l9966_fsm.h"
#include "compiler.h"

static void l9966_cplt_cb(spi_slave_t *spi_slave, error_t errorcode)
{
  l9966_ctx_t *ctx = (l9966_ctx_t *)spi_slave->usrdata;

  //TODO: is it really needed?
  (void)ctx;
}

error_t l9966_init(l9966_ctx_t *ctx, const l9966_init_ctx_t *init_ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || init_ctx == NULL || init_ctx->spi_slave == NULL, err = E_PARAM);

    memset(ctx, 0u, sizeof(l9966_ctx_t));
    memcpy(&ctx->init, init_ctx, sizeof(l9966_init_ctx_t));
    ctx->init.spi_slave->usrdata = ctx;
    ctx->digital_poll_period = L9966_DEFAULT_DIGITAL_POLL_PERIOD;

    if(gpio_valid(&ctx->init.nrst_pin)) {
      gpio_reset(&ctx->init.nrst_pin);
    }

    if(gpio_valid(&ctx->init.sync_pin)) {
      gpio_reset(&ctx->init.sync_pin);
    }

    err = l9966_reg_init(&ctx->register_map);
    BREAK_IF(err != E_OK);

    err = spi_slave_configure_datasize(ctx->init.spi_slave, 16);
    BREAK_IF(err != E_OK);

    err = spi_slave_configure_mode(ctx->init.spi_slave, L9966_SPI_MODE);
    BREAK_IF(err != E_OK);

    err = spi_slave_configure_callback(ctx->init.spi_slave, l9966_cplt_cb);
    BREAK_IF(err != E_OK);

    ctx->ready = true;

  } while(0);

  return err;
}

void l9966_loop_main(l9966_ctx_t *ctx)
{
  if(ctx->ready) {

  }
}

void l9966_loop_slow(l9966_ctx_t *ctx)
{
  if(ctx->ready) {

  }
}

void l9966_loop_fast(l9966_ctx_t *ctx)
{
  error_t err = E_OK;

  if(ctx->ready) {
    err = l9966_fsm(ctx);
    if(err != E_OK && err != E_AGAIN) {
      //TODO: set error in future
    }
  }
}


error_t l9966_write_config(l9966_ctx_t *ctx, const l9966_config_t *config)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || config == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->initialized == false, err = E_NOTRDY);

    if(ctx->configure_request == false) {
      if(&ctx->config != config) {
        memcpy(&ctx->config, config, sizeof(l9966_config_t));
      }
      ctx->configure_errcode = E_AGAIN;
      ctx->configure_request = true;
    } else if(ctx->configure_errcode != E_AGAIN) {
      err = ctx->configure_errcode;
      ctx->configure_request = false;
    } else {
      err = E_AGAIN;
    }

  } while(0);

  return err;
}

error_t l9966_set_rrx_value(l9966_ctx_t *ctx, uint8_t rrx, float value)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || rrx >= L9966_RRx_COUNT, err = E_PARAM);

    ctx->rrx[rrx] = value;

  } while(0);

  return err;
}

error_t l9966_set_dig_poll_period(l9966_ctx_t *ctx, time_delta_us_t period)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);

    ctx->digital_poll_period = period;

  } while(0);

  return err;
}

error_t l9966_reset(l9966_ctx_t *ctx)
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

error_t l9966_get_version(l9966_ctx_t *ctx, l9966_ctrl_ver_t *ver)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || ver == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->initialized == false, err = E_NOTRDY);
    BREAK_IF_ACTION(ctx->version_valid != true, err = E_AGAIN);

    memcpy(ver, &ctx->version, sizeof(l9966_ctrl_ver_t));
  } while(0);

  return err;
}

error_t l9966_get_status(l9966_ctx_t *ctx, l9966_ctrl_gs_t *status)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || status == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->initialized == false, err = E_NOTRDY);
    BREAK_IF_ACTION(ctx->status_valid != true, err = E_AGAIN);

    memcpy(status, &ctx->status, sizeof(l9966_ctrl_gs_t));
  } while(0);

  return err;
}

error_t l9966_get_inputs(l9966_ctx_t *ctx, l9966_ctrl_dig_inputs_t *dig_inputs)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || dig_inputs == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->digital_inputs_valid != true, err = E_AGAIN);
    BREAK_IF_ACTION(ctx->initialized == false || ctx->configured == false, err = E_NOTRDY);

    memcpy(dig_inputs, &ctx->digital_inputs, sizeof(l9966_ctrl_dig_inputs_t));
  } while(0);

  return err;
}

error_t l9966_start_sqncr(l9966_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->digital_inputs_valid != true, err = E_AGAIN);
    BREAK_IF_ACTION(ctx->initialized == false || ctx->configured == false, err = E_NOTRDY);

    for(int i = 0; i < L9966_EU_COUNT; i++) {
      ctx->eu_enabled[i] = true;
    }

  } while(0);

  return err;
}

error_t l9966_stop_sqncr(l9966_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->digital_inputs_valid != true, err = E_AGAIN);
    BREAK_IF_ACTION(ctx->initialized == false || ctx->configured == false, err = E_NOTRDY);

    for(int i = 0; i < L9966_EU_COUNT; i++) {
      ctx->eu_enabled[i] = false;
    }

  } while(0);

  return err;
}

error_t l9966_get_sqncr_output(l9966_ctx_t *ctx, uint8_t cmd_index, float *sqncr_output)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || sqncr_output == NULL || cmd_index >= L9966_CHANNELS, err = E_PARAM);
    BREAK_IF_ACTION(ctx->initialized == false || ctx->configured == false, err = E_NOTRDY);

    if((ctx->sqncr_cmd_ready_mask & (1 << cmd_index)) == 0) {
      err = E_AGAIN;
    }

    *sqncr_output = ctx->sqncr_cmd_results[cmd_index];

  } while(0);

  return err;
}

error_t l9966_start_sc(l9966_ctx_t *ctx, const l9966_ctrl_sc_conf_t *cfg)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->digital_inputs_valid != true, err = E_AGAIN);
    BREAK_IF_ACTION(ctx->initialized == false || ctx->configured == false, err = E_NOTRDY);
    BREAK_IF_ACTION(ctx->sc_enabled == true, err = E_INVALACT);

    ctx->sc_enabled = true;

  } while(0);

  return err;
}

error_t l9966_poll_sc(l9966_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->initialized == false || ctx->configured == false, err = E_NOTRDY);
    BREAK_IF_ACTION(ctx->sc_enabled == false && ctx->sc_ready == false, err = E_INVALACT);
    BREAK_IF_ACTION(ctx->sc_ready != true, err = E_AGAIN);

  } while(0);

  return err;
}

error_t l9966_get_sc(l9966_ctx_t *ctx, float *sc_output)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || sc_output == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->initialized == false || ctx->configured == false, err = E_NOTRDY);

    if(ctx->sc_ready == true) {
      ctx->sc_ready = false;
    } else {
      err = E_AGAIN;
    }

    *sc_output = ctx->sc_result;

  } while(0);

  return err;
}

void l9966_int_irq_handler(l9966_ctx_t *ctx)
{
  if(ctx != NULL && ctx->initialized == true && ctx->configured == true) {
    ctx->int_triggered = true;
  }
}


