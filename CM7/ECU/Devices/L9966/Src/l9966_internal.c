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
  error_t err = E_OK;

  do {
    if(ctx->spi_busy == false) {
      ctx->tx_payload[0] = ctx->init.chip_address & L9966_FRAME_CFG_ADDR_MASK;
      ctx->tx_payload[0] |= L9966_FRAME_READ;
      ctx->tx_payload[0] |= L9966_FRAME_CLK_MON_ON;
      ctx->tx_payload[0] |= reg << L9966_FRAME_REG_SHIFT;
      if(PARITY_ODD_CHECK(ctx->tx_payload[0]) == true) {
        ctx->tx_payload[0] |= L9966_FRAME_PAR_MOSI_INST;
      }
      ctx->tx_payload[1] = 0;
      ctx->spi_busy = true;
      continue;
    } else {
      err = spi_transmit_and_receive(ctx->init.spi_slave, ctx->tx_payload, ctx->rx_payload, 2);
      if(err == E_OK) {
        ctx->spi_busy = false;

        BREAK_IF_ACTION((ctx->rx_payload[0] & L9966_FRAME_MISO_VERIFY_MASK) != L9966_FRAME_MISO_VERIFY_VALUE, err = E_BADRESP);
        BREAK_IF_ACTION((ctx->rx_payload[0] & L9966_FRAME_MISO_ECHO_REG_MASK) != reg, err = E_BADRESP);
        BREAK_IF_ACTION((ctx->rx_payload[0] & L9966_FRAME_MISO_TRANS_F) != 0, err = E_BADRESP);
        BREAK_IF_ACTION(ctx->rx_payload[1] == 0xFFFF, err = E_BADRESP);

        *data = ctx->rx_payload[1];
      }
    }
  } while(0);

  return err;
}

error_t l9966_reg_write(l9966_ctx_t *ctx, uint8_t reg, uint16_t data)
{
  error_t err = E_OK;
  uint16_t hword;

  do {
    if(ctx->spi_busy == false) {
      ctx->tx_payload[0] = ctx->init.chip_address & L9966_FRAME_CFG_ADDR_MASK;
      ctx->tx_payload[0] |= L9966_FRAME_READ;
      ctx->tx_payload[0] |= L9966_FRAME_CLK_MON_ON;
      ctx->tx_payload[0] |= reg << L9966_FRAME_REG_SHIFT;
      if(PARITY_ODD_CHECK(ctx->tx_payload[0]) == true) {
        ctx->tx_payload[0] |= L9966_FRAME_PAR_MOSI_INST;
      }

      hword = data & ~L9966_FRAME_PAR_MOSI_DATA;
      if(PARITY_ODD_CHECK(hword) == true) {
        hword |= L9966_FRAME_PAR_MOSI_DATA;
      }
      ctx->tx_payload[1] = hword;

      ctx->spi_busy = true;
      continue;
    } else {
      err = spi_transmit_and_receive(ctx->init.spi_slave, ctx->tx_payload, ctx->rx_payload, 2);
      if(err == E_OK) {

        BREAK_IF_ACTION((ctx->rx_payload[0] & L9966_FRAME_MISO_VERIFY_MASK) != L9966_FRAME_MISO_VERIFY_VALUE, err = E_BADRESP);
        BREAK_IF_ACTION((ctx->rx_payload[0] & L9966_FRAME_MISO_ECHO_REG_MASK) != reg, err = E_BADRESP);
        BREAK_IF_ACTION((ctx->rx_payload[0] & L9966_FRAME_MISO_TRANS_F) != 0, err = E_BADRESP);
        BREAK_IF_ACTION(ctx->rx_payload[1] == 0xFFFF, err = E_BADRESP);

        data = ctx->rx_payload[1];
        ctx->spi_busy = false;
      }
    }
  } while(0);

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
