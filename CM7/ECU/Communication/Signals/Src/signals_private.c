/*
 * signals_private.c
 *
 *  Created on: 13 окт. 2026 г.
 *      Author: VHEMaster
 */

#include "config_comm.h"
#include "config_common.h"
#include "signals_private.h"

typedef error_t (*signals_conversion_func_t)(can_signal_value_t *value, uint32_t valmask, float gain, float offset);

static error_t signals_phy_to_raw_uint(can_signal_value_t *value, uint32_t valmask, float gain, float offset);
static error_t signals_phy_to_raw_sint(can_signal_value_t *value, uint32_t valmask, float gain, float offset);

static error_t signals_raw_to_phy_uint(can_signal_value_t *value, uint32_t valmask, float gain, float offset);
static error_t signals_raw_to_phy_sint(can_signal_value_t *value, uint32_t valmask, float gain, float offset);

static signals_conversion_func_t signals_phy_to_raw_funcs[CAN_SIGNAL_TYPE_MAX] = {
    signals_phy_to_raw_uint, //CAN_SIGNAL_TYPE_UINT
    signals_phy_to_raw_sint, //CAN_SIGNAL_TYPE_SINT
    signals_phy_to_raw_uint, //CAN_SIGNAL_TYPE_UFLT
    signals_phy_to_raw_sint  //CAN_SIGNAL_TYPE_SFLT
};

static signals_conversion_func_t signals_raw_to_phy_funcs[CAN_SIGNAL_TYPE_MAX] = {
    signals_raw_to_phy_uint, //CAN_SIGNAL_TYPE_UINT
    signals_raw_to_phy_sint, //CAN_SIGNAL_TYPE_SINT
    signals_raw_to_phy_uint, //CAN_SIGNAL_TYPE_UFLT
    signals_raw_to_phy_sint  //CAN_SIGNAL_TYPE_SFLT
};

static error_t signals_handle_downstream_message(signals_runtime_messages_fifo_item_t *working_item, signals_runtime_message_ctx_t *message);
static error_t signals_handle_upstream_message(signals_runtime_messages_fifo_item_t *working_item, signals_runtime_message_ctx_t *message);

static error_t signals_can_write_bits(uint8_t *payload, uint8_t payloadlen, uint8_t bitlength, uint16_t startbit, can_signal_byteorder_t byteorder, uint32_t raw);
static error_t signals_can_read_bits(const uint8_t *payload, uint8_t payloadlen, uint8_t bitlength, uint16_t startbit, can_signal_byteorder_t byteorder, uint32_t *raw);

static error_t signals_phy_to_raw(can_signal_value_t *value, uint32_t valmask, can_signal_type_t type, float gain, float offset);
static error_t signals_raw_to_phy(can_signal_value_t *value, uint32_t valmask, can_signal_type_t type, float gain, float offset);

static error_t signals_calculate_message_crc(uint8_t *p_crc, const uint8_t *payload, uint8_t payloadlen, uint8_t poly, uint8_t crc_byte_pos);
static error_t signals_invalidate_upstream_message(signals_runtime_message_ctx_t *message);

INLINE error_t signals_frame_fifo_push(signals_runtime_messages_fifo_t *fifo, const signals_runtime_messages_fifo_item_t *message)
{
  error_t err = E_OK;
  uint32_t read, write, cur;

  do {
    BREAK_IF_ACTION(fifo == NULL, err = E_PARAM);
    BREAK_IF_ACTION(message == NULL, err = E_PARAM);

    read = fifo->read;
    write = fifo->write;
    cur = write;

    if(++write >= fifo->length) {
      write = 0;
    }

    if(write == read) {
      err = E_OVERFLOW;
    } else {
      memcpy(&fifo->buffer[cur], message, sizeof(signals_runtime_messages_fifo_item_t));
      fifo->write = write;
    }

  } while(0);

  return err;
}

INLINE error_t signals_frame_fifo_pull(signals_runtime_messages_fifo_t *fifo, signals_runtime_messages_fifo_item_t *message)
{
  error_t err = E_OK;
  uint32_t read, write;

  do {
    BREAK_IF_ACTION(fifo == NULL, err = E_PARAM);
    BREAK_IF_ACTION(message == NULL, err = E_PARAM);

    read = fifo->read;
    write = fifo->write;

    if(read == write) {
      err = E_AGAIN;
    } else {
      memcpy(message, &fifo->buffer[read], sizeof(signals_runtime_messages_fifo_item_t));
      if(++read >= fifo->length) {
        read = 0;
      }
      fifo->read = read;
    }

  } while(0);

  return err;
}

