/*
 * tunerstudio_arbiter.c
 *
 *  Created on: Nov 26, 2026
 *      Author: VHEMaster
 */

#include "tunerstudio_arbiter.h"

void tunerstudio_arbiter_reset(tunerstudio_arbiter_t *ctx, uint32_t generation)
{
  memset(ctx, 0u, sizeof(*ctx));
  ctx->active = TUNERSTUDIO_TRANSPORT_MAX;
  ctx->generation = generation;
}

void tunerstudio_arbiter_sync(tunerstudio_arbiter_t *ctx, uint32_t generation)
{
  if(ctx->generation != generation) {
    tunerstudio_arbiter_reset(ctx, generation);
  }
}

void tunerstudio_arbiter_channel_reset(tunerstudio_arbiter_t *ctx, tunerstudio_transport_t transport)
{
  do {
    BREAK_IF(ctx == NULL || transport >= TUNERSTUDIO_TRANSPORT_MAX);

    ctx->channels[transport].state = TUNERSTUDIO_ARBITER_IDLE;
    ctx->channels[transport].length = 0u;
    ctx->channels[transport].pos = 0u;
    if(ctx->active == transport) {
      ctx->active = TUNERSTUDIO_TRANSPORT_MAX;
    }

  } while(0);
}

bool tunerstudio_arbiter_channel_idle(tunerstudio_arbiter_t *ctx, tunerstudio_transport_t transport)
{
  return ctx != NULL && transport < TUNERSTUDIO_TRANSPORT_MAX &&
      ctx->channels[transport].state == TUNERSTUDIO_ARBITER_IDLE;
}

error_t tunerstudio_arbiter_rx_acquire(tunerstudio_arbiter_t *ctx, tunerstudio_transport_t transport, uint8_t **payload, uint16_t *length)
{
  error_t err = E_OK;
  tunerstudio_arbiter_channel_t *channel;

  do {
    BREAK_IF_ACTION(ctx == NULL || payload == NULL || length == NULL || transport >= TUNERSTUDIO_TRANSPORT_MAX, err = E_PARAM);

    channel = &ctx->channels[transport];
    BREAK_IF_ACTION(channel->state != TUNERSTUDIO_ARBITER_IDLE, err = E_AGAIN);

    *payload = channel->payload;
    *length = sizeof(channel->payload);

  } while(0);

  return err;
}

error_t tunerstudio_arbiter_rx_commit(tunerstudio_arbiter_t *ctx, tunerstudio_transport_t transport, uint16_t length)
{
  error_t err = E_OK;
  tunerstudio_arbiter_channel_t *channel;

  do {
    BREAK_IF_ACTION(ctx == NULL || transport >= TUNERSTUDIO_TRANSPORT_MAX ||
        length == 0u || length > TUNERSTUDIO_ARBITER_TRANSFER_LEN, err = E_PARAM);

    channel = &ctx->channels[transport];
    BREAK_IF_ACTION(channel->state != TUNERSTUDIO_ARBITER_IDLE, err = E_AGAIN);

    channel->length = length;
    channel->pos = 0u;
    channel->state = TUNERSTUDIO_ARBITER_RX_READY;

  } while(0);

  return err;
}

error_t tunerstudio_arbiter_tx_peek(tunerstudio_arbiter_t *ctx, tunerstudio_transport_t transport, const uint8_t **payload, uint16_t *length)
{
  error_t err = E_OK;
  tunerstudio_arbiter_channel_t *channel;

  do {
    BREAK_IF_ACTION(ctx == NULL || payload == NULL || length == NULL || transport >= TUNERSTUDIO_TRANSPORT_MAX, err = E_PARAM);

    channel = &ctx->channels[transport];
    BREAK_IF_ACTION(channel->state != TUNERSTUDIO_ARBITER_TX_READY, err = E_AGAIN);

    *payload = &channel->payload[channel->pos];
    *length = channel->length - channel->pos;

  } while(0);

  return err;
}

error_t tunerstudio_arbiter_tx_consume(tunerstudio_arbiter_t *ctx, tunerstudio_transport_t transport, uint16_t length)
{
  error_t err = E_OK;
  tunerstudio_arbiter_channel_t *channel;

  do {
    BREAK_IF_ACTION(ctx == NULL || transport >= TUNERSTUDIO_TRANSPORT_MAX || length == 0u, err = E_PARAM);

    channel = &ctx->channels[transport];
    BREAK_IF_ACTION(channel->state != TUNERSTUDIO_ARBITER_TX_READY, err = E_AGAIN);
    BREAK_IF_ACTION(length > channel->length - channel->pos, err = E_PARAM);

    channel->pos += length;
    if(channel->pos == channel->length) {
      tunerstudio_arbiter_channel_reset(ctx, transport);
    }

  } while(0);

  return err;
}

error_t tunerstudio_arbiter_read(tunerstudio_arbiter_t *ctx, uint8_t **payload, uint16_t *length)
{
  error_t err = E_AGAIN;
  tunerstudio_transport_t transport;
  tunerstudio_arbiter_channel_t *channel;

  do {
    BREAK_IF_ACTION(ctx == NULL || payload == NULL || length == NULL, err = E_PARAM);
    BREAK_IF(ctx->active != TUNERSTUDIO_TRANSPORT_MAX);

    for(uint32_t i = 0; i < TUNERSTUDIO_TRANSPORT_MAX; i++) {
      transport = (ctx->next + i) % TUNERSTUDIO_TRANSPORT_MAX;
      channel = &ctx->channels[transport];
      if(channel->state == TUNERSTUDIO_ARBITER_RX_READY) {
        ctx->active = transport;
        ctx->next = (transport + 1u) % TUNERSTUDIO_TRANSPORT_MAX;
        *payload = channel->payload;
        *length = channel->length;
        err = E_OK;
        break;
      }
    }

  } while(0);

  return err;
}

error_t tunerstudio_arbiter_write(tunerstudio_arbiter_t *ctx, uint16_t length)
{
  error_t err = E_OK;
  tunerstudio_arbiter_channel_t *channel;

  do {
    BREAK_IF_ACTION(ctx == NULL || length > TUNERSTUDIO_ARBITER_TRANSFER_LEN, err = E_PARAM);
    BREAK_IF_ACTION(ctx->active >= TUNERSTUDIO_TRANSPORT_MAX, err = E_INVALACT);

    channel = &ctx->channels[ctx->active];
    if(length == 0u) {
      tunerstudio_arbiter_channel_reset(ctx, ctx->active);
    } else {
      channel->length = length;
      channel->pos = 0u;
      channel->state = TUNERSTUDIO_ARBITER_TX_READY;
      ctx->active = TUNERSTUDIO_TRANSPORT_MAX;
    }

  } while(0);

  return err;
}
