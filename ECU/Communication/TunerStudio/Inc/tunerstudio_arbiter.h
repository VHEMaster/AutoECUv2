/*
 * tunerstudio_arbiter.h
 *
 *  Created on: Nov 26, 2026
 *      Author: VHEMaster
 */

#ifndef COMMUNICATION_TUNERSTUDIO_INC_TUNERSTUDIO_ARBITER_H_
#define COMMUNICATION_TUNERSTUDIO_INC_TUNERSTUDIO_ARBITER_H_

#include "common.h"

#define TUNERSTUDIO_ARBITER_TRANSFER_LEN    256u

typedef enum {
  TUNERSTUDIO_TRANSPORT_LWIP = 0,
  TUNERSTUDIO_TRANSPORT_RS422,
  TUNERSTUDIO_TRANSPORT_MAX
}tunerstudio_transport_t;

typedef enum {
  TUNERSTUDIO_ARBITER_IDLE = 0,
  TUNERSTUDIO_ARBITER_RX_READY,
  TUNERSTUDIO_ARBITER_TX_READY
}tunerstudio_arbiter_state_t;

typedef struct {
    tunerstudio_arbiter_state_t state;
    uint16_t length;
    uint16_t pos;
    uint8_t payload[TUNERSTUDIO_ARBITER_TRANSFER_LEN];
}tunerstudio_arbiter_channel_t;

typedef struct {
    tunerstudio_arbiter_channel_t channels[TUNERSTUDIO_TRANSPORT_MAX];
    tunerstudio_transport_t active;
    tunerstudio_transport_t next;
    uint32_t generation;
}tunerstudio_arbiter_t;

/* Main-loop context only. The caller serializes configuration changes. */
void tunerstudio_arbiter_reset(tunerstudio_arbiter_t *ctx, uint32_t generation);
void tunerstudio_arbiter_sync(tunerstudio_arbiter_t *ctx, uint32_t generation);
void tunerstudio_arbiter_channel_reset(tunerstudio_arbiter_t *ctx, tunerstudio_transport_t transport);
bool tunerstudio_arbiter_channel_idle(tunerstudio_arbiter_t *ctx, tunerstudio_transport_t transport);

error_t tunerstudio_arbiter_rx_acquire(tunerstudio_arbiter_t *ctx, tunerstudio_transport_t transport, uint8_t **payload, uint16_t *length);
error_t tunerstudio_arbiter_rx_commit(tunerstudio_arbiter_t *ctx, tunerstudio_transport_t transport, uint16_t length);
error_t tunerstudio_arbiter_tx_peek(tunerstudio_arbiter_t *ctx, tunerstudio_transport_t transport, const uint8_t **payload, uint16_t *length);
error_t tunerstudio_arbiter_tx_consume(tunerstudio_arbiter_t *ctx, tunerstudio_transport_t transport, uint16_t length);

/* Protocol-side API: the reply is routed to the selected input implicitly. */
error_t tunerstudio_arbiter_read(tunerstudio_arbiter_t *ctx, uint8_t **payload, uint16_t *length);
error_t tunerstudio_arbiter_write(tunerstudio_arbiter_t *ctx, uint16_t length);

#endif /* COMMUNICATION_TUNERSTUDIO_INC_TUNERSTUDIO_ARBITER_H_ */
