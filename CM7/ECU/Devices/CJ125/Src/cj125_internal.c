/*
 * cj125_internal.c
 *
 *  Created on: Apr 19, 2024
 *      Author: VHEMaster
 */

#include "cj125_internal.h"
#include "compiler.h"

error_t cj125_serial_operation(cj125_ctx_t *ctx, cj125_payload_t request, cj125_payload_t *response)
{
  error_t err = E_OK;

  while(true) {
    if(ctx->spi_busy == false) {
      ctx->tx_payload = request.word;

      ctx->spi_busy = true;
      err = E_AGAIN;
      continue;
    } else {
      err = spi_transmit_and_receive(ctx->init.spi_slave, &ctx->tx_payload, &ctx->rx_payload, 1);
      if(err == E_OK) {
        ctx->spi_busy = false;

        if(response != NULL) {
          response->word = ctx->rx_payload;
        }
      } else if(err == E_BUSY) {
        err = E_AGAIN;
      }
    }
    break;
  }

  return err;
}
