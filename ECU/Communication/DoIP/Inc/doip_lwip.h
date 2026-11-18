/*
 * doip_lwip.h
 *
 *  Created on: Nov 18, 2026
 *      Author: VHEMaster
 */

#ifndef COMMUNICATION_DOIP_INC_DOIP_LWIP_H_
#define COMMUNICATION_DOIP_INC_DOIP_LWIP_H_

#include "doip.h"
#include "lwip/tcp.h"
#include "lwip/udp.h"
#include "lwip/netif.h"

typedef struct doip_lwip_ctx_tag doip_lwip_ctx_t;

typedef struct {
    doip_lwip_ctx_t *ctx;
    struct tcp_pcb *pcb;
    struct pbuf *rx;
    uint16_t rx_pos;
    uint32_t generation;
    uint32_t unacked;
    bool remote_closed;
    uint8_t connection;
}doip_lwip_connection_t;

typedef struct doip_lwip_ctx_tag {
    doip_ctx_t *doip_ctx;
    struct netif *netif;
    struct tcp_pcb *listener;
#if LWIP_UDP
    struct udp_pcb *udp;
#endif /* LWIP_UDP */
    doip_lwip_connection_t connections[DOIP_CONNECTIONS_COUNT];
    uint16_t port;
    bool started;
    uint8_t announcement_count;
    time_us_t announcement_time;
    time_us_t retry_time;
    bool retry_pending;
    error_t error_code;
}doip_lwip_ctx_t;

/* All functions, including stop, must execute in the lwIP main-loop context. */
void doip_lwip_init(doip_lwip_ctx_t *ctx, doip_ctx_t *doip_ctx, struct netif *netif);
void doip_lwip_loop(doip_lwip_ctx_t *ctx, bool link_up);
void doip_lwip_stop(doip_lwip_ctx_t *ctx);

#endif /* COMMUNICATION_DOIP_INC_DOIP_LWIP_H_ */
