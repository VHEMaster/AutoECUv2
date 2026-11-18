/*
 * doip_private.c
 *
 *  Created on: Nov 18, 2026
 *      Author: VHEMaster
 */

#include "doip_private.h"

uint16_t doip_read_u16(const uint8_t *buffer)
{
  return ((uint16_t)buffer[0] << 8u) | buffer[1];
}

uint32_t doip_read_u32(const uint8_t *buffer)
{
  return ((uint32_t)buffer[0] << 24u) | ((uint32_t)buffer[1] << 16u) |
      ((uint32_t)buffer[2] << 8u) | buffer[3];
}

void doip_write_u16(uint8_t *buffer, uint16_t value)
{
  buffer[0] = value >> 8u;
  buffer[1] = value;
}

void doip_write_u32(uint8_t *buffer, uint32_t value)
{
  buffer[0] = value >> 24u;
  buffer[1] = value >> 16u;
  buffer[2] = value >> 8u;
  buffer[3] = value;
}

void doip_header_write(uint8_t *buffer, uint8_t version, uint16_t type, uint32_t length)
{
  buffer[0] = version;
  buffer[1] = version ^ 0xFFu;
  doip_write_u16(&buffer[2], type);
  doip_write_u32(&buffer[4], length);
}

void doip_error_set(doip_ctx_t *ctx, doip_error_code_t code)
{
  ctx->error_code = code;
  ctx->error_code_counter[code]++;
  if(ctx->init.error_callback != NULL) {
    ctx->init.error_callback(ctx, code, ctx->init.callback_userdata);
  }
}

void doip_connection_clear(doip_connection_t *connection)
{
  uint32_t generation = connection->generation + 1u;

  memset(connection, 0u, sizeof(*connection));
  connection->generation = generation == 0u ? 1u : generation;
  connection->alive_connection = DOIP_CONNECTION_NONE;
}

error_t doip_frame_push(doip_connection_t *connection, uint16_t type, const uint8_t *payload, uint16_t length)
{
  error_t err = E_OK;
  doip_frame_fifo_t *fifo = &connection->downstream;
  doip_frame_t *frame;

  do {
    BREAK_IF_ACTION(length > DOIP_FRAME_LEN_MAX - DOIP_HEADER_LEN, err = E_PARAM);
    BREAK_IF_ACTION(fifo->write - fifo->read >= DOIP_FRAME_FIFO_LEN, err = E_AGAIN);

    frame = &fifo->buffer[fifo->write % DOIP_FRAME_FIFO_LEN];
    doip_header_write(frame->payload, connection->version, type, length);
    if(length != 0u) {
      memcpy(&frame->payload[DOIP_HEADER_LEN], payload, length);
    }
    frame->length = DOIP_HEADER_LEN + length;
    fifo->write++;

  } while(0);

  return err;
}

