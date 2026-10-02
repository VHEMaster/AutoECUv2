/*
 * max31855.c
 *
 *  Created on: Mar 27, 2024
 *      Author: VHEMaster
 */

#include <string.h>
#include "max31855.h"
#include "compiler.h"

static void max31855_cplt_cb(spi_slave_t *spi_slave, error_t errorcode)
{
  max31855_ctx_t *ctx = (max31855_ctx_t *)spi_slave->usrdata;
  max31855_payload_t payload;

  if(errorcode == E_OK) {
    payload.dword = __REV16(__REV(ctx->payload_rx));
    ctx->payload.dword = payload.dword;
    if(payload.bits.always_zero_0 == 0u && payload.bits.always_zero_1 == 0u) {
      ctx->data.temperature = ctx->payload.bits.temperature_data * 0.25f;
      ctx->data.reference = ctx->payload.bits.junction_reference * 0.0625f;
      ctx->data.valid = true;
    } else {
      ctx->comm_errorcode = E_BADRESP;
      ctx->data.valid = false;
    }
  } else {
    ctx->comm_errorcode = errorcode;
  }
}

error_t max31855_init(max31855_ctx_t *ctx, spi_slave_t *spi_slave)
{
  error_t err = E_OK;

  do {
    memset(ctx, 0u, sizeof(max31855_ctx_t));

    ctx->spi_slave = spi_slave;
    ctx->spi_slave->usrdata = ctx;
    ctx->time_last = 0u;
    ctx->update_triggered = true;

    err = spi_slave_configure_datasize(spi_slave, 16);
    BREAK_IF(err != E_OK);

    err = spi_slave_configure_mode(spi_slave, MAX31855_SPI_MODE);
    BREAK_IF(err != E_OK);

    err = spi_slave_configure_callback(spi_slave, max31855_cplt_cb);
    BREAK_IF(err != E_OK);

    ctx->initialized = true;
    ctx->configured = false;
  } while(0);


  return err;
}

error_t max31855_configure(max31855_ctx_t *ctx, const max31855_config_t *config)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || config == NULL, err = E_PARAM);

    if(&ctx->config != config) {
      memcpy(&ctx->config, config, sizeof(max31855_config_t));
    }
    ctx->configured = true;

  } while(0);

  return err;
}

error_t max31855_reset(max31855_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);

    ctx->configured = false;

  } while(0);

  return err;
}

ITCM_FUNC  void max31855_loop_slow(max31855_ctx_t *ctx)
{
  time_us_t now;
  error_t err = E_OK;

  now = time_now_us();

  if(ctx->initialized == true && ctx->configured == true) {
    if(ctx->config.enabled) {
      if(ctx->comm_busy == false) {
        if(time_diff(now, ctx->time_last) >= ctx->config.poll_period) {
          err = spi_transmit_and_receive(ctx->spi_slave, &ctx->payload_tx, &ctx->payload_rx, 2);
          if(err == E_AGAIN) {
            ctx->time_last = now;
            ctx->comm_busy = true;
          } else if(err == E_OK) {
            ctx->time_last = now;
            ctx->comm_busy = false;
          } else if(err != E_BUSY) {
            ctx->comm_errorcode = err;
          }
        }
      } else {
        err = spi_sync(ctx->spi_slave);
        if(err == E_OK) {
          ctx->comm_busy = false;
        }
      }
    } else {

    }
  }
}

error_t max31855_trigger_update(max31855_ctx_t *ctx)
{
  error_t err = E_OK;

  ctx->update_triggered = true;

  return err;
}

error_t max31855_get_diag(max31855_ctx_t *ctx, max31855_diag_t *diag)
{
  error_t err = E_OK;
  max31855_diag_t diag_temp;

  if(diag != NULL) {
    diag_temp.byte = 0;
    diag_temp.bits.comm = ctx->comm_errorcode != E_OK;
    diag_temp.bits.oc = ctx->payload.bits.diag_fault_oc;
    diag_temp.bits.scg = ctx->payload.bits.diag_fault_scg;
    diag_temp.bits.scv = ctx->payload.bits.diag_fault_scv;

    *diag = diag_temp;
  }

  return err;
}

error_t max31855_get_data(max31855_ctx_t *ctx, max31855_data_t *data)
{
  error_t err = E_OK;

  if(data != NULL) {
    *data = ctx->data;
  }

  return err;
}