error_t signals_config_build(signals_ctx_t *ctx)
{
  error_t err = E_OK;
  time_us_t now;
  const signals_config_t *config;
  const signals_config_message_description_t *message_config;
  const signals_config_signal_description_t *signal_config;
  ecu_core_runtime_value_ctx_t *value;

  signals_runtime_message_ctx_t *message_runtime;
  signals_runtime_signal_tree_t *signal_runtime;
  signals_runtime_signal_tree_t *signal_runtime_prev;
  uint32_t message_id;

  do {
    config = &ctx->config;
    BREAK_IF_ACTION(ctx->init.messages_downstream_buffer == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->init.messages_upstream_buffer == NULL, err = E_PARAM);

    BREAK_IF_ACTION(ctx->init.messages_downstream_buffer_len < 2u, err = E_PARAM);
    BREAK_IF_ACTION(ctx->init.messages_upstream_buffer_len < 2u, err = E_PARAM);

    ctx->runtime.fifo_messages_downstream.buffer = ctx->init.messages_downstream_buffer;
    ctx->runtime.fifo_messages_downstream.length = ctx->init.messages_downstream_buffer_len;
    ctx->runtime.fifo_messages_upstream.buffer = ctx->init.messages_upstream_buffer;
    ctx->runtime.fifo_messages_upstream.length = ctx->init.messages_upstream_buffer_len;

    ctx->runtime.messages_downstream.messages_used = 0;
    ctx->runtime.messages_downstream.signals_used = 0;
    ctx->runtime.downstream_working_item_sending = false;

    now = time_now_us();

    for(uint32_t i = 0; i < ITEMSOF(config->messages_lists.downstream_list); i++) {
      RETURN_IF(ctx->runtime.messages_downstream.messages_used >= ITEMSOF(config->messages_lists.downstream_list), E_PARAM);

      message_config = &config->messages_lists.downstream_list[i];
      CONTINUE_IF(message_config->enabled != true);

      RETURN_IF(message_config->message_len > CAN_MESSAGE_PAYLOAD_LEN_MAX, E_PARAM);
      if(message_config->has_crc) {
        RETURN_IF(message_config->crc_byte_pos >= message_config->message_len, E_PARAM);
      }
      if(message_config->has_counter) {
        RETURN_IF(message_config->counter_byte_pos >= message_config->message_len, E_PARAM);
      }
      if(message_config->has_crc && message_config->has_counter) {
        RETURN_IF(message_config->crc_byte_pos == message_config->counter_byte_pos, E_PARAM);
      }

      message_runtime = &ctx->runtime.messages_downstream.messages[ctx->runtime.messages_downstream.messages_used];
      message_runtime->used = true;
      message_runtime->last = now;
      message_runtime->signals_tree = NULL;
      message_runtime->message_config = message_config;
      message_id = message_config->message_id;
      signal_runtime_prev = NULL;

      for(uint32_t s = 0; s < ITEMSOF(config->signals_lists.downstream_list); s++) {
        signal_config = &config->signals_lists.downstream_list[s];
        CONTINUE_IF(signal_config->enabled != true);
        if(signal_config->message_id == message_id) {
          RETURN_IF(ctx->runtime.messages_downstream.signals_used >= ITEMSOF(config->signals_lists.downstream_list), E_PARAM);
          signal_runtime = &ctx->runtime.messages_downstream.signal_tree[ctx->runtime.messages_downstream.signals_used];
          signal_runtime->signal_config = signal_config;
          err = ecu_config_common_get_parameter_ptr_by_id(signal_config->parameter_id, &value);
          BREAK_IF(err != E_OK);
          BREAK_IF_ACTION(value == NULL, err = E_BADRESP);
          signal_runtime->parameter_ptr = value;
          signal_runtime->next = NULL;
          if(signal_runtime_prev != NULL) {
            signal_runtime_prev->next = signal_runtime;
          } else {
            message_runtime->signals_tree = signal_runtime;
          }
          signal_runtime_prev = signal_runtime;
          ctx->runtime.messages_downstream.signals_used++;
        }
      }
      BREAK_IF(err != E_OK);
      ctx->runtime.messages_downstream.messages_used++;
    }
    BREAK_IF(err != E_OK);

    for(uint32_t i = 0; i < ITEMSOF(config->messages_lists.upstream_list); i++) {
      RETURN_IF(ctx->runtime.messages_upstream.messages_used >= ITEMSOF(config->messages_lists.upstream_list), E_PARAM);
      message_config = &config->messages_lists.upstream_list[i];
      CONTINUE_IF(message_config->enabled != true);

      RETURN_IF(message_config->message_len > CAN_MESSAGE_PAYLOAD_LEN_MAX, E_PARAM);
      if(message_config->has_crc) {
        RETURN_IF(message_config->crc_byte_pos >= message_config->message_len, E_PARAM);
      }
      if(message_config->has_counter) {
        RETURN_IF(message_config->counter_byte_pos >= message_config->message_len, E_PARAM);
      }
      if(message_config->has_crc && message_config->has_counter) {
        RETURN_IF(message_config->crc_byte_pos == message_config->counter_byte_pos, E_PARAM);
      }

      message_runtime = &ctx->runtime.messages_upstream.messages[ctx->runtime.messages_upstream.messages_used];
      message_runtime->used = true;
      message_runtime->last = now;
      message_runtime->signals_tree = NULL;
      message_runtime->message_config = message_config;
      message_id = message_config->message_id;
      signal_runtime_prev = NULL;

      for(uint32_t s = 0; s < ITEMSOF(config->signals_lists.upstream_list); s++) {
        signal_config = &config->signals_lists.upstream_list[s];
        CONTINUE_IF(signal_config->enabled != true);
        if(signal_config->message_id == message_id) {
          RETURN_IF(ctx->runtime.messages_upstream.signals_used >= ITEMSOF(config->signals_lists.upstream_list), E_PARAM);
          signal_runtime = &ctx->runtime.messages_upstream.signal_tree[ctx->runtime.messages_upstream.signals_used];
          signal_runtime->signal_config = signal_config;

          // TODO: assign proper ID to be written by CAN
          err = ecu_config_common_get_parameter_ptr_by_id(signal_config->parameter_id, &value);
          BREAK_IF(err != E_OK);
          BREAK_IF_ACTION(value == NULL, err = E_BADRESP);
          signal_runtime->parameter_ptr = value;
          signal_runtime->next = NULL;
          if(signal_runtime_prev != NULL) {
            signal_runtime_prev->next = signal_runtime;
          } else {
            message_runtime->signals_tree = signal_runtime;
          }
          signal_runtime_prev = signal_runtime;
          ctx->runtime.messages_upstream.signals_used++;
        }
      }
      BREAK_IF(err != E_OK);
      ctx->runtime.messages_upstream.messages_used++;
    }
    BREAK_IF(err != E_OK);

  } while(0);

  return err;
}

