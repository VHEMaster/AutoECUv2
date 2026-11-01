/*
 * signals_types.h
 *
 *  Created on: 13 окт. 2026 г.
 *      Author: VHEMaster
 */

#ifndef COMMUNICATION_SIGNALS_INC_SIGNALS_TYPES_H_
#define COMMUNICATION_SIGNALS_INC_SIGNALS_TYPES_H_

#include "common.h"
#include "time.h"
#include "versioned_signals.h"

#define SIGNALS_UPSTREAM_FIFO_LIMIT_PER_CALL              16u
#define SIGNALS_UPSTREAM_TIMEOUT_HANDLER_LIMIT_PER_CALL   16u

typedef struct signals_ctx_tag signals_ctx_t;

typedef struct signals_runtime_downstream_signal_tree_tag signals_runtime_downstream_signal_tree_t;

typedef struct {
    can_message_t message;
    time_us_t timestamp;
}signals_runtime_messages_fifo_item_t;

typedef struct {
    signals_runtime_messages_fifo_item_t *buffer;
    uint32_t length;
    uint32_t read;
    uint32_t write;
}signals_runtime_messages_fifo_t;

typedef struct {
    bool used;
    time_us_t last;

    const signals_config_message_description_t *message_config;
    signals_runtime_downstream_signal_tree_t *signals_tree;
    uint8_t periodic_counter;
    uint8_t crc;
    bool counter_valid;
}signals_runtime_message_ctx_t;

typedef struct signals_runtime_downstream_signal_tree_tag {
    const signals_config_signal_description_t *signal_config;
    can_signal_value_t value;
    ecu_core_runtime_value_ctx_t *parameter_ptr;
    signals_runtime_downstream_signal_tree_t *next;
}signals_runtime_signal_tree_t;

typedef struct {
    signals_runtime_message_ctx_t messages[SIGNALS_CONFIG_MESSAGES_DOWNSTREAM_LIST_SIZE_MAX];
    signals_runtime_signal_tree_t signal_tree[SIGNALS_CONFIG_SIGNALS_DOWNSTREAM_LIST_SIZE_MAX];

    uint32_t messages_used;
    uint32_t message_index;
    uint32_t signals_used;

}signals_runtime_downstream_messages_ctx_t;


typedef struct {
    signals_runtime_message_ctx_t messages[SIGNALS_CONFIG_MESSAGES_UPSTREAM_LIST_SIZE_MAX];
    signals_runtime_signal_tree_t signal_tree[SIGNALS_CONFIG_SIGNALS_UPSTREAM_LIST_SIZE_MAX];

    uint32_t messages_used;
    uint32_t message_index;
    uint32_t signals_used;

}signals_runtime_upstream_messages_ctx_t;

typedef struct {
    signals_runtime_messages_fifo_t fifo_messages_upstream;
    signals_runtime_messages_fifo_t fifo_messages_downstream;

    signals_runtime_downstream_messages_ctx_t messages_downstream;
    signals_runtime_upstream_messages_ctx_t messages_upstream;

    signals_runtime_messages_fifo_item_t upstream_working_item;
    signals_runtime_messages_fifo_item_t downstream_working_item;
    bool downstream_working_item_sending;


}signals_runtime_ctx_t;

typedef struct {
    signals_runtime_messages_fifo_item_t *messages_upstream_buffer;
    signals_runtime_messages_fifo_item_t *messages_downstream_buffer;
    uint32_t messages_upstream_buffer_len;
    uint32_t messages_downstream_buffer_len;

}signals_init_ctx_t;

typedef struct signals_ctx_tag {
    signals_config_t config;
    signals_init_ctx_t init;
    bool initialized;
    bool configured;

    signals_runtime_ctx_t runtime;

    bool reset_trigger;

}signals_ctx_t;

#endif /* COMMUNICATION_SIGNALS_INC_SIGNALS_TYPES_H_ */
