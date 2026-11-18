/*
 * doip.c
 *
 *  Created on: Nov 18, 2026
 *      Author: VHEMaster
 */

#include "doip.h"
#include "doip_private.h"
#include "doip_fsm.h"

error_t doip_init(doip_ctx_t *ctx, const doip_init_ctx_t *init)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || init == NULL, err = E_PARAM);

    memset(ctx, 0u, sizeof(doip_ctx_t));
    memcpy(&ctx->init, init, sizeof(doip_init_ctx_t));
    ctx->initialized = true;
    ctx->loop_time = time_now_us();

  } while(0);

  return err;
}

error_t doip_configure(doip_ctx_t *ctx, const doip_config_t *config)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || config == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->initialized == false, err = E_INVALACT);
    BREAK_IF_ACTION(config->protocol_version != 0x02u && config->protocol_version != 0x03u, err = E_PARAM);
    BREAK_IF_ACTION(config->port == 0u || config->logical_address == 0u || config->logical_address > 0x7FFFu, err = E_PARAM);
    BREAK_IF_ACTION(config->logical_address >= 0x0E00u && config->logical_address <= 0x0FFFu, err = E_PARAM);
    BREAK_IF_ACTION(config->functional_address != 0u && (config->functional_address < 0xE400u || config->functional_address > 0xE7FFu), err = E_PARAM);
    BREAK_IF_ACTION(config->tester_address_start < 0x0E00u || config->tester_address_end > 0x0FFFu ||
        config->tester_address_start > config->tester_address_end, err = E_PARAM);
    BREAK_IF_ACTION(config->max_connections == 0u || config->max_connections > DOIP_CONNECTIONS_MAX, err = E_PARAM);
    BREAK_IF_ACTION(config->initial_timeout == 0u || config->inactivity_timeout == 0u ||
        config->alive_timeout == 0u || config->receive_timeout == 0u, err = E_PARAM);
    BREAK_IF_ACTION(!ATOMIC_TRY_LOCK(ctx->lock), err = E_AGAIN);

    ctx->configured = false;
    for(uint32_t i = 0; i < DOIP_CONNECTIONS_COUNT; i++) {
      doip_connection_clear(&ctx->connections[i]);
    }
    memcpy(&ctx->config, config, sizeof(doip_config_t));
    ctx->reset_trigger = false;
    ctx->error_code = DOIP_OK;
    ctx->loop_time = time_now_us();
    ctx->configured = ctx->config.enabled;
    ATOMIC_RELEASE(ctx->lock);

  } while(0);

  return err;
}

error_t doip_reset(doip_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->initialized == false, err = E_INVALACT);

    ATOMIC_STORE(ctx->reset_trigger, true);

  } while(0);

  return err;
}

error_t doip_data_get_error(doip_ctx_t *ctx, doip_error_code_t *code)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || code == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->initialized == false, err = E_INVALACT);

    *code = ctx->error_code;

  } while(0);

  return err;
}

void doip_loop(doip_ctx_t *ctx)
{
  do {
    BREAK_IF(ctx == NULL || ctx->configured == false);
    BREAK_IF(!ATOMIC_TRY_LOCK(ctx->lock));

    doip_fsm(ctx);
    ATOMIC_RELEASE(ctx->lock);

  } while(0);
}

error_t doip_connection_open(doip_ctx_t *ctx, uint8_t *connection)
{
  error_t err = E_OVERFLOW;

  do {
    BREAK_IF_ACTION(ctx == NULL || connection == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->configured == false || ATOMIC_LOAD(ctx->reset_trigger), err = E_INVALACT);

    BREAK_IF_ACTION(!ATOMIC_TRY_LOCK(ctx->lock), err = E_AGAIN);

    /* One extra socket permits duplicate-address alive checking at capacity. */
    for(uint32_t i = 0; i <= ctx->config.max_connections; i++) {
      if(ctx->connections[i].state == DOIP_STATE_CLOSED) {
        doip_connection_clear(&ctx->connections[i]);
        ctx->connections[i].version = ctx->config.protocol_version;
        ctx->connections[i].state = DOIP_STATE_CONNECTED;
        *connection = i;
        err = E_OK;
        break;
      }
    }
    ATOMIC_RELEASE(ctx->lock);

  } while(0);

  return err;
}

