/*
 * tunerstudio_lwip.c
 *
 *  Created on: Nov 26, 2026
 *      Author: VHEMaster
 */

#include "tunerstudio_lwip.h"

static void tunerstudio_lwip_release(tunerstudio_lwip_ctx_t *ctx)
{
  if(ctx->rx != NULL) {
    pbuf_free(ctx->rx);
    ctx->rx = NULL;
  }
  ctx->rx_pos = 0u;
  tunerstudio_arbiter_channel_reset(&ctx->tunerstudio_ctx->arbiter, TUNERSTUDIO_TRANSPORT_LWIP);
  ctx->remote_closed = false;
}

static void tunerstudio_lwip_abort(tunerstudio_lwip_ctx_t *ctx)
{
  struct tcp_pcb *pcb = ctx->pcb;

  ctx->pcb = NULL;

  if(pcb != NULL) {
    tcp_arg(pcb, NULL);
    tcp_recv(pcb, NULL);
    tcp_err(pcb, NULL);
    tcp_abort(pcb);
  }
  tunerstudio_lwip_release(ctx);
}

static err_t tunerstudio_lwip_receive_cb(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t error)
{
  tunerstudio_lwip_ctx_t *ctx = arg;

  if(ctx == NULL || ctx->pcb != pcb) {
    if(p != NULL) {
      pbuf_free(p);
    }
    tcp_abort(pcb);
    return ERR_ABRT;
  }
  if(error != ERR_OK) {
    return error;
  }
  if(p == NULL) {
    ctx->remote_closed = true;
    return ERR_OK;
  }
  if(ctx->rx != NULL) {
    /* lwIP owns refused_data and will retry the callback. */
    return ERR_MEM;
  }
  ctx->rx = p;
  ctx->rx_pos = 0u;

  return ERR_OK;
}

static void tunerstudio_lwip_error_cb(void *arg, err_t error)
{
  tunerstudio_lwip_ctx_t *ctx = arg;

  if(ctx != NULL) {
    /* lwIP has already freed the PCB. */
    ctx->pcb = NULL;
    ctx->error_code = E_IO;
    tunerstudio_lwip_release(ctx);
  }
  (void)error;
}

static err_t tunerstudio_lwip_accept_cb(void *arg, struct tcp_pcb *pcb, err_t error)
{
  tunerstudio_lwip_ctx_t *ctx = arg;
  bool configured;

  if(pcb == NULL) {
    return error;
  }
  if(ctx == NULL || error != ERR_OK || ctx->listener == NULL || ctx->pcb != NULL) {
    tcp_abort(pcb);
    return ERR_ABRT;
  }
  if(!ATOMIC_TRY_LOCK(ctx->tunerstudio_ctx->lock)) {
    tcp_abort(pcb);
    return ERR_ABRT;
  }
  configured = ctx->tunerstudio_ctx->configured && ctx->tunerstudio_ctx->config.lwip_enabled &&
      ctx->generation == ctx->tunerstudio_ctx->generation;
  ATOMIC_RELEASE(ctx->tunerstudio_ctx->lock);
  if(configured == false) {
    tcp_abort(pcb);
    return ERR_ABRT;
  }

  ctx->pcb = pcb;
  ctx->error_code = E_OK;
  tcp_arg(pcb, ctx);
  tcp_recv(pcb, tunerstudio_lwip_receive_cb);
  tcp_err(pcb, tunerstudio_lwip_error_cb);
  tcp_nagle_disable(pcb);
  tcp_backlog_accepted(pcb);

  return ERR_OK;
}

void tunerstudio_lwip_init(tunerstudio_lwip_ctx_t *ctx, tunerstudio_ctx_t *tunerstudio_ctx, struct netif *netif)
{
  do {
    BREAK_IF(ctx == NULL);

    memset(ctx, 0u, sizeof(*ctx));
    ctx->tunerstudio_ctx = tunerstudio_ctx;
    ctx->netif = netif;

  } while(0);
}

void tunerstudio_lwip_stop(tunerstudio_lwip_ctx_t *ctx)
{
  do {
    BREAK_IF(ctx == NULL);

    if(ctx->pcb != NULL || ctx->rx != NULL) {
      tunerstudio_lwip_abort(ctx);
    }
    if(ctx->listener != NULL) {
      (void)tcp_close(ctx->listener);
      ctx->listener = NULL;
    }
    ctx->retry_pending = false;

  } while(0);
}

static error_t tunerstudio_lwip_start(tunerstudio_lwip_ctx_t *ctx)
{
  error_t err = E_OK;
  struct tcp_pcb *pcb;
  err_t result;

  do {
    pcb = tcp_new_ip_type(IPADDR_TYPE_V4);
    BREAK_IF_ACTION(pcb == NULL, err = E_AGAIN);

    ip_set_option(pcb, SOF_REUSEADDR);

    tcp_bind_netif(pcb, ctx->netif);

    result = tcp_bind(pcb, IP_ANY_TYPE, ctx->tunerstudio_ctx->config.port);
    if(result != ERR_OK) {
      tcp_abort(pcb);
      err = E_IO;
      break;
    }

    ctx->listener = tcp_listen_with_backlog_and_err(pcb, 1u, &result);
    if(ctx->listener == NULL) {
      tcp_abort(pcb);
      err = E_AGAIN;
      break;
    }

    tcp_arg(ctx->listener, ctx);
    tcp_accept(ctx->listener, tunerstudio_lwip_accept_cb);

  } while(0);

  if(err != E_OK) {
    tunerstudio_lwip_stop(ctx);
  }

  return err;
}