void signals_handle_upstream(signals_ctx_t *ctx)
{
  error_t err;
  signals_runtime_messages_fifo_item_t *working_item;
  signals_runtime_message_ctx_t *message;
  time_us_t now;
  uint32_t message_index, messages_used, messages_to_handle;

  do {
    working_item = &ctx->runtime.upstream_working_item;
    now = time_now_us();
    messages_used = ctx->runtime.messages_upstream.messages_used;

    for(uint32_t i = 0; i < SIGNALS_UPSTREAM_FIFO_LIMIT_PER_CALL; i++) {
      err = signals_frame_fifo_pull(&ctx->runtime.fifo_messages_upstream, working_item);
      if(err == E_OK) {
        for(uint32_t m = 0; m < messages_used; m++) {
          message = &ctx->runtime.messages_upstream.messages[m];
          if(message->message_config->message_id == working_item->message.id) {
            err = signals_handle_upstream_message(working_item, message);
            BREAK_IF(err != E_OK);

            message->last = now;
            break;
          }
        }
      } else if(err != E_AGAIN) {
        // TODO: error handling
        break;
      } else {
        break;
      }
    }

    message_index = ctx->runtime.messages_upstream.message_index;
    messages_to_handle = MIN(SIGNALS_UPSTREAM_TIMEOUT_HANDLER_LIMIT_PER_CALL, messages_used);
    for(uint32_t i = 0; i < messages_to_handle; i++) {
      message = &ctx->runtime.messages_upstream.messages[message_index];

      if(message->message_config->upstream_timeout != 0u &&
          time_diff(now, message->last) > message->message_config->upstream_timeout) {
        err = E_TIMEOUT;

        (void)signals_invalidate_upstream_message(message);

        message->last = now;
      }

      if(++message_index >= messages_used) {
        message_index = 0;
      }
    }
    ctx->runtime.messages_upstream.message_index = message_index;



  } while(0);
}

