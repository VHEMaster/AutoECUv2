/*
 * doip_lwip.c
 *
 *  Created on: Nov 18, 2026
 *      Author: VHEMaster
 */

#include "doip_lwip.h"

#define DOIP_LWIP_TRANSFER_LEN       256u
#define DOIP_LWIP_LOOP_ITERATIONS    8u

static void doip_lwip_connection_release(doip_lwip_connection_t *conn)
{
  if(conn->rx != NULL) {
    pbuf_free(conn->rx);
    conn->rx = NULL;
  }
  conn->rx_pos = 0u;
  conn->unacked = 0u;
  conn->remote_closed = false;

  (void)doip_connection_close(conn->ctx->doip_ctx, conn->connection);
}

static void doip_lwip_connection_abort(doip_lwip_connection_t *conn)
{
  struct tcp_pcb *pcb = conn->pcb;

  conn->pcb = NULL;
  if(pcb != NULL) {
    tcp_arg(pcb, NULL);
    tcp_recv(pcb, NULL);
    tcp_sent(pcb, NULL);
    tcp_err(pcb, NULL);
    tcp_abort(pcb);
  }

  doip_lwip_connection_release(conn);
}

static err_t doip_lwip_receive_cb(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t error)
{
  doip_lwip_connection_t *conn = arg;

  if(conn == NULL || conn->pcb != pcb) {
    if(p != NULL) {
      pbuf_free(p);
    }
    tcp_abort(pcb);
    return ERR_ABRT;
  }
  if(p == NULL) {
    conn->remote_closed = true;
    return ERR_OK;
  }
  if(error != ERR_OK) {
    return error;
  }
  if(conn->rx != NULL) {
    /* lwIP retains refused_data and retries; do not free or acknowledge it. */
    return ERR_MEM;
  }
  conn->rx = p;
  conn->rx_pos = 0u;

  return ERR_OK;
}

static err_t doip_lwip_sent_cb(void *arg, struct tcp_pcb *pcb, uint16_t length)
{
  doip_lwip_connection_t *conn = arg;

  if(conn != NULL && conn->pcb == pcb) {
    conn->unacked -= MIN(conn->unacked, (uint32_t)length);
  }

  return ERR_OK;
}

static void doip_lwip_error_cb(void *arg, err_t error)
{
  doip_lwip_connection_t *conn = arg;

  if(conn != NULL) {
    /* The stack has already freed this PCB. */
    conn->pcb = NULL;
    conn->ctx->error_code = E_IO;
    doip_lwip_connection_release(conn);
  }
  (void)error;
}

static err_t doip_lwip_accept_cb(void *arg, struct tcp_pcb *pcb, err_t error)
{
  doip_lwip_ctx_t *ctx = arg;
  doip_lwip_connection_t *conn;
  uint8_t connection;
  error_t err;

  if(ctx == NULL || error != ERR_OK || !ctx->started) {
    tcp_abort(pcb);
    return ERR_ABRT;
  }
  err = doip_connection_open(ctx->doip_ctx, &connection);
  if(err != E_OK) {
    tcp_abort(pcb);
    return ERR_ABRT;
  }
  conn = &ctx->connections[connection];
  if(conn->pcb != NULL) {
    /* A protocol reset invalidated an old socket before the next adapter poll. */
    doip_lwip_connection_abort(conn);
    err = doip_connection_open(ctx->doip_ctx, &connection);
    if(err != E_OK) {
      tcp_abort(pcb);
      return ERR_ABRT;
    }
    conn = &ctx->connections[connection];
  }
  conn->pcb = pcb;
  conn->generation = ctx->doip_ctx->connections[connection].generation;
  conn->remote_closed = false;
  tcp_arg(pcb, conn);
  tcp_recv(pcb, doip_lwip_receive_cb);
  tcp_sent(pcb, doip_lwip_sent_cb);
  tcp_err(pcb, doip_lwip_error_cb);
  tcp_nagle_disable(pcb);
  tcp_backlog_accepted(pcb);

  return ERR_OK;
}

#if LWIP_UDP
static void doip_lwip_udp_receive_cb(void *arg, struct udp_pcb *pcb, struct pbuf *p,
    const ip_addr_t *address, uint16_t port)
{
  doip_lwip_ctx_t *ctx = arg;
  uint8_t request[25];
  uint8_t response[41];
  uint16_t length = sizeof(response);
  struct pbuf *reply;
  error_t err;

  do {
    BREAK_IF(ctx == NULL || p == NULL || address == NULL);
    BREAK_IF(p->tot_len < DOIP_HEADER_LEN);

    /* Only the first request in a UDP datagram is processed. */
    uint16_t bytes = MIN(p->tot_len, sizeof(request));
    BREAK_IF(pbuf_copy_partial(p, request, bytes, 0u) != bytes);
    err = doip_datagram_process(ctx->doip_ctx, request, bytes, response, &length);
    BREAK_IF(err != E_OK || length == 0u);

    reply = pbuf_alloc(PBUF_TRANSPORT, length, PBUF_RAM);
    BREAK_IF(reply == NULL);
    if(pbuf_take(reply, response, length) == ERR_OK) {
      (void)udp_sendto(pcb, reply, address, port);
    }
    pbuf_free(reply);

  } while(0);

  if(p != NULL) {
    pbuf_free(p);
  }
}

