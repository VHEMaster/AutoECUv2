/*
 * doip_types.h
 *
 *  Created on: Nov 18, 2026
 *      Author: VHEMaster
 */

#ifndef COMMUNICATION_DOIP_INC_DOIP_TYPES_H_
#define COMMUNICATION_DOIP_INC_DOIP_TYPES_H_

#include "common.h"
#include "time.h"
#include "versioned_doip.h"

#define DOIP_PAYLOAD_LEN_MAX       1024u
#define DOIP_HEADER_LEN            8u
#define DOIP_ADDRESS_LEN           4u
#define DOIP_FRAME_LEN_MAX         (DOIP_HEADER_LEN + DOIP_ADDRESS_LEN + DOIP_PAYLOAD_LEN_MAX)
#define DOIP_CONNECTIONS_MAX       2u
#define DOIP_CONNECTIONS_COUNT     (DOIP_CONNECTIONS_MAX + 1u)
#define DOIP_FRAME_FIFO_LEN        4u
#define DOIP_PORT                  13400u
#define DOIP_CONNECTION_NONE       255u

typedef enum {
  DOIP_OK = 0,
  DOIP_INVALID_HEADER,
  DOIP_INVALID_LENGTH,
  DOIP_UNKNOWN_PAYLOAD,
  DOIP_INVALID_ADDRESS,
  DOIP_BUFFER_OVERFLOW,
  DOIP_TIMEOUT,
  DOIP_MAX
}doip_error_code_t;

typedef enum {
  DOIP_STATE_CLOSED = 0,
  DOIP_STATE_CONNECTED,
  DOIP_STATE_ROUTING_PENDING,
  DOIP_STATE_ROUTING_ACTIVE,
  DOIP_STATE_CLOSING
}doip_state_t;

typedef enum {
  DOIP_RX_STATE_HEADER = 0,
  DOIP_RX_STATE_PAYLOAD,
  DOIP_RX_STATE_DISCARD,
  DOIP_RX_STATE_COMPLETE
}doip_rx_state_t;

typedef struct doip_ctx_tag doip_ctx_t;

typedef void (*doip_error_callback_t)(doip_ctx_t *ctx, doip_error_code_t code, void *userdata);

typedef struct {
    doip_error_callback_t error_callback;
    void *callback_userdata;
}doip_init_ctx_t;

typedef struct {
    uint8_t connection;
    uint32_t generation;
    uint16_t source_address;
    uint16_t target_address;
}doip_message_t;

typedef struct {
    uint8_t payload[DOIP_FRAME_LEN_MAX];
    uint16_t length;
}doip_frame_t;

typedef struct {
    doip_frame_t buffer[DOIP_FRAME_FIFO_LEN];
    uint32_t read;
    uint32_t write;
    uint16_t pos;
}doip_frame_fifo_t;

typedef struct {
    doip_state_t state;
    doip_rx_state_t rx_state;
    uint32_t generation;
    uint16_t source_address;
    uint16_t pending_address;
    uint8_t version;
    uint8_t alive_connection;
    bool alive_pending;
    bool alive_sent;
    uint32_t inactivity_time;
    uint32_t receive_time;
    uint32_t alive_time;
    uint32_t close_time;

    uint8_t rx_buffer[DOIP_FRAME_LEN_MAX];
    uint32_t rx_pos;
    uint32_t rx_length;
    uint32_t discard_left;
    uint16_t payload_type;

    doip_frame_fifo_t downstream;
    uint8_t upstream_data[DOIP_PAYLOAD_LEN_MAX];
    uint16_t upstream_length;
    doip_message_t upstream_message;
    bool upstream_ready;
}doip_connection_t;

typedef struct doip_ctx_tag {
    doip_config_t config;
    doip_init_ctx_t init;
    bool initialized;
    bool configured;
    bool reset_trigger;
    ATOMIC_LOCK_TYPE lock;

    doip_connection_t connections[DOIP_CONNECTIONS_COUNT];
    uint8_t upstream_next;
    time_us_t loop_time;
    doip_error_code_t error_code;
    uint32_t error_code_counter[DOIP_MAX];
}doip_ctx_t;

#endif /* COMMUNICATION_DOIP_INC_DOIP_TYPES_H_ */