void signals_handle_downstream(signals_ctx_t *ctx)
{
  error_t err;
  signals_runtime_messages_fifo_item_t *working_item;
  signals_runtime_message_ctx_t *message;
  time_us_t now;
  uint32_t message_index, messages_used;
  bool *working_item_sending;

  do {
    working_item_sending = &ctx->runtime.downstream_working_item_sending;
    working_item = &ctx->runtime.downstream_working_item;
    now = time_now_us();
    message_index = ctx->runtime.messages_downstream.message_index;
    messages_used = ctx->runtime.messages_downstream.messages_used;

    for(uint32_t i = 0; i < messages_used; i++) {
      message = &ctx->runtime.messages_downstream.messages[message_index];
      if(*working_item_sending == false) {
        if(message->used && time_diff(now, message->last) > message->message_config->downstream_periodicity) {
          err = signals_handle_downstream_message(working_item, message);
          if(err == E_OK) {
            *working_item_sending = true;
          }
        }
      }

      if(*working_item_sending == true) {
        err = signals_frame_fifo_push(&ctx->runtime.fifo_messages_downstream, working_item);
        if(err == E_OK) {
          message->last = now;
        } else if(message->message_config->upstream_timeout != 0u &&
            time_diff(now, message->last) > message->message_config->upstream_timeout) {
          // TODO: error handling
          message->last = now;
        } else {
          break;
        }
        *working_item_sending = false;
      }

      if(++message_index >= messages_used) {
        message_index = 0;
      }
    }

    ctx->runtime.messages_downstream.message_index = message_index;

  } while(0);
}

static error_t signals_handle_downstream_message(signals_runtime_messages_fifo_item_t *working_item, signals_runtime_message_ctx_t *message)
{
  error_t err = E_OK;
  error_t err_local;
  const signals_config_message_description_t *message_config;
  const signals_config_signal_description_t *signal_config;
  signals_runtime_signal_tree_t *signal;
  ecu_core_runtime_value_ctx_t value;
  uint32_t valmask;

  do {
    memset(&working_item->message, 0u, sizeof(working_item->message));

    message_config = message->message_config;

    working_item->message.id = message_config->message_id;
    working_item->message.len = message_config->message_len;
    working_item->timestamp = time_now_us();

    signal = message->signals_tree;
    while(signal != NULL) {
      signal_config = signal->signal_config;
      if(signal_config != NULL) {
        if(signal_config->description.bitlength == 32u) {
          valmask = ~0u;
        } else {
          valmask = (1u << signal_config->description.bitlength) - 1u;
        }

        if(signal->parameter_ptr != NULL) {
          value = *signal->parameter_ptr;
          if(value.valid) {
            signal->value.phy_value = value.value;
            err_local = signals_phy_to_raw(&signal->value, valmask,
                signal_config->description.type,
                signal_config->description.gain,
                signal_config->description.offset);
            if(err_local != E_OK) {
              signal->value.raw_value = valmask;
            }
          } else {
            signal->value.raw_value = valmask;
          }
        } else {
          signal->value.raw_value = valmask;
        }

        if((signal->value.raw_value & ~valmask) != 0u) {
          err = E_OVERFLOW;
          break;
        }
        signal->value.raw_value &= valmask;

        err = signals_can_write_bits(working_item->message.payload, working_item->message.len,
            signal_config->description.bitlength, signal_config->description.startbit,
            signal_config->description.byteorder, signal->value.raw_value);
        BREAK_IF(err != E_OK);

      } else {
        err = E_FAULT;
        break;
      }
      signal = signal->next;
    }
    BREAK_IF(err != E_OK);

    if(message_config->has_counter) {
      message->periodic_counter = (message->periodic_counter + 1) & message_config->counter_mask;
      working_item->message.payload[message_config->counter_byte_pos] = message->periodic_counter;
    }

    if(message_config->has_crc) {
      err = signals_calculate_message_crc(&message->crc, working_item->message.payload,
          working_item->message.len, message_config->crc_poly, message_config->crc_byte_pos);
      BREAK_IF(err != E_OK);

      working_item->message.payload[message_config->crc_byte_pos] = message->crc;
    }

  } while(0);

  return err;
}

