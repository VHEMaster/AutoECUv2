/*
 * l9966.c
 *
 *  Created on: Apr 6, 2024
 *      Author: VHEMaster
 */

#include <string.h>
#include "l9966.h"

static void l9966_cplt_cb(spi_slave_t *spi_slave, error_t errorcode)
{
  l9966_ctx_t *ctx = (l9966_ctx_t *)spi_slave->usrdata;

}

error_t l9966_init(l9966_ctx_t *ctx, const l9966_init_ctx_t *init_ctx)
{
  error_t err = E_OK;

  memset(ctx, 0u, sizeof(l9966_ctx_t));
  memcpy(&ctx->init, init_ctx, sizeof(l9966_init_ctx_t));
  ctx->init.spi_slave->usrdata = ctx;
  ctx->digital_poll_period = L9966_DEFAULT_DIGITAL_POLL_PERIOD;

  err |= spi_slave_configure_datasize(ctx->init.spi_slave, 16);
  err |= spi_slave_configure_mode(ctx->init.spi_slave, L9966_SPI_MODE);
  err |= spi_slave_configure_callback(ctx->init.spi_slave, l9966_cplt_cb);

  return err;
}

void l9966_loop_main(l9966_ctx_t *ctx)
{
  if(ctx->initialized) {

  }
}

void l9966_loop_slow(l9966_ctx_t *ctx)
{
  if(ctx->initialized) {

  }
}

void l9966_loop_fast(l9966_ctx_t *ctx)
{
  if(ctx->initialized) {

  }
}


error_t l9966_write_config(l9966_ctx_t *ctx, const l9966_config_t *config)
{
  error_t err = E_NOTSUPPORT;

  return err;
}

error_t l9966_set_rrx_value(l9966_ctx_t *ctx, uint8_t rrx, float value)
{
  error_t err = E_NOTSUPPORT;

  return err;
}

error_t l9966_set_dig_poll_period(l9966_ctx_t *ctx, time_delta_us_t period)
{
  error_t err = E_NOTSUPPORT;

  return err;
}

error_t l9966_reset(l9966_ctx_t *ctx)
{
  error_t err = E_NOTSUPPORT;

  return err;
}

error_t l9966_read_version(l9966_ctx_t *ctx, l9966_ctrl_ver_t *ver)
{
  error_t err = E_NOTSUPPORT;

  return err;
}

error_t l9966_read_status(l9966_ctx_t *ctx, l9966_ctrl_gs_t *status)
{
  error_t err = E_NOTSUPPORT;

  return err;
}

error_t l9966_read_inputs(l9966_ctx_t *ctx, l9966_ctrl_dig_inputs_t *dig_inputs)
{
  error_t err = E_NOTSUPPORT;

  return err;
}


error_t l9966_start_sqncr(l9966_ctx_t *ctx)
{
  error_t err = E_NOTSUPPORT;

  return err;
}

error_t l9966_read_sqncr_output(l9966_ctx_t *ctx, uint8_t cmd_index, float *sqncr_output)
{
  error_t err = E_NOTSUPPORT;

  return err;
}

error_t l9966_start_sc(l9966_ctx_t *ctx, const l9966_ctrl_sc_conf_t *cfg)
{
  error_t err = E_NOTSUPPORT;

  return err;
}

error_t l9966_poll_sc(l9966_ctx_t *ctx)
{
  error_t err = E_NOTSUPPORT;

  return err;
}

error_t l9966_read_sc(l9966_ctx_t *ctx, float *sc_output)
{
  error_t err = E_NOTSUPPORT;

  return err;
}

void l9966_int_irq_handler(l9966_ctx_t *ctx)
{

}


