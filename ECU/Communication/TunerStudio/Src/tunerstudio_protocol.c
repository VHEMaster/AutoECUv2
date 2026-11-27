/*
 * tunerstudio_protocol.c
 *
 *  Created on: Nov 27, 2026
 *      Author: VHEMaster
 */

#include "tunerstudio_protocol.h"

static error_t tunerstudio_protocol_process(tunerstudio_ctx_t *ctx, uint8_t *payload, uint16_t *length)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || payload == NULL || length == NULL || *length == 0u, err = E_PARAM);

    /* TODO: stream framing, packet validation and command dispatch. */
    /* For now leave the bytes unchanged for the stream echo test. */

  } while(0);

  return err;
}

void tunerstudio_protocol_loop(tunerstudio_ctx_t *ctx)
{
  error_t err;
  uint8_t *payload;
  uint16_t length;

  do {
    BREAK_IF(ctx == NULL || ctx->configured == false);

    for(uint32_t i = 0; i < TUNERSTUDIO_TRANSPORT_MAX; i++) {
      err = tunerstudio_arbiter_read(&ctx->arbiter, &payload, &length);
      BREAK_IF(err != E_OK);

      err = tunerstudio_protocol_process(ctx, payload, &length);
      if(err != E_OK) {
        length = 0u;
      }
      (void)tunerstudio_arbiter_write(&ctx->arbiter, length);
    }

  } while(0);
}