static error_t signals_handle_upstream_message(signals_runtime_messages_fifo_item_t *working_item, signals_runtime_message_ctx_t *message)
{
  error_t err = E_OK;
  const signals_config_message_description_t *message_config;
  const signals_config_signal_description_t *signal_config;
  signals_runtime_signal_tree_t *signal;
  ecu_core_runtime_value_ctx_t value;
  uint32_t valmask;
  uint8_t crc_calculated, crc_received;
  uint8_t counter_received, counter_expected;

  do {
    message_config = message->message_config;

    BREAK_IF_ACTION(working_item->message.id != message_config->message_id, err = E_BADVALUE);
    BREAK_IF_ACTION(working_item->message.len != message_config->message_len, err = E_BADVALUE);

    signal = message->signals_tree;
    while(signal != NULL) {
      signal_config = signal->signal_config;
      if(signal_config != NULL) {
        if(signal_config->description.bitlength == 32u) {
          valmask = ~0u;
        } else {
          valmask = (1u << signal_config->description.bitlength) - 1u;
        }

        err = signals_can_read_bits(working_item->message.payload, working_item->message.len,
            signal_config->description.bitlength, signal_config->description.startbit,
            signal_config->description.byteorder, &signal->value.raw_value);
        BREAK_IF(err != E_OK);

        err = signals_raw_to_phy(&signal->value, valmask,
            signal_config->description.type,
            signal_config->description.gain,
            signal_config->description.offset);
        BREAK_IF(err != E_OK);
      } else {
        err = E_FAULT;
        break;
      }
      signal = signal->next;
    }
    BREAK_IF(err != E_OK);

    if(message_config->has_crc) {
      crc_received = working_item->message.payload[message_config->crc_byte_pos];
      err = signals_calculate_message_crc(&crc_calculated, working_item->message.payload,
          working_item->message.len, message_config->crc_poly, message_config->crc_byte_pos);
      message->crc = crc_calculated;
      BREAK_IF(err != E_OK);

      if(crc_calculated != crc_received) {
        err = E_BADRESP;
        // TODO: error handling
        break;
      }
    }

    if(message_config->has_counter) {
      counter_received = working_item->message.payload[message_config->counter_byte_pos] & message_config->counter_mask;
      counter_expected = (message->periodic_counter + 1) & message_config->counter_mask;
      if(message->counter_valid == true) {
        if(counter_received != counter_expected) {
          err = E_BADRESP;
          // TODO: error handling
          break;
        }
      }
      message->counter_valid = true;
      message->periodic_counter = counter_received;
    }

    signal = message->signals_tree;
    while(signal != NULL) {
      value.valid = true;
      value.value = signal->value.phy_value;
      if(signal->parameter_ptr != NULL) {
        *signal->parameter_ptr = value;
      }
      signal = signal->next;
    }


  } while(0);

  return err;
}

static error_t signals_calculate_message_crc(uint8_t *p_crc, const uint8_t *payload, uint8_t payloadlen, uint8_t poly, uint8_t crc_byte_pos)
{
  error_t err = E_OK;
  uint8_t crc = 0xFF;

  // TODO: use HW implementation
  do {
    for (uint32_t i = 0; i < payloadlen; i++) {
      CONTINUE_IF(i == crc_byte_pos);
      crc ^= payload[i];

      for (uint8_t bit = 0; bit < 8; bit++) {
        if (crc & 0x80) {
          crc = (crc << 1) ^ poly;
        } else {
          crc <<= 1;
        }
      }
    }

    *p_crc = crc ^ 0xFF;
  } while(0);

  return err;
}