void doip_receive_header(doip_ctx_t *ctx, doip_connection_t *connection)
{
  uint8_t *buffer = connection->rx_buffer;
  uint32_t length = doip_read_u32(&buffer[4]);
  uint16_t type = doip_read_u16(&buffer[2]);
  uint8_t nack = 0xFFu;
  bool close = false;
  bool active = connection->state == DOIP_STATE_ROUTING_ACTIVE;

  do {
    BREAK_IF(connection->downstream.write - connection->downstream.read >= DOIP_FRAME_FIFO_LEN);

    if((uint8_t)(buffer[0] ^ buffer[1]) != 0xFFu || (buffer[0] != 0x02u && buffer[0] != 0x03u)) {
      nack = 0x00u;
      close = true;
      doip_error_set(ctx, DOIP_INVALID_HEADER);
    } else {
      connection->version = buffer[0];
      if((type == 0x0005u && length != 7u && length != 11u) ||
          (type == 0x0008u && length != 2u) || (type == 0x8001u && length < 5u) ||
          (type == 0x0000u && length != 1u)) {
        if(active || type == 0x0005u || (type == 0x0008u && connection->source_address != 0u)) {
          nack = 0x04u;
          close = true;
        }
        doip_error_set(ctx, DOIP_INVALID_LENGTH);
      } else if(length > DOIP_FRAME_LEN_MAX - DOIP_HEADER_LEN) {
        if(active) {
          nack = 0x02u;
        }
        doip_error_set(ctx, DOIP_INVALID_LENGTH);
      } else if(type != 0x0000u && type != 0x0005u && type != 0x0008u && type != 0x8001u) {
        if(active) {
          nack = 0x01u;
        }
        doip_error_set(ctx, DOIP_UNKNOWN_PAYLOAD);
      } else {
        connection->payload_type = type;
        connection->rx_length = DOIP_HEADER_LEN + length;
        connection->rx_state = DOIP_RX_STATE_PAYLOAD;
        break;
      }
    }
    if(nack != 0xFFu) {
      (void)doip_frame_push(connection, 0x0000u, &nack, 1u);
    }
    if(close) {
      connection->state = DOIP_STATE_CLOSING;
      connection->close_time = 0u;
    } else {
      connection->discard_left = length;
      connection->rx_state = length == 0u ? DOIP_RX_STATE_HEADER : DOIP_RX_STATE_DISCARD;
      connection->rx_pos = 0u;
    }

  } while(0);
}

void doip_routing_response(doip_ctx_t *ctx, doip_connection_t *connection, uint16_t address, uint8_t code)
{
  uint8_t payload[9] = {0};

  doip_write_u16(payload, address);
  doip_write_u16(&payload[2], ctx->config.logical_address);
  payload[4] = code;
  if(doip_frame_push(connection, 0x0006u, payload, sizeof(payload)) == E_OK) {
    if(code == 0x10u) {
      connection->source_address = address;
      connection->state = DOIP_STATE_ROUTING_ACTIVE;
      connection->inactivity_time = 0u;
    } else {
      connection->state = DOIP_STATE_CLOSING;
      connection->close_time = 0u;
    }
  }
}

static void doip_routing_activate(doip_ctx_t *ctx, doip_connection_t *connection, const uint8_t *payload)
{
  uint16_t address = doip_read_u16(payload);
  uint8_t code = 0x10u;
  uint32_t active = 0u;
  uint8_t duplicate = DOIP_CONNECTION_NONE;
  doip_connection_t *other;

  if(address < ctx->config.tester_address_start || address > ctx->config.tester_address_end) {
    code = 0x00u;
  } else if(connection->source_address != 0u && connection->source_address != address) {
    code = 0x02u;
  } else if(payload[2] != 0x00u) {
    code = 0x06u;
  }
  if(code == 0x10u) {
    for(uint32_t i = 0; i < DOIP_CONNECTIONS_COUNT; i++) {
      other = &ctx->connections[i];
      CONTINUE_IF(other == connection || other->state != DOIP_STATE_ROUTING_ACTIVE);
      active++;
      if(other->source_address == address) {
        duplicate = i;
      }
    }
    if(duplicate != DOIP_CONNECTION_NONE || active >= ctx->config.max_connections) {
      /* Reserve all control queue entries before changing any connection. */
      for(uint32_t i = 0; i < DOIP_CONNECTIONS_COUNT; i++) {
        other = &ctx->connections[i];
        CONTINUE_IF(other == connection || other->state != DOIP_STATE_ROUTING_ACTIVE);
        CONTINUE_IF(duplicate != DOIP_CONNECTION_NONE && duplicate != i);
        if(other->downstream.write - other->downstream.read >= DOIP_FRAME_FIFO_LEN) {
          return;
        }
      }
      connection->pending_address = address;
      connection->alive_connection = duplicate;
      connection->state = DOIP_STATE_ROUTING_PENDING;
      for(uint32_t i = 0; i < DOIP_CONNECTIONS_COUNT; i++) {
        other = &ctx->connections[i];
        CONTINUE_IF(other == connection || other->state != DOIP_STATE_ROUTING_ACTIVE);
        CONTINUE_IF(duplicate != DOIP_CONNECTION_NONE && duplicate != i);
        if(!other->alive_pending) {
          (void)doip_frame_push(other, 0x0007u, NULL, 0u);
          other->alive_pending = true;
          other->alive_sent = false;
          other->alive_time = 0u;
        }
      }
      return;
    }
  }
  doip_routing_response(ctx, connection, address, code);
}