static void doip_lwip_announce(doip_lwip_ctx_t *ctx)
{
  uint8_t payload[41];
  uint16_t length = sizeof(payload);
  struct pbuf *p;
  ip_addr_t address;
  err_t err;

  time_us_t now = time_now_us();

  do {
    BREAK_IF(ctx->doip_ctx->config.anouncement_amount != 0u &&
        ctx->announcement_count >= ctx->doip_ctx->config.anouncement_amount);
    BREAK_IF(ctx->doip_ctx->config.anouncement_period == 0);

    BREAK_IF(time_diff(now, ctx->announcement_time) < ctx->doip_ctx->config.anouncement_period);
    BREAK_IF(doip_vehicle_announcement(ctx->doip_ctx, payload, &length) != E_OK);

    ip_addr_set_ip4_u32(&address, IPADDR_BROADCAST);

    p = pbuf_alloc(PBUF_TRANSPORT, length, PBUF_RAM);
    BREAK_IF(p == NULL);

    err = pbuf_take(p, payload, length);
    if(err == ERR_OK) {
      err = udp_sendto(ctx->udp, p, &address, ctx->port);
    }
    pbuf_free(p);

    if(err == ERR_OK) {
      ctx->announcement_count++;
      ctx->announcement_time = now;
    }

  } while(0);
}
#endif /* LWIP_UDP */

void doip_lwip_init(doip_lwip_ctx_t *ctx, doip_ctx_t *doip_ctx, struct netif *netif)
{
  do {
    BREAK_IF(ctx == NULL);

    memset(ctx, 0u, sizeof(*ctx));
    ctx->doip_ctx = doip_ctx;
    ctx->netif = netif;
    for(uint32_t i = 0; i < DOIP_CONNECTIONS_COUNT; i++) {
      ctx->connections[i].ctx = ctx;
      ctx->connections[i].connection = i;
    }

  } while(0);
}

void doip_lwip_stop(doip_lwip_ctx_t *ctx)
{
  do {
    BREAK_IF(ctx == NULL);

    ctx->started = false;
    for(uint32_t i = 0; i < DOIP_CONNECTIONS_COUNT; i++) {
      if(ctx->connections[i].pcb != NULL || ctx->connections[i].rx != NULL) {
        doip_lwip_connection_abort(&ctx->connections[i]);
      }
    }
    if(ctx->listener != NULL) {
      (void)tcp_close(ctx->listener);
      ctx->listener = NULL;
    }
#if LWIP_UDP
    if(ctx->udp != NULL) {
      udp_remove(ctx->udp);
      ctx->udp = NULL;
    }
#endif /* LWIP_UDP */

  } while(0);
}

static error_t doip_lwip_start(doip_lwip_ctx_t *ctx)
{
  error_t err = E_OK;
  struct tcp_pcb *pcb;
  err_t result;

  do {
    pcb = tcp_new_ip_type(IPADDR_TYPE_V4);
    BREAK_IF_ACTION(pcb == NULL, err = E_AGAIN);

    ip_set_option(pcb, SOF_REUSEADDR);

    tcp_bind_netif(pcb, ctx->netif);

    result = tcp_bind(pcb, IP_ANY_TYPE, ctx->doip_ctx->config.port);
    if(result != ERR_OK) {
      tcp_abort(pcb);
      err = E_IO;
      break;
    }

    ctx->listener = tcp_listen_with_backlog_and_err(pcb, DOIP_CONNECTIONS_COUNT, &result);
    if(ctx->listener == NULL) {
      tcp_abort(pcb);
      err = E_AGAIN;
      break;
    }

    tcp_arg(ctx->listener, ctx);
    tcp_accept(ctx->listener, doip_lwip_accept_cb);

#if LWIP_UDP
    ctx->udp = udp_new_ip_type(IPADDR_TYPE_V4);
    BREAK_IF_ACTION(ctx->udp == NULL, err = E_AGAIN);

    udp_bind_netif(ctx->udp, ctx->netif);

    result = udp_bind(ctx->udp, IP_ANY_TYPE, ctx->doip_ctx->config.port);
    BREAK_IF_ACTION(result != ERR_OK, err = E_IO);

    udp_recv(ctx->udp, doip_lwip_udp_receive_cb, ctx);

#endif /* LWIP_UDP */

    ctx->port = ctx->doip_ctx->config.port;
    ctx->announcement_count = 0u;
    ctx->announcement_time = time_now_us();
    ctx->started = true;

  } while(0);

  if(err != E_OK) {
    doip_lwip_stop(ctx);
  }

  return err;
}

