/*
 * lwipdrv_arbiter.c
 */

#include "lwipdrv_arbiter.h"

void lwipdrv_arbiter_reset(lwipdrv_arbiter_t *ctx)
{
  ctx->state = LWIPDRV_ARBITER_IDLE;
  ctx->last_rx = LWIPDRV_TRANSPORT_NONE;
  ctx->owner = LWIPDRV_TRANSPORT_NONE;
}

bool lwipdrv_arbiter_rx(lwipdrv_arbiter_t *ctx, lwipdrv_transport_t transport)
{
  if(transport != LWIPDRV_TRANSPORT_RS422 && transport != LWIPDRV_TRANSPORT_USB) {
    return false;
  }

  switch(ctx->state) {
    case LWIPDRV_ARBITER_IDLE:
    case LWIPDRV_ARBITER_CONNECTING:
      ctx->last_rx = transport;
      ctx->state = LWIPDRV_ARBITER_CONNECTING;
      return true;
    case LWIPDRV_ARBITER_CONNECTED:
      return ctx->owner == transport;
    case LWIPDRV_ARBITER_DISCONNECTING:
      return lwipdrv_arbiter_tx(ctx) == transport;
    default:
      return false;
  }
}

lwipdrv_transport_t lwipdrv_arbiter_tx(const lwipdrv_arbiter_t *ctx)
{
  return ctx->owner != LWIPDRV_TRANSPORT_NONE ? ctx->owner : ctx->last_rx;
}

void lwipdrv_arbiter_link_up(lwipdrv_arbiter_t *ctx)
{
  if(ctx->state == LWIPDRV_ARBITER_CONNECTING && ctx->last_rx != LWIPDRV_TRANSPORT_NONE) {
    ctx->owner = ctx->last_rx;
    ctx->state = LWIPDRV_ARBITER_CONNECTED;
  }
}

void lwipdrv_arbiter_close(lwipdrv_arbiter_t *ctx)
{
  ctx->state = LWIPDRV_ARBITER_DISCONNECTING;
}