static void tunerstudio_lwip_connection_loop(tunerstudio_lwip_ctx_t *ctx)
{
  tunerstudio_arbiter_t *arbiter = &ctx->tunerstudio_ctx->arbiter;
  struct tcp_pcb *pcb;
  const uint8_t *tx;
  uint8_t *rx;
  uint16_t length;
  error_t err;
  err_t result;

  do {
    BREAK_IF(ctx->pcb == NULL);

    pcb = ctx->pcb;
    err = tunerstudio_arbiter_tx_peek(arbiter, TUNERSTUDIO_TRANSPORT_LWIP, &tx, &length);
    if(err == E_OK) {
      length = MIN(length, tcp_sndbuf(pcb));
      if(length != 0u) {
        result = tcp_write(pcb, tx, length, TCP_WRITE_FLAG_COPY);
        if(result == ERR_OK) {
          err = tunerstudio_arbiter_tx_consume(arbiter, TUNERSTUDIO_TRANSPORT_LWIP, length);
          if(err != E_OK) {
            ctx->error_code = E_FAULT;
            tunerstudio_lwip_abort(ctx);
            break;
          }
        } else if(result != ERR_MEM) {
          ctx->error_code = E_IO;
          tunerstudio_lwip_abort(ctx);
          break;
        }
      }
    }

    if(ctx->rx != NULL) {
      err = tunerstudio_arbiter_rx_acquire(arbiter, TUNERSTUDIO_TRANSPORT_LWIP, &rx, &length);
      if(err == E_OK) {
        length = MIN(length, ctx->rx->tot_len - ctx->rx_pos);
        if(pbuf_copy_partial(ctx->rx, rx, length, ctx->rx_pos) != length ||
            tunerstudio_arbiter_rx_commit(arbiter, TUNERSTUDIO_TRANSPORT_LWIP, length) != E_OK) {
          ctx->error_code = E_FAULT;
          tunerstudio_lwip_abort(ctx);
          break;
        }

        ctx->rx_pos += length;
        tcp_recved(pcb, length);
        if(ctx->rx_pos == ctx->rx->tot_len) {
          pbuf_free(ctx->rx);
          ctx->rx = NULL;
          ctx->rx_pos = 0u;
        }
      }
    }

    (void)tcp_output(pcb);

    /* Drain both the received pbuf and the arbiter channel before closing. */
    if(ctx->remote_closed && ctx->rx == NULL &&
        tunerstudio_arbiter_channel_idle(arbiter, TUNERSTUDIO_TRANSPORT_LWIP)) {
      tcp_arg(pcb, NULL);
      tcp_recv(pcb, NULL);
      tcp_err(pcb, NULL);

      result = tcp_close(pcb);
      if(result == ERR_OK) {
        ctx->pcb = NULL;
        tunerstudio_lwip_release(ctx);
      } else {

        tcp_arg(pcb, ctx);
        tcp_recv(pcb, tunerstudio_lwip_receive_cb);
        tcp_err(pcb, tunerstudio_lwip_error_cb);
      }
    }

  } while(0);
}

void tunerstudio_lwip_loop(tunerstudio_lwip_ctx_t *ctx, bool link_up)
{
  do {
    BREAK_IF(ctx == NULL || ctx->tunerstudio_ctx == NULL || ctx->netif == NULL);
    BREAK_IF(!ATOMIC_TRY_LOCK(ctx->tunerstudio_ctx->lock));

    tunerstudio_arbiter_sync(&ctx->tunerstudio_ctx->arbiter, ctx->tunerstudio_ctx->generation);

    do {
      if(ctx->generation != ctx->tunerstudio_ctx->generation) {
        tunerstudio_lwip_stop(ctx);
        ctx->generation = ctx->tunerstudio_ctx->generation;
      }

      if(link_up == false || ctx->tunerstudio_ctx->configured == false || ctx->tunerstudio_ctx->config.lwip_enabled == false) {
        tunerstudio_lwip_stop(ctx);
        break;
      }

      if(ctx->listener == NULL) {
        BREAK_IF(ctx->retry_pending && time_diff(time_now_us(), ctx->retry_time) < TIME_US_IN_S);

        ctx->error_code = tunerstudio_lwip_start(ctx);
        ctx->retry_pending = ctx->error_code != E_OK;
        ctx->retry_time = time_now_us();
        BREAK_IF(ctx->listener == NULL);
      }

      tunerstudio_lwip_connection_loop(ctx);

    } while(0);

    ATOMIC_RELEASE(ctx->tunerstudio_ctx->lock);

  } while(0);
}
