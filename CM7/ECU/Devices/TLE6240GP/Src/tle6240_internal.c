/*
 * tle6240_internal.c
 *
 *  Created on: Apr 16, 2024
 *      Author: VHEMaster
 */

#include "tle6240_internal.h"

error_t tle6240_serial_operation(tle6240_ctx_t *ctx, uint8_t ctrl, uint8_t data, uint16_t *diag)
{
  error_t err = E_OK;

  while(true) {
    if(ctx->spi_busy == false) {
      ctx->tx_payload = ctrl << 8 | data;

      ctx->spi_busy = true;
      err = E_AGAIN;
      continue;
    } else {
      err = spi_transmit_and_receive(ctx->init.spi_slave, &ctx->tx_payload, &ctx->rx_payload, 1);
      if(err == E_OK) {
        ctx->spi_busy = false;

        if(diag != NULL) {
          *diag = ctx->rx_payload;
        }
      } else if(err == E_BUSY) {
        err = E_AGAIN;
      }
    }
    break;
  }

  return err;
}