static void doip_lwip_connection_loop(doip_lwip_connection_t *conn)
{
  doip_lwip_ctx_t *ctx = conn->ctx;
  uint8_t payload[DOIP_LWIP_TRANSFER_LEN];
  struct tcp_pcb *pcb;
  uint16_t length;
  uint16_t bytes;
  doip_state_t state;
  error_t err;
  err_t result;

  do {
    BREAK_IF(conn->pcb == NULL);
    if(conn->generation != ctx->doip_ctx->connections[conn->connection].generation) {
      doip_lwip_connection_abort(conn);
      break;
    }
    (void)doip_connection_get_state(ctx->doip_ctx, conn->connection, &state);
    if(state == DOIP_STATE_CLOSED || conn->remote_closed) {
      pcb = conn->pcb;
      tcp_arg(pcb, NULL);
      tcp_recv(pcb, NULL);
      tcp_sent(pcb, NULL);
      tcp_err(pcb, NULL);

      result = tcp_close(pcb);
      if(result == ERR_OK) {
        conn->pcb = NULL;
        doip_lwip_connection_release(conn);
      } else {

        tcp_arg(pcb, conn);
        tcp_recv(pcb, doip_lwip_receive_cb);
        tcp_sent(pcb, doip_lwip_sent_cb);
        tcp_err(pcb, doip_lwip_error_cb);
      }
      break;
    }

    for(uint32_t i = 0; i < DOIP_LWIP_LOOP_ITERATIONS && conn->rx != NULL; i++) {
      length = MIN((uint32_t)(conn->rx->tot_len - conn->rx_pos), sizeof(payload));
      BREAK_IF(pbuf_copy_partial(conn->rx, payload, length, conn->rx_pos) != length);

      bytes = length;
      err = doip_stream_write_upstream(ctx->doip_ctx, conn->connection, payload, &bytes);
      BREAK_IF(err != E_OK || bytes == 0u);

      conn->rx_pos += bytes;
      tcp_recved(conn->pcb, bytes);
      if(conn->rx_pos == conn->rx->tot_len) {
        pbuf_free(conn->rx);
        conn->rx = NULL;
        conn->rx_pos = 0u;
      }
    }

    for(uint32_t i = 0; i < DOIP_LWIP_LOOP_ITERATIONS; i++) {
      length = MIN(tcp_sndbuf(conn->pcb), sizeof(payload));
      BREAK_IF(length == 0u);

      err = doip_stream_peek_downstream(ctx->doip_ctx, conn->connection, payload, &length);
      BREAK_IF(err != E_OK || length == 0u);

      result = tcp_write(conn->pcb, payload, length, TCP_WRITE_FLAG_COPY);
      BREAK_IF(result == ERR_MEM);
      if(result != ERR_OK) {
        ctx->error_code = E_IO;
        doip_lwip_connection_abort(conn);
        break;
      }

      err = doip_stream_consume_downstream(ctx->doip_ctx, conn->connection, length);
      if(err != E_OK) {
        ctx->error_code = E_FAULT;
        doip_lwip_connection_abort(conn);
        break;
      }

      conn->unacked += length;
    }
    if(conn->pcb != NULL) {
      (void)tcp_output(conn->pcb);
    }

  } while(0);
}

void doip_lwip_loop(doip_lwip_ctx_t *ctx, bool link_up)
{
  do {
    BREAK_IF(ctx == NULL || ctx->doip_ctx == NULL || ctx->netif == NULL);

    if(link_up == false || ctx->doip_ctx->configured == false || ATOMIC_LOAD(ctx->doip_ctx->reset_trigger) == true) {
      doip_lwip_stop(ctx);
      ctx->retry_pending = false;
      break;
    }

    if(ctx->started == true && ctx->port != ctx->doip_ctx->config.port) {
      doip_lwip_stop(ctx);
    }

    if(ctx->started == false) {
      BREAK_IF(ctx->retry_pending && time_diff(time_now_us(), ctx->retry_time) < TIME_US_IN_S);

      ctx->error_code = doip_lwip_start(ctx);
      ctx->retry_pending = ctx->error_code != E_OK;
      ctx->retry_time = time_now_us();
      BREAK_IF(ctx->started == false);
    }
    for(uint32_t i = 0; i < DOIP_CONNECTIONS_COUNT; i++) {
      doip_lwip_connection_loop(&ctx->connections[i]);
    }
#if LWIP_UDP
    doip_lwip_announce(ctx);
#endif /* LWIP_UDP */

  } while(0);
}