error_t doip_connection_close(doip_ctx_t *ctx, uint8_t connection)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || connection >= DOIP_CONNECTIONS_COUNT, err = E_PARAM);
    BREAK_IF_ACTION(!ATOMIC_TRY_LOCK(ctx->lock), err = E_AGAIN);

    doip_connection_clear(&ctx->connections[connection]);
    ATOMIC_RELEASE(ctx->lock);

  } while(0);

  return err;
}

error_t doip_connection_get_state(doip_ctx_t *ctx, uint8_t connection, doip_state_t *state)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || state == NULL || connection >= DOIP_CONNECTIONS_COUNT, err = E_PARAM);

    *state = ctx->connections[connection].state;

  } while(0);

  return err;
}

error_t doip_stream_write_upstream(doip_ctx_t *ctx, uint8_t connection, const uint8_t *payload, uint16_t *length)
{
  error_t err = E_OK;
  doip_connection_t *conn;
  uint16_t capacity;
  uint16_t pos = 0u;
  uint32_t bytes;

  do {
    BREAK_IF_ACTION(ctx == NULL || payload == NULL || length == NULL || connection >= DOIP_CONNECTIONS_COUNT, err = E_PARAM);
    capacity = *length;
    *length = 0u;
    BREAK_IF_ACTION(ctx->configured == false || ATOMIC_LOAD(ctx->reset_trigger), err = E_INVALACT);
    BREAK_IF_ACTION(!ATOMIC_TRY_LOCK(ctx->lock), err = E_AGAIN);

    conn = &ctx->connections[connection];
    while(pos < capacity && conn->state != DOIP_STATE_CLOSED && conn->state != DOIP_STATE_CLOSING) {
      if(conn->rx_state == DOIP_RX_STATE_COMPLETE) {
        doip_receive_complete(ctx, conn);
        BREAK_IF(conn->rx_state == DOIP_RX_STATE_COMPLETE);
        continue;
      }
      if(conn->rx_state == DOIP_RX_STATE_DISCARD) {
        bytes = MIN(conn->discard_left, (uint32_t)capacity - pos);
        conn->discard_left -= bytes;
        pos += bytes;
        if(conn->discard_left == 0u) {
          conn->rx_pos = 0u;
          conn->rx_state = DOIP_RX_STATE_HEADER;
          conn->receive_time = 0u;
        }
        continue;
      }
      bytes = conn->rx_state == DOIP_RX_STATE_HEADER ? DOIP_HEADER_LEN : conn->rx_length;
      bytes = MIN(bytes - conn->rx_pos, (uint32_t)capacity - pos);
      memcpy(&conn->rx_buffer[conn->rx_pos], &payload[pos], bytes);
      conn->rx_pos += bytes;
      pos += bytes;
      if(conn->rx_state == DOIP_RX_STATE_HEADER && conn->rx_pos == DOIP_HEADER_LEN) {
        doip_receive_header(ctx, conn);
        BREAK_IF(conn->rx_state == DOIP_RX_STATE_HEADER && conn->rx_pos == DOIP_HEADER_LEN);
      }
      if(conn->rx_state == DOIP_RX_STATE_PAYLOAD && conn->rx_pos == conn->rx_length) {
        conn->rx_state = DOIP_RX_STATE_COMPLETE;
        doip_receive_complete(ctx, conn);
      }
    }
    *length = pos;
    if(pos == 0u && capacity != 0u) {
      err = E_AGAIN;
    }
    ATOMIC_RELEASE(ctx->lock);

  } while(0);

  return err;
}

error_t doip_stream_peek_downstream(doip_ctx_t *ctx, uint8_t connection, uint8_t *payload, uint16_t *length)
{
  error_t err = E_OK;
  doip_frame_fifo_t *fifo;
  doip_frame_t *frame;

  do {
    BREAK_IF_ACTION(ctx == NULL || payload == NULL || length == NULL || connection >= DOIP_CONNECTIONS_COUNT, err = E_PARAM);
    BREAK_IF_ACTION(!ATOMIC_TRY_LOCK(ctx->lock), err = E_AGAIN);

    fifo = &ctx->connections[connection].downstream;
    if(fifo->write == fifo->read) {
      err = E_AGAIN;
    } else {
      frame = &fifo->buffer[fifo->read % DOIP_FRAME_FIFO_LEN];
      *length = MIN(*length, frame->length - fifo->pos);
      memcpy(payload, &frame->payload[fifo->pos], *length);
    }
    ATOMIC_RELEASE(ctx->lock);

  } while(0);

  return err;
}

