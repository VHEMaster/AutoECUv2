/*
 * tunerstudio_lwip.h
 *
 *  Created on: Nov 26, 2026
 *      Author: VHEMaster
 */

#ifndef COMMUNICATION_TUNERSTUDIO_INC_TUNERSTUDIO_LWIP_H_
#define COMMUNICATION_TUNERSTUDIO_INC_TUNERSTUDIO_LWIP_H_

#include "tunerstudio.h"
#include "time.h"
#include "lwip/tcp.h"
#include "lwip/netif.h"

typedef struct {
    tunerstudio_ctx_t *tunerstudio_ctx;
    struct netif *netif;
    struct tcp_pcb *listener;
    struct tcp_pcb *pcb;
    struct pbuf *rx;
    uint16_t rx_pos;
    bool remote_closed;
    time_us_t retry_time;
    bool retry_pending;
    uint32_t generation;
    error_t error_code;
}tunerstudio_lwip_ctx_t;

/* All functions, including stop, must execute in the lwIP main-loop context. */
void tunerstudio_lwip_init(tunerstudio_lwip_ctx_t *ctx, tunerstudio_ctx_t *tunerstudio_ctx, struct netif *netif);
void tunerstudio_lwip_loop(tunerstudio_lwip_ctx_t *ctx, bool link_up);
void tunerstudio_lwip_stop(tunerstudio_lwip_ctx_t *ctx);

#endif /* COMMUNICATION_TUNERSTUDIO_INC_TUNERSTUDIO_LWIP_H_ */
