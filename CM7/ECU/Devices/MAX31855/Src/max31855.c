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
#if MAX31855_SPI_32B_DATASIZE == true
    payload.u.dword = ctx->payload_rx;
#else /* MAX31855_SPI_32B_DATASIZE */
    payload.u.dword = __REV(ctx->payload_rx);
#endif /* MAX31855_SPI_32B_DATASIZE */
    if(payload.u.bits.always_zero_0 == 0u && payload.u.bits.always_zero_1 == 0u) {
      ctx->data.u.dword = payload.u.dword;
      ctx->temperature = ctx->data.u.bits.temperature_data * 0.25f;
      ctx->reference = ctx->data.u.bits.junction_reference * 0.0625f;
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

    err = spi_slave_configure_datasize(spi_slave, MAX31855_SPI_32B_DATASIZE == true ? 32 : 8);
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

void max31855_loop_slow(max31855_ctx_t *ctx)
{
  time_us_t now;
  error_t err = E_OK;

  now = time_get_current_us();

  if(ctx->ready == true) {
    if(ctx->comm_busy == false) {
      if(time_diff(now, ctx->time_last) >= ctx->poll_period) {
        err = spi_transmit_and_receive(ctx->spi_slave, &ctx->payload_tx, &ctx->payload_rx, MAX31855_SPI_32B_DATASIZE == true ? 1 : sizeof(uint32_t));
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

error_t max31855_diagnostics(max31855_ctx_t *ctx, max31855_diag_t *diag)
{
  error_t err = E_OK;

  if(diag != NULL) {
    diag->u.bits.comm = ctx->comm_errorcode != E_OK;
    diag->u.bits.oc = ctx->data.u.bits.diag_fault_oc;
    diag->u.bits.scg = ctx->data.u.bits.diag_fault_scg;
    diag->u.bits.scv = ctx->data.u.bits.diag_fault_scv;
  }

  return err;
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

error_t max31855_get_temperature(max31855_ctx_t *ctx, float *temperature)
{
  error_t err = E_OK;

  if(temperature != NULL) {
    *temperature = ctx->temperature;
  }

  return err;
}

