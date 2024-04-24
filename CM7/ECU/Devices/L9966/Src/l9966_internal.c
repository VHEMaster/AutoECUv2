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
  uint16_t hword;

  while(true) {
    if(ctx->spi_busy == false) {
      ctx->tx_payload[0] = ctx->init.chip_address & L9966_FRAME_CFG_ADDR_MASK;
      ctx->tx_payload[0] |= L9966_FRAME_READ;
      ctx->tx_payload[0] |= L9966_FRAME_CLK_MON_ON;
      ctx->tx_payload[0] |= reg << L9966_FRAME_REG_SHIFT;
      if(PARITY_ODD_CHECK(ctx->tx_payload[0]) == false) {
        ctx->tx_payload[0] |= L9966_FRAME_PAR_MOSI_INST;
      }

      hword = 0;
      if(PARITY_ODD_CHECK(hword) == false) {
        hword |= L9966_FRAME_PAR_MOSI_DATA;
      }
      ctx->tx_payload[1] = hword;

      ctx->spi_busy = true;
      err = E_AGAIN;
      continue;
    } else {
      err = spi_transmit_and_receive(ctx->init.spi_slave, ctx->tx_payload, ctx->rx_payload, 2);
      if(err == E_OK) {
        ctx->spi_busy = false;
        *data = ctx->rx_payload[1];

        if((ctx->rx_payload[0] & L9966_FRAME_MISO_VERIFY_MASK) != L9966_FRAME_MISO_VERIFY_VALUE ||
            (ctx->rx_payload[0] & L9966_FRAME_MISO_ECHO_REG_MASK) != reg ||
            ctx->rx_payload[1] == 0xFFFF) {
          ctx->bad_responses_cnt++;
          err = E_BADRESP;
          break;
        }

        if((ctx->rx_payload[0] & L9966_FRAME_MISO_TRANS_F)) {
          ctx->bad_frames_reported_cnt++;
        }
      } else if(err == E_BUSY) {
        err = E_AGAIN;
      }
    }
    break;
  }

  return err;
}

error_t l9966_reg_write(l9966_ctx_t *ctx, uint8_t reg, uint16_t data)
{
  error_t err = E_OK;
  uint16_t hword;

  while(true) {
    if(ctx->spi_busy == false) {
      ctx->tx_payload[0] = ctx->init.chip_address & L9966_FRAME_CFG_ADDR_MASK;
      ctx->tx_payload[0] |= L9966_FRAME_WRITE;
      ctx->tx_payload[0] |= L9966_FRAME_CLK_MON_ON;
      ctx->tx_payload[0] |= reg << L9966_FRAME_REG_SHIFT;
      if(PARITY_ODD_CHECK(ctx->tx_payload[0]) == false) {
        ctx->tx_payload[0] |= L9966_FRAME_PAR_MOSI_INST;
      }

      hword = data & ~L9966_FRAME_PAR_MOSI_DATA;
      if(PARITY_ODD_CHECK(hword) == false) {
        hword |= L9966_FRAME_PAR_MOSI_DATA;
      }
      ctx->tx_payload[1] = hword;

      ctx->spi_busy = true;
      err = E_AGAIN;
      continue;
    } else {
      err = spi_transmit_and_receive(ctx->init.spi_slave, ctx->tx_payload, ctx->rx_payload, 2);
      if(err == E_OK) {
        ctx->spi_busy = false;
        data = ctx->rx_payload[1];

        if((ctx->rx_payload[0] & L9966_FRAME_MISO_VERIFY_MASK) != L9966_FRAME_MISO_VERIFY_VALUE ||
            (ctx->rx_payload[0] & L9966_FRAME_MISO_ECHO_REG_MASK) != reg ||
            ctx->rx_payload[1] == 0xFFFF) {
          ctx->bad_responses_cnt++;
          err = E_BADRESP;
          break;
        }

        if((ctx->rx_payload[0] & L9966_FRAME_MISO_TRANS_F)) {
          ctx->bad_frames_reported_cnt++;
        }
      } else if(err == E_BUSY) {
        err = E_AGAIN;
      }
    }
    break;
  }

  return err;
}