error_t doip_stream_consume_downstream(doip_ctx_t *ctx, uint8_t connection, uint16_t length)
{
  error_t err = E_OK;
  doip_frame_fifo_t *fifo;
  doip_frame_t *frame;

  do {
    BREAK_IF_ACTION(ctx == NULL || connection >= DOIP_CONNECTIONS_COUNT, err = E_PARAM);
    BREAK_IF_ACTION(!ATOMIC_TRY_LOCK(ctx->lock), err = E_AGAIN);

    fifo = &ctx->connections[connection].downstream;
    if(fifo->write == fifo->read) {
      err = E_AGAIN;
    } else {
      frame = &fifo->buffer[fifo->read % DOIP_FRAME_FIFO_LEN];
      if(length > frame->length - fifo->pos) {
        err = E_PARAM;
      } else {
        fifo->pos += length;
        if(fifo->pos == frame->length) {
          if(doip_read_u16(&frame->payload[2]) == 0x0007u) {
            ctx->connections[connection].alive_sent = true;
            ctx->connections[connection].alive_time = 0u;
          }
          fifo->read++;
          fifo->pos = 0u;
        }
      }
    }
    ATOMIC_RELEASE(ctx->lock);

  } while(0);

  return err;
}

error_t doip_data_read_upstream(doip_ctx_t *ctx, doip_message_t *message, uint8_t *payload, uint16_t *length)
{
  error_t err = E_AGAIN;
  doip_connection_t *conn;
  uint32_t index;

  do {
    BREAK_IF_ACTION(ctx == NULL || message == NULL || payload == NULL || length == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->configured == false || ATOMIC_LOAD(ctx->reset_trigger), err = E_INVALACT);
    BREAK_IF_ACTION(!ATOMIC_TRY_LOCK(ctx->lock), err = E_AGAIN);

    for(uint32_t i = 0; i < DOIP_CONNECTIONS_COUNT; i++) {
      index = (ctx->upstream_next + i) % DOIP_CONNECTIONS_COUNT;
      conn = &ctx->connections[index];
      CONTINUE_IF(conn->state != DOIP_STATE_ROUTING_ACTIVE || !conn->upstream_ready);
      if(*length < conn->upstream_length) {
        err = E_OVERFLOW;
      } else {
        *message = conn->upstream_message;
        *length = conn->upstream_length;
        memcpy(payload, conn->upstream_data, *length);
        conn->upstream_ready = false;
        ctx->upstream_next = (index + 1u) % DOIP_CONNECTIONS_COUNT;
        err = E_OK;
      }
      break;
    }
    ATOMIC_RELEASE(ctx->lock);

  } while(0);

  return err;
}

error_t doip_message_is_active(doip_ctx_t *ctx, const doip_message_t *message)
{
  error_t err = E_OK;
  doip_connection_t *conn;

  do {
    BREAK_IF_ACTION(ctx == NULL || message == NULL || message->connection >= DOIP_CONNECTIONS_COUNT, err = E_PARAM);
    BREAK_IF_ACTION(!ATOMIC_TRY_LOCK(ctx->lock), err = E_AGAIN);

    conn = &ctx->connections[message->connection];
    if(!ctx->configured || ATOMIC_LOAD(ctx->reset_trigger) || conn->state != DOIP_STATE_ROUTING_ACTIVE ||
        conn->generation != message->generation || conn->source_address != message->source_address) {
      err = E_ABORTED;
    }
    ATOMIC_RELEASE(ctx->lock);

  } while(0);

  return err;
}

error_t doip_data_write_downstream(doip_ctx_t *ctx, const doip_message_t *message, const uint8_t *payload, uint16_t length)
{
  error_t err = E_OK;
  doip_connection_t *conn;
  doip_frame_fifo_t *fifo;
  doip_frame_t *frame;

  do {
    BREAK_IF_ACTION(ctx == NULL || message == NULL || payload == NULL || length == 0u, err = E_PARAM);
    BREAK_IF_ACTION(message->connection >= DOIP_CONNECTIONS_COUNT || length > DOIP_PAYLOAD_LEN_MAX, err = E_PARAM);
    BREAK_IF_ACTION(!ATOMIC_TRY_LOCK(ctx->lock), err = E_AGAIN);

    conn = &ctx->connections[message->connection];
    fifo = &conn->downstream;
    if(!ctx->configured || ATOMIC_LOAD(ctx->reset_trigger) || conn->state != DOIP_STATE_ROUTING_ACTIVE ||
        conn->generation != message->generation || conn->source_address != message->source_address) {
      err = E_ABORTED;
    } else if(fifo->write - fifo->read >= DOIP_FRAME_FIFO_LEN - 1u) {
      /* Keep one queue entry available for transport control messages. */
      err = E_AGAIN;
    } else {
      frame = &fifo->buffer[fifo->write % DOIP_FRAME_FIFO_LEN];
      doip_header_write(frame->payload, conn->version, 0x8001u, length + DOIP_ADDRESS_LEN);
      doip_write_u16(&frame->payload[8], ctx->config.logical_address);
      doip_write_u16(&frame->payload[10], message->source_address);
      memcpy(&frame->payload[12], payload, length);
      frame->length = length + 12u;
      fifo->write++;
    }
    ATOMIC_RELEASE(ctx->lock);

  } while(0);

  return err;
}