STATIC_INLINE error_t signals_can_write_bits(uint8_t *payload, uint8_t payloadlen, uint8_t bitlength, uint16_t startbit, can_signal_byteorder_t byteorder, uint32_t raw)
{
  error_t err = E_OK;
  uint32_t byte_index = startbit >> 3u;
  uint8_t bit_index = startbit & 0x7u;
  uint8_t remaining = bitlength;
  uint8_t processed = 0u;
  uint32_t bit_position;
  uint8_t byte_mask;

  uint8_t available;
  uint8_t count;
  uint32_t mask;
  uint8_t shift;
  uint32_t part;

  do {
    BREAK_IF_ACTION(payload == NULL, err = E_PARAM);
    BREAK_IF_ACTION(byteorder >= CAN_SIGNAL_BYTEORDER_MAX, err = E_PARAM);
    BREAK_IF_ACTION(bitlength == 0u || bitlength > 32u, err = E_PARAM);

    bit_position = (byteorder == CAN_SIGNAL_BYTEORDER_INTEL)
        ? startbit
        : (startbit ^ 0x7u);


    BREAK_IF_ACTION((bit_position + bitlength) > (payloadlen << 3u), err = E_OVERFLOW);

    while(remaining) {
      available = (byteorder == CAN_SIGNAL_BYTEORDER_INTEL)
        ? (8u - bit_index)
        : (bit_index + 1u);
      count = remaining < available ? remaining : available;
      mask = (1u << count) - 1u;

      if(byteorder == CAN_SIGNAL_BYTEORDER_INTEL) {
        shift = bit_index;
        part = (raw >> processed) & mask;
        processed = (processed + count);
        bit_index = 0u;
      } else {
        shift = (bit_index + 1u - count);
        part = (raw >> (remaining - count)) & mask;
        bit_index = 7u;
      }

      byte_mask = (mask << shift);
      payload[byte_index] = ((payload[byte_index] & ~byte_mask) | (part << shift));

      remaining = remaining - count;
      byte_index++;
    }
  } while(0);

  return err;
}

STATIC_INLINE error_t signals_can_read_bits(const uint8_t *payload, uint8_t payloadlen, uint8_t bitlength, uint16_t startbit, can_signal_byteorder_t byteorder, uint32_t *raw)
{
  error_t err = E_OK;
  uint32_t byte_index = startbit >> 3u;
  uint8_t bit_index = startbit & 0x7u;
  uint8_t remaining = bitlength;
  uint8_t processed = 0u;
  uint32_t bit_position;
  uint32_t payload_bits = payloadlen;

  uint8_t available;
  uint8_t count;
  uint32_t mask;
  uint8_t shift;
  uint32_t part;
  uint32_t value = 0u;

  do {
    BREAK_IF_ACTION(payload == NULL || raw == NULL, err = E_PARAM);
    BREAK_IF_ACTION(byteorder >= CAN_SIGNAL_BYTEORDER_MAX, err = E_PARAM);
    BREAK_IF_ACTION(bitlength == 0u || bitlength > 32u, err = E_PARAM);

    bit_position = (byteorder == CAN_SIGNAL_BYTEORDER_INTEL)
        ? startbit
        : (startbit ^ 0x7u);

    payload_bits <<= 3u;

    BREAK_IF_ACTION(bit_position + bitlength > payload_bits, err = E_OVERFLOW);

    while(remaining) {
      available = (byteorder == CAN_SIGNAL_BYTEORDER_INTEL)
          ? (8u - bit_index)
          : (bit_index + 1u);

      count = remaining < available ? remaining : available;
      mask = (1u << count) - 1u;

      if(byteorder == CAN_SIGNAL_BYTEORDER_INTEL) {
        shift = bit_index;
        part = (payload[byte_index] >> shift) & mask;
        value |= part << processed;
        processed = processed + count;
        bit_index = 0u;
      } else {
        shift = bit_index + 1u - count;
        part = (payload[byte_index] >> shift) & mask;
        value |= part << (remaining - count);
        bit_index = 7u;
      }

      remaining = remaining - count;
      byte_index++;
    }

    *raw = value;

  } while(0);

  return err;
}

STATIC_INLINE error_t signals_phy_to_raw(can_signal_value_t *value, uint32_t valmask, can_signal_type_t type, float gain, float offset)
{
  error_t err = E_OK;
  signals_conversion_func_t func;

  do {
    BREAK_IF_ACTION(type >= CAN_SIGNAL_TYPE_MAX, err = E_PARAM);
    BREAK_IF_ACTION(value == NULL, err = E_PARAM);

    func = signals_phy_to_raw_funcs[type];

    err = func(value, valmask, gain, offset);

  } while(0);

  return err;

}