error_t l9966_burst_reg_read(l9966_ctx_t *ctx, uint8_t reg, uint16_t *data, uint8_t len)
{
  error_t err = E_OK;
  uint16_t hword;

  while(true) {
    if(ctx->spi_busy == false) {
      ctx->tx_payload[0] = ctx->init.chip_address & L9966_FRAME_CFG_ADDR_MASK;
      ctx->tx_payload[0] |= L9966_FRAME_READ;
      ctx->tx_payload[0] |= L9966_FRAME_CLK_MON_OFF;
      ctx->tx_payload[0] |= reg << L9966_FRAME_REG_SHIFT;
      if(PARITY_ODD_CHECK(ctx->tx_payload[0]) == false) {
        ctx->tx_payload[0] |= L9966_FRAME_PAR_MOSI_INST;
      }

      hword = 0;
      if(PARITY_ODD_CHECK(hword) == false) {
        hword |= L9966_FRAME_PAR_MOSI_DATA;
      }
      for(int i = 0; i < len; i++) {
        ctx->tx_payload[i + 1] = hword;
      }

      ctx->spi_busy = true;
      err = E_AGAIN;
      continue;
    } else {
      err = spi_transmit_and_receive(ctx->init.spi_slave, ctx->tx_payload, ctx->rx_payload, len + 1);
      if(err == E_OK) {
        ctx->spi_busy = false;

        if((ctx->rx_payload[0] & L9966_FRAME_MISO_VERIFY_MASK) != L9966_FRAME_MISO_VERIFY_VALUE ||
            (ctx->rx_payload[0] & L9966_FRAME_MISO_ECHO_REG_MASK) != reg) {
          ctx->bad_responses_cnt++;
          err = E_BADRESP;
          break;
        }

        for(int i = 0; i < len; i++) {
          data[i] = ctx->rx_payload[i + 1];
          if(data[i] == 0xFFFF) {
            err = E_BADRESP;
          }
        }
        if(err != E_OK) {
          ctx->bad_responses_cnt++;
          break;
        }

        if((ctx->rx_payload[0] & L9966_FRAME_MISO_TRANS_F)) {
          ctx->bad_frames_reported_cnt++;
        }
      } else if(err == E_BUSY) {
        err = E_AGAIN;
      }
    }
    break;
  }

  return err;
}

error_t l9966_burst_reg_write(l9966_ctx_t *ctx, uint8_t reg, const uint16_t *data, uint8_t len)
{
  error_t err = E_OK;
  uint16_t hword;

  while(true) {
    if(ctx->spi_busy == false) {
      ctx->tx_payload[0] = ctx->init.chip_address & L9966_FRAME_CFG_ADDR_MASK;
      ctx->tx_payload[0] |= L9966_FRAME_WRITE;
      ctx->tx_payload[0] |= L9966_FRAME_CLK_MON_ON;
      ctx->tx_payload[0] |= reg << L9966_FRAME_REG_SHIFT;
      if(PARITY_ODD_CHECK(ctx->tx_payload[0]) == false) {
        ctx->tx_payload[0] |= L9966_FRAME_PAR_MOSI_INST;
      }

      for(int i = 0; i < len; i++) {
        hword = data[i] & ~L9966_FRAME_PAR_MOSI_DATA;
        if(PARITY_ODD_CHECK(hword) == false) {
          hword |= L9966_FRAME_PAR_MOSI_DATA;
        }
        ctx->tx_payload[i + 1] = hword;
      }


      ctx->spi_busy = true;
      err = E_AGAIN;
      continue;
    } else {
      err = spi_transmit_and_receive(ctx->init.spi_slave, ctx->tx_payload, ctx->rx_payload, 2);
      if(err == E_OK) {
        ctx->spi_busy = false;
        data = ctx->rx_payload[1];

        if((ctx->rx_payload[0] & L9966_FRAME_MISO_VERIFY_MASK) != L9966_FRAME_MISO_VERIFY_VALUE ||
            (ctx->rx_payload[0] & L9966_FRAME_MISO_ECHO_REG_MASK) != reg) {
          ctx->bad_responses_cnt++;
          err = E_BADRESP;
          break;
        }

        for(int i = 0; i < len; i++) {
          if(ctx->rx_payload[i + 1] == 0xFFFF) {
            err = E_BADRESP;
          }
        }
        if(err != E_OK) {
          ctx->bad_responses_cnt++;
          break;
        }

        if((ctx->rx_payload[0] & L9966_FRAME_MISO_TRANS_F)) {
          ctx->bad_frames_reported_cnt++;
        }
      } else if(err == E_BUSY) {
        err = E_AGAIN;
      }
    }
    break;
  }

  return err;
}

error_t l9966_is_int_pin_triggered(l9966_ctx_t *ctx, bool *triggered)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || triggered == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->initialized == false, err = E_NOTRDY);
    BREAK_IF_ACTION(gpio_valid(&ctx->init.int_pin) == false, err = E_INVALACT);

    if(ctx->int_triggered == true) {
      ctx->int_triggered = false;
      *triggered = true;
    } else {
      *triggered = gpio_read(&ctx->init.int_pin) == true;
    }

  } while(0);

  return err;
}