error_t doip_datagram_process(doip_ctx_t *ctx, const uint8_t *payload, uint16_t length, uint8_t *response, uint16_t *response_length)
{
  error_t err = E_OK;
  uint16_t type;
  uint16_t expected = 0u;
  uint16_t capacity;
  uint16_t size = 0u;
  uint8_t version;
  uint8_t nack = 0xFFu;
  uint8_t open = 0u;

  do {
    BREAK_IF_ACTION(ctx == NULL || payload == NULL || response == NULL || response_length == NULL, err = E_PARAM);
    capacity = *response_length;
    *response_length = 0u;
    BREAK_IF_ACTION(ctx->configured == false || ATOMIC_LOAD(ctx->reset_trigger), err = E_INVALACT);
    BREAK_IF_ACTION(length < DOIP_HEADER_LEN, err = E_BADRESP);
    BREAK_IF_ACTION(capacity < 41u, err = E_OVERFLOW);
    BREAK_IF_ACTION(!ATOMIC_TRY_LOCK(ctx->lock), err = E_AGAIN);

    version = payload[0];
    type = doip_read_u16(&payload[2]);
    if((uint8_t)(payload[0] ^ payload[1]) != 0xFFu ||
        (version != 0x02u && version != 0x03u && !(version == 0xFFu && type >= 1u && type <= 3u))) {
      nack = 0x00u;
    } else if(type != 0u) {
      if(type == 2u) {
        expected = 6u;
      } else if(type == 3u) {
        expected = 17u;
      } else if(type != 1u && type != 0x4001u && type != 0x4003u) {
        nack = 0x01u;
      }
      if(nack == 0xFFu && (doip_read_u32(&payload[4]) != expected || length < DOIP_HEADER_LEN + expected)) {
        nack = 0x04u;
      }
      if(nack == 0xFFu) {
        if(type >= 1u && type <= 3u) {
          if((type != 2u || memcmp(&payload[8], ctx->config.eid, 6u) == 0) &&
              (type != 3u || memcmp(&payload[8], ctx->config.vin, 17u) == 0)) {
            size = doip_identification_write(ctx, version == 0xFFu ? ctx->config.protocol_version : version, response);
          }
        } else if(type == 0x4001u) {
          for(uint32_t i = 0; i < DOIP_CONNECTIONS_COUNT; i++) {
            if(ctx->connections[i].state != DOIP_STATE_CLOSED) {
              open++;
            }
          }
          doip_header_write(response, version, 0x4002u, 7u);
          response[8] = 1u;
          response[9] = ctx->config.max_connections;
          response[10] = MIN(open, ctx->config.max_connections);
          doip_write_u32(&response[11], DOIP_PAYLOAD_LEN_MAX + DOIP_ADDRESS_LEN);
          size = 15u;
        } else if(type == 0x4003u) {
          doip_header_write(response, version, 0x4004u, 1u);
          response[8] = 1u;
          size = 9u;
        }
      }
    }
    if(nack != 0xFFu) {
      doip_header_write(response, ctx->config.protocol_version, 0x0000u, 1u);
      response[8] = nack;
      size = 9u;
    }
    *response_length = size;
    ATOMIC_RELEASE(ctx->lock);

  } while(0);

  return err;
}

error_t doip_vehicle_announcement(doip_ctx_t *ctx, uint8_t *payload, uint16_t *length)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || payload == NULL || length == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->configured == false || ATOMIC_LOAD(ctx->reset_trigger), err = E_INVALACT);
    BREAK_IF_ACTION(*length < 41u, err = E_OVERFLOW);
    BREAK_IF_ACTION(!ATOMIC_TRY_LOCK(ctx->lock), err = E_AGAIN);

    *length = doip_identification_write(ctx, ctx->config.protocol_version, payload);
    ATOMIC_RELEASE(ctx->lock);

  } while(0);

  return err;
}
