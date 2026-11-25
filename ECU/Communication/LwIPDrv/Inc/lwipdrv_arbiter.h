/*
 * lwipdrv_arbiter.h
 *
 * Both transports can feed PPP negotiation until link-up.
 */

#ifndef COMMUNICATION_LWIPDRV_INC_LWIPDRV_ARBITER_H_
#define COMMUNICATION_LWIPDRV_INC_LWIPDRV_ARBITER_H_

#include "bool.h"

typedef enum {
  LWIPDRV_TRANSPORT_NONE = 0,
  LWIPDRV_TRANSPORT_RS422,
  LWIPDRV_TRANSPORT_USB,
}lwipdrv_transport_t;

typedef enum {
  LWIPDRV_ARBITER_IDLE = 0,
  LWIPDRV_ARBITER_CONNECTING,
  LWIPDRV_ARBITER_CONNECTED,
  LWIPDRV_ARBITER_DISCONNECTING,
}lwipdrv_arbiter_state_t;

typedef struct {
    lwipdrv_arbiter_state_t state;
    lwipdrv_transport_t last_rx;
    lwipdrv_transport_t owner;
}lwipdrv_arbiter_t;

void lwipdrv_arbiter_reset(lwipdrv_arbiter_t *ctx);
bool lwipdrv_arbiter_rx(lwipdrv_arbiter_t *ctx, lwipdrv_transport_t transport);
lwipdrv_transport_t lwipdrv_arbiter_tx(const lwipdrv_arbiter_t *ctx);
void lwipdrv_arbiter_link_up(lwipdrv_arbiter_t *ctx);
void lwipdrv_arbiter_close(lwipdrv_arbiter_t *ctx);

#endif /* COMMUNICATION_LWIPDRV_INC_LWIPDRV_ARBITER_H_ */
