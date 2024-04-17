/*
 * l9960_internal.c
 *
 *  Created on: Apr 17, 2024
 *      Author: VHEMaster
 */

#include "l9960_internal.h"
#include "compiler.h"

error_t l9960_serial_operation(l9960_ctx_t *ctx, l9960_request_payload_t request, l9960_response_payload_t *response)
{
  error_t err = E_OK;

  while(true) {
    if(ctx->spi_busy == false) {
      request.bits.parity = 0;
      request.bits.parity = PARITY_ODD_CHECK(request.data) == false;
      ctx->tx_payload = request.data;

      ctx->spi_busy = true;
      err = E_AGAIN;
      continue;
    } else {
      err = spi_transmit_and_receive(ctx->init.spi_slave, &ctx->tx_payload, &ctx->rx_payload, 1);
      if(err == E_OK) {
        ctx->spi_busy = false;

        if(response != NULL) {
          response->data = ctx->rx_payload;
        }
      } else if(err == E_BUSY) {
        err = E_AGAIN;
      }
    }
    break;
  }

  return err;
}

void l9960_set_enabled(l9960_ctx_t *ctx, bool enabled)
{
  if(gpio_valid(&ctx->init.dis_pin)) {
    gpio_write(&ctx->init.dis_pin, !enabled);
  }
  if(gpio_valid(&ctx->init.ndis_pin)) {
    gpio_write(&ctx->init.ndis_pin, enabled);
  }
}
