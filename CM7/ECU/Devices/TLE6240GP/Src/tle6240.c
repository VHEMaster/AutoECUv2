/*
 * tle6240.c
 *
 *  Created on: Apr 6, 2024
 *      Author: VHEMaster
 */

#include "tle6240.h"
#include "tle6240_fsm.h"
#include "compiler.h"
#include <string.h>

static void tle6240_cplt_cb(spi_slave_t *spi_slave, error_t errorcode)
{
  tle6240_ctx_t *ctx = (tle6240_ctx_t *)spi_slave->usrdata;

  //TODO: is it really needed?
  (void)ctx;
}

error_t tle6240_init(tle6240_ctx_t *ctx, const tle6240_init_ctx_t *init_ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || init_ctx == NULL || init_ctx->spi_slave == NULL, err = E_PARAM);

    memset(ctx, 0u, sizeof(tle6240_ctx_t));
    memcpy(&ctx->init, init_ctx, sizeof(tle6240_init_ctx_t));
    ctx->init.spi_slave->usrdata = ctx;
    ctx->poll_period = TLE6240_DEFAULT_POLL_PERIOD_US;
    ctx->process_fsm = TLE6240_PROCESS_RESET;

    for(int i = 0; i < ITEMSOF(ctx->init.input_pins); i++) {
      if(gpio_valid(&ctx->init.input_pins[i])) {
        gpio_set(&ctx->init.input_pins[i]);
      }
    }

    if(gpio_valid(&ctx->init.reset_pin)) {
      gpio_reset(&ctx->init.reset_pin);
    }

    err = spi_slave_configure_datasize(ctx->init.spi_slave, 16);
    BREAK_IF(err != E_OK);

    err = spi_slave_configure_mode(ctx->init.spi_slave, TLE6240_SPI_MODE);
    BREAK_IF(err != E_OK);

    err = spi_slave_configure_callback(ctx->init.spi_slave, tle6240_cplt_cb);
    BREAK_IF(err != E_OK);

    ctx->ready = true;

  } while(0);

  return err;
}

void tle6240_loop_main(tle6240_ctx_t *ctx)
{

}

void tle6240_loop_slow(tle6240_ctx_t *ctx)
{

}

ITCM_FUNC void tle6240_loop_fast(tle6240_ctx_t *ctx)
{
  error_t err = E_OK;

  if(ctx->ready) {
    err = tle6240_fsm(ctx);
    if(err != E_OK && err != E_AGAIN && err != E_BUSY) {
      //TODO: set error in future
    }
  }
}


error_t tle6240_write_all_masked(tle6240_ctx_t *ctx, uint16_t mask, uint16_t value)
{
  error_t err = E_OK;
  uint32_t prim;
  uint16_t temp_value;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    prim = EnterCritical();
    temp_value = ctx->output_state;
    temp_value &= ~mask;
    temp_value |= value & mask;
    ctx->output_state = temp_value;
    ctx->output_updated = true;
    ExitCritical(prim);

  } while(0);

  return err;
}

error_t tle6240_write_all(tle6240_ctx_t *ctx, uint16_t value)
{
  error_t err = E_OK;
  uint32_t prim;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    prim = EnterCritical();
    ctx->output_state = value;
    ctx->output_updated = true;
    ExitCritical(prim);

  } while(0);

  return err;
}

error_t tle6240_update(tle6240_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    ctx->output_updated = true;

  } while(0);

  return err;
}

error_t tle6240_diagnostics(tle6240_ctx_t *ctx, tle6240_diag_t *diag)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(diag == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    memcpy(diag, &ctx->diag, sizeof(tle6240_diag_t));

  } while(0);

  return err;
}

error_t tle6240_set_poll_period(tle6240_ctx_t *ctx, time_delta_us_t period)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    ctx->poll_period = period;

  } while(0);

  return err;
}

error_t tle6240_ch_write(tle6240_ctx_t *ctx, uint8_t channel, bool value)
{
  error_t err = E_OK;
  uint32_t prim;
  uint16_t temp_value;
  uint16_t ch_bit;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    ch_bit = 1 << channel;

    prim = EnterCritical();
    temp_value = ctx->output_state;
    if(value) {
      temp_value |= ch_bit;
    } else {
      temp_value &= ~ch_bit;
    }
    ctx->output_state = temp_value;
    ctx->output_updated = true;
    ExitCritical(prim);

  } while(0);

  return err;
}

error_t tle6240_ch_read(tle6240_ctx_t *ctx, uint8_t channel, bool *value)
{
  error_t err = E_OK;
  uint16_t temp_value;
  uint16_t ch_bit;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(value == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    ch_bit = 1 << channel;

    temp_value = ctx->output_state;
    temp_value &= ch_bit;
    *value = temp_value ? true : false;

  } while(0);

  return err;
}

bool tle6240_ch_is_serial(tle6240_ctx_t *ctx, uint8_t channel)
{
  bool retval = false;

  if(channel >= 4 && channel < 8) {
    retval = true;
  } else if(channel >= 12 && channel < 16) {
    retval = true;
  }
  return retval;
}

bool tle6240_ch_is_direct(tle6240_ctx_t *ctx, uint8_t channel)
{
  bool retval = false;

  if(channel >= 0 && channel < 4) {
    retval = true;
  } else if(channel >= 8 && channel < 12) {
    retval = true;
  }
  return retval;
}
