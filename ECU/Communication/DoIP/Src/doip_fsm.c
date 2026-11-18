/*
 * doip_fsm.c
 *
 *  Created on: Nov 18, 2026
 *      Author: VHEMaster
 */

#include "doip_fsm.h"
#include "doip_private.h"

static uint32_t doip_time_add(uint32_t value, uint32_t delta)
{
  return UINT32_MAX - value < delta ? UINT32_MAX : value + delta;
}

void doip_fsm(doip_ctx_t *ctx)
{
  doip_connection_t *conn;
  doip_connection_t *other;
  time_us_t now = time_now_us();
  uint32_t delta = time_diff(now, ctx->loop_time);
  uint32_t active;
  bool waiting;
  bool duplicate;

  ctx->loop_time = now;
  if(ATOMIC_EXCHANGE_ACQUIRE(ctx->reset_trigger, false)) {
    for(uint32_t i = 0; i < DOIP_CONNECTIONS_COUNT; i++) {
      doip_connection_clear(&ctx->connections[i]);
    }
    ctx->error_code = DOIP_OK;
    return;
  }

  for(uint32_t i = 0; i < DOIP_CONNECTIONS_COUNT; i++) {
    conn = &ctx->connections[i];
    CONTINUE_IF(conn->state == DOIP_STATE_CLOSED);

    conn->inactivity_time = doip_time_add(conn->inactivity_time, delta);
    if(conn->state == DOIP_STATE_CLOSING) {
      conn->close_time = doip_time_add(conn->close_time, delta);
      if(conn->downstream.read == conn->downstream.write || conn->close_time >= ctx->config.receive_timeout) {
        conn->state = DOIP_STATE_CLOSED;
      }
      continue;
    }
    if(conn->rx_pos != 0u || conn->rx_state != DOIP_RX_STATE_HEADER) {
      conn->receive_time = doip_time_add(conn->receive_time, delta);
      if(conn->receive_time >= ctx->config.receive_timeout) {
        doip_error_set(ctx, DOIP_TIMEOUT);
        doip_connection_clear(conn);
        continue;
      }
    }
    if(conn->alive_pending) {
      conn->alive_time = doip_time_add(conn->alive_time, delta);
      if(conn->alive_time >= (conn->alive_sent ? ctx->config.alive_timeout : ctx->config.receive_timeout)) {
        doip_error_set(ctx, DOIP_TIMEOUT);
        doip_connection_clear(conn);
        continue;
      }
    }
    if((conn->state == DOIP_STATE_CONNECTED && conn->inactivity_time >= ctx->config.initial_timeout) ||
        (conn->state == DOIP_STATE_ROUTING_ACTIVE && conn->inactivity_time >= ctx->config.inactivity_timeout)) {
      doip_error_set(ctx, DOIP_TIMEOUT);
      doip_connection_clear(conn);
      continue;
    }
    if(conn->rx_state == DOIP_RX_STATE_HEADER && conn->rx_pos == DOIP_HEADER_LEN) {
      doip_receive_header(ctx, conn);
    }
    if(conn->rx_state == DOIP_RX_STATE_COMPLETE) {
      doip_receive_complete(ctx, conn);
    }
    if(conn->state == DOIP_STATE_ROUTING_PENDING) {
      active = 0u;
      waiting = false;
      duplicate = false;
      for(uint32_t n = 0; n < DOIP_CONNECTIONS_COUNT; n++) {
        other = &ctx->connections[n];
        CONTINUE_IF(other == conn || other->state != DOIP_STATE_ROUTING_ACTIVE);
        active++;
        if(other->source_address == conn->pending_address) {
          duplicate = true;
        }
        if(other->alive_pending && (conn->alive_connection == DOIP_CONNECTION_NONE || conn->alive_connection == n)) {
          waiting = true;
        }
      }
      if(!waiting) {
        doip_routing_response(ctx, conn, conn->pending_address,
            duplicate ? 0x03u : active >= ctx->config.max_connections ? 0x01u : 0x10u);
      }
    }
  }
}
