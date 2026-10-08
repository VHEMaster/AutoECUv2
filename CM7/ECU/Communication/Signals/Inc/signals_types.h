/*
 * signals_types.h
 *
 *  Periodic runtime parameter CAN signals
 */

#ifndef COMMUNICATION_SIGNALS_INC_SIGNALS_TYPES_H_
#define COMMUNICATION_SIGNALS_INC_SIGNALS_TYPES_H_

#include "common.h"
#include "time.h"
#include "config_common_types.h"
#include "router.h"

#define SIGNALS_MESSAGES_MAX   4u
#define SIGNALS_ITEMS_MAX      4u

typedef struct {
    ecu_config_parameter_id_t parameter_id;
    float multiplier;
    float offset;
    uint8_t byte_offset;
}signals_item_config_t;

typedef struct {
    bool enabled;
    uint32_t message_id;
    time_delta_us_t period;
    uint8_t signals_count;
    signals_item_config_t signals[SIGNALS_ITEMS_MAX];
}signals_message_config_t;

typedef struct {
    uint8_t messages_count;
    signals_message_config_t messages[SIGNALS_MESSAGES_MAX];
}signals_config_t;

typedef struct {
    router_ctx_t *router;
}signals_init_ctx_t;

typedef struct {
    time_us_t last_time;
    bool started;
    can_message_t pending;
    bool pending_valid;
}signals_message_runtime_t;

typedef struct {
    signals_init_ctx_t init;
    signals_config_t config;
    signals_message_runtime_t messages[SIGNALS_MESSAGES_MAX];
    bool initialized;
    bool configured;
}signals_ctx_t;

#endif /* COMMUNICATION_SIGNALS_INC_SIGNALS_TYPES_H_ */