void doip_receive_complete(doip_ctx_t *ctx, doip_connection_t *connection)
{
  const uint8_t *payload = &connection->rx_buffer[DOIP_HEADER_LEN];
  uint16_t length = connection->rx_length - DOIP_HEADER_LEN;
  uint16_t source;
  uint16_t target;
  uint8_t ack[5];
  uint8_t code = 0u;
  bool close = false;

  do {
    BREAK_IF(connection->state == DOIP_STATE_CLOSING || connection->state == DOIP_STATE_CLOSED);
    BREAK_IF(connection->downstream.write - connection->downstream.read >= DOIP_FRAME_FIFO_LEN);

    if(connection->payload_type == 0x0005u) {
      if(connection->state == DOIP_STATE_ROUTING_PENDING) {
        /* The first activation is still being resolved. */
        break;
      }
      doip_routing_activate(ctx, connection, payload);
      /* A full queue on a different socket can defer activation. */
      BREAK_IF(connection->state == DOIP_STATE_CONNECTED);
    } else if(connection->payload_type == 0x0008u && connection->source_address != 0u) {
      if(doip_read_u16(payload) != connection->source_address) {
        connection->state = DOIP_STATE_CLOSING;
        doip_error_set(ctx, DOIP_INVALID_ADDRESS);
      } else {
        connection->alive_pending = false;
      }
    } else if(connection->payload_type == 0x8001u && connection->state == DOIP_STATE_ROUTING_ACTIVE) {
      source = doip_read_u16(payload);
      target = doip_read_u16(&payload[2]);
      if(source != connection->source_address) {
        code = 0x02u;
        close = true;
      } else if(target != ctx->config.logical_address &&
          (ctx->config.functional_address == 0u || target != ctx->config.functional_address)) {
        code = 0x03u;
      } else if(connection->upstream_ready) {
        code = 0x05u;
      }
      doip_write_u16(ack, target);
      doip_write_u16(&ack[2], source);
      ack[4] = code;
      (void)doip_frame_push(connection, code == 0u ? 0x8002u : 0x8003u, ack, sizeof(ack));
      if(code == 0u) {
        connection->upstream_message.connection = connection - ctx->connections;
        connection->upstream_message.generation = connection->generation;
        connection->upstream_message.source_address = source;
        connection->upstream_message.target_address = target;
        connection->upstream_length = length - DOIP_ADDRESS_LEN;
        memcpy(connection->upstream_data, &payload[4], connection->upstream_length);
        connection->upstream_ready = true;
      } else {
        doip_error_set(ctx, code == 0x05u ? DOIP_BUFFER_OVERFLOW : DOIP_INVALID_ADDRESS);
      }
      if(close) {
        connection->state = DOIP_STATE_CLOSING;
        connection->close_time = 0u;
      }
    }
    connection->inactivity_time = 0u;
    connection->receive_time = 0u;
    connection->rx_pos = 0u;
    connection->rx_state = DOIP_RX_STATE_HEADER;

  } while(0);
}

uint16_t doip_identification_write(doip_ctx_t *ctx, uint8_t version, uint8_t *buffer)
{
  doip_header_write(buffer, version, 0x0004u, 33u);
  memcpy(&buffer[8], ctx->config.vin, 17u);
  doip_write_u16(&buffer[25], ctx->config.logical_address);
  memcpy(&buffer[27], ctx->config.eid, 6u);
  memcpy(&buffer[33], ctx->config.gid, 6u);
  buffer[39] = 0u;
  buffer[40] = 0u;

  return 41u;
}
