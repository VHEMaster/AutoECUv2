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
    payload.dword = __REV(ctx->payload_rx);
    ctx->data.dword = payload.dword;
    if(payload.bits.always_zero_0 == 0u && payload.bits.always_zero_1 == 0u) {
      ctx->temperature = ctx->data.bits.temperature_data * 0.25f;
      ctx->reference = ctx->data.bits.junction_reference * 0.0625f;
    } else {
      ctx->comm_errorcode = E_BADRESP;
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
    ctx->poll_period = MAX31855_DEFAULT_POLL_PERIOD_US;

    err = spi_slave_configure_datasize(spi_slave, 8);
    BREAK_IF(err != E_OK);

    err = spi_slave_configure_mode(spi_slave, MAX31855_SPI_MODE);
    BREAK_IF(err != E_OK);

    err = spi_slave_configure_callback(spi_slave, max31855_cplt_cb);
    BREAK_IF(err != E_OK);

    ctx->ready = true;
  } while(0);


  return err;
}

void max31855_loop_main(max31855_ctx_t *ctx)
{

}

ITCM_FUNC  void max31855_loop_slow(max31855_ctx_t *ctx)
{
  time_us_t now;
  error_t err = E_OK;

  now = time_get_current_us();

  if(ctx->ready == true) {
    if(ctx->comm_busy == false) {
      if(time_diff(now, ctx->time_last) >= ctx->poll_period) {
        err = spi_transmit_and_receive(ctx->spi_slave, &ctx->payload_tx, &ctx->payload_rx, sizeof(uint32_t));
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
  }
}

void max31855_loop_fast(max31855_ctx_t *ctx)
{

}

error_t max31855_set_poll_period(max31855_ctx_t *ctx, time_delta_us_t period)
{
  error_t err = E_OK;

  ctx->poll_period = period;

  return err;
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
    diag_temp.bits.oc = ctx->data.bits.diag_fault_oc;
    diag_temp.bits.scg = ctx->data.bits.diag_fault_scg;
    diag_temp.bits.scv = ctx->data.bits.diag_fault_scv;

    *diag = diag_temp;
  }

  return err;
}

error_t max31855_get_data(max31855_ctx_t *ctx, max31855_data_t *data)
{
  error_t err = E_OK;
  max31855_data_t data_temp;

  if(data != NULL) {
    data_temp.temperature = ctx->temperature;
    data_temp.reference = ctx->reference;

    *data = data_temp;
  }

  return err;
}

