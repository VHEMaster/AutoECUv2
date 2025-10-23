/*
 * isotp.c
 *
 *  Created on: Oct 7, 2025
 *      Author: VHEMaster
 */

#include "isotp.h"
#include "isotp_fsm.h"
#include "isotp_private.h"

error_t isotp_init(isotp_ctx_t *ctx, const isotp_init_ctx_t *init)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(init == NULL, err = E_PARAM);

    memset(ctx, 0u, sizeof(isotp_ctx_t));
    memcpy(&ctx->init, init, sizeof(isotp_init_ctx_t));

  } while(0);

  return err;
}

error_t isotp_configure(isotp_ctx_t *ctx, const isotp_config_t *config)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(config == NULL, err = E_PARAM);

    memcpy(&ctx->config, config, sizeof(isotp_config_t));

  } while(0);

  return err;
}

error_t isotp_frame_write_upstream(isotp_ctx_t *ctx, const isotp_frame_t *frame)
{
  error_t err = E_OK;
  isotp_frame_fifo_t *frame_fifo;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(frame == NULL, err = E_PARAM);

    frame_fifo = &ctx->frame_fifo_upstream;
    err = isotp_frame_fifo_push(frame_fifo, frame);

  } while(0);

  return err;
}

error_t isotp_frame_read_downstream(isotp_ctx_t *ctx, isotp_frame_t *frame)
{
  error_t err = E_OK;
  isotp_frame_fifo_t *frame_fifo;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(frame == NULL, err = E_PARAM);

    frame_fifo = &ctx->frame_fifo_downstream;
    err = isotp_frame_fifo_pull(frame_fifo, frame);

  } while(0);

  return err;
}

error_t isotp_data_write_downstream(isotp_ctx_t *ctx, const uint8_t *payload, uint16_t length)
{
  error_t err = E_OK;
  isotp_data_t *data;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(payload == NULL || length == 0, err = E_PARAM);

    data = &ctx->data_downstream;
    BREAK_IF_ACTION(data->ready, err = E_AGAIN);

    data->length = length;
    memcpy(data->payload, payload, length);
    data->ready = true;

  } while(0);

  return err;
}

error_t isotp_data_read_upstream(isotp_ctx_t *ctx, uint8_t *payload, uint16_t *length)
{
  error_t err = E_OK;
  isotp_data_t *data;
  uint16_t len;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(payload == NULL || length == NULL, err = E_PARAM);

    data = &ctx->data_upstream;
    BREAK_IF_ACTION(!data->ready, err = E_AGAIN);

    len = data->length;
    BREAK_IF_ACTION(*length < len, err = E_OVERFLOW);

    *length = len;
    memcpy(payload, data->payload, len);
    data->ready = false;


  } while(0);

  return err;
}

error_t isotp_data_get_error(isotp_ctx_t *ctx, isotp_error_code_t *code)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(code == NULL, err = E_PARAM);

    *code = ctx->error_code;


  } while(0);

  return err;
}

error_t isotp_reset(isotp_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);

    ctx->reset_trigger = true;

  } while(0);

  return err;
}

void isotp_loop(isotp_ctx_t *ctx)
{
  do {
    BREAK_IF(ctx == NULL);

    isotp_poll_for_reset(ctx);
    isotp_fsm(ctx);

  } while(0);
}