STATIC_INLINE error_t signals_raw_to_phy(can_signal_value_t *value, uint32_t valmask, can_signal_type_t type, float gain, float offset)
{
  error_t err = E_OK;
  signals_conversion_func_t func;

  do {
    BREAK_IF_ACTION(type >= CAN_SIGNAL_TYPE_MAX, err = E_PARAM);
    BREAK_IF_ACTION(value == NULL, err = E_PARAM);

    func = signals_raw_to_phy_funcs[type];

    err = func(value, valmask, gain, offset);

  } while(0);

  return err;
}

static error_t signals_phy_to_raw_uint(can_signal_value_t *value, uint32_t valmask, float gain, float offset)
{
  error_t err = E_OK;
  float value_flt;

  do {
    BREAK_IF_ACTION(value == NULL, err = E_PARAM);

    value->raw_value = valmask;

    BREAK_IF(!isfinite(value->phy_value));
    BREAK_IF(gain == 0.0f);

    value_flt = value->phy_value;
    value_flt -= offset;
    value_flt /= gain;
    value_flt = roundf(value_flt);

    BREAK_IF(isnan(value_flt));

    if(value_flt < 0.0f) {
      value->raw_value = 0u;
      err = E_OVERFLOW;
    } else if(value_flt >= (float)valmask + 1.0f) {
      value->raw_value = valmask;
      err = E_OVERFLOW;
    } else {
      value->raw_value = (uint32_t)value_flt;
    }

  } while(0);

  return err;
}

static error_t signals_phy_to_raw_sint(can_signal_value_t *value, uint32_t valmask, float gain, float offset)
{
  error_t err = E_OK;
  float value_flt, limit;
  uint32_t signbit;

  do {
    BREAK_IF_ACTION(value == NULL, err = E_PARAM);

    value->raw_value = valmask;

    BREAK_IF(!isfinite(value->phy_value));
    BREAK_IF(gain == 0.0f);

    signbit = (valmask >> 1) + 1u;
    limit = (float)signbit;

    value_flt = value->phy_value;
    value_flt -= offset;
    value_flt /= gain;
    value_flt = roundf(value_flt);

    BREAK_IF(isnan(value_flt));

    if(value_flt < -limit) {
      value->raw_value = signbit;
      err = E_OVERFLOW;
    } else if(value_flt >= limit) {
      value->raw_value = signbit - 1u;
      err = E_OVERFLOW;
    } else {
      value->raw_value = (uint32_t)(int32_t)value_flt & valmask;
    }

  } while(0);

  return err;
}

static error_t signals_raw_to_phy_uint(can_signal_value_t *value, uint32_t valmask, float gain, float offset)
{
  error_t err = E_OK;
  float value_flt;

  do {
    BREAK_IF_ACTION(value == NULL, err = E_PARAM);
    BREAK_IF_ACTION(gain == 0.0f, err = E_PARAM);

    value_flt = (float)(value->raw_value & valmask);
    value_flt *= gain;
    value_flt += offset;

    BREAK_IF_ACTION(!isfinite(value_flt), err = E_OVERFLOW);

    value->phy_value = value_flt;
  } while(0);

  return err;
}

static error_t signals_raw_to_phy_sint(can_signal_value_t *value, uint32_t valmask, float gain, float offset)
{
  error_t err = E_OK;
  float value_flt;
  uint32_t raw, signbit;

  do {
    BREAK_IF_ACTION(value == NULL, err = E_PARAM);
    BREAK_IF_ACTION(gain == 0.0f, err = E_PARAM);

    raw = value->raw_value & valmask;
    signbit = (valmask >> 1) + 1u;

    if((raw & signbit)) {
      value_flt = -(float)(((~raw) & valmask) + 1u);
    } else {
      value_flt = (float)raw;
    }

    value_flt *= gain;
    value_flt += offset;

    BREAK_IF_ACTION(!isfinite(value_flt), err = E_OVERFLOW);

    value->phy_value = value_flt;
  } while(0);

  return err;
}

static error_t signals_invalidate_upstream_message(signals_runtime_message_ctx_t *message)
{
  error_t err = E_OK;
  signals_runtime_signal_tree_t *signal;

  do {
    message->counter_valid = false;
    signal = message->signals_tree;
    while(signal != NULL) {
      if(signal->parameter_ptr != NULL) {
        signal->parameter_ptr->valid = false;
      }
      signal = signal->next;
    }
  } while(0);

  return err;
}
