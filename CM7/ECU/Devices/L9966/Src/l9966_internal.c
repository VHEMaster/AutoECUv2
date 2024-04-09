/*
 * l9966_internal.c
 *
 *  Created on: Apr 8, 2024
 *      Author: VHEMaster
 */

#include "l9966_internal.h"
#include "compiler.h"

error_t l9966_reg_read(l9966_ctx_t *ctx, uint8_t reg, uint16_t *data)
{
  error_t err = E_NOTSUPPORT;

  return err;
}

error_t l9966_reg_write(l9966_ctx_t *ctx, uint8_t reg, uint16_t data)
{
  error_t err = E_NOTSUPPORT;

  return err;
}

error_t l9966_burst_reg_read(l9966_ctx_t *ctx, uint8_t reg, uint16_t *data, uint8_t len)
{
  error_t err = E_NOTSUPPORT;

  return err;
}

error_t l9966_burst_reg_write(l9966_ctx_t *ctx, uint8_t reg, const uint16_t *data, uint8_t len)
{
  error_t err = E_NOTSUPPORT;

  return err;
}

error_t l9966_is_int_pin_triggered(l9966_ctx_t *ctx, bool *triggered)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || triggered == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->initialized == false, err = E_NOTRDY);
    BREAK_IF_ACTION(gpio_valid(&ctx->init.int_pin) == false, err = E_INVALACT);

    *triggered = gpio_read(&ctx->init.int_pin) == true;

  } while(0);

  return err;
}
