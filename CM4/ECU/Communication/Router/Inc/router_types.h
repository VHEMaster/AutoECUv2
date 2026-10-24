/*
 * router_types.h
 *
 *  Created on: Oct 23, 2025
 *      Author: VHEMaster
 */

#ifndef COMMUNICATION_ROUTER_INC_ROUTER_TYPES_H_
#define COMMUNICATION_ROUTER_INC_ROUTER_TYPES_H_

#include "common.h"
#include "time.h"
#include "versioned_router.h"

#include "can.h"
#include "kwp.h"
#include "isotp.h"
#include "uds.h"
#include "obd2.h"
#include "router.h"
#include "signals.h"

typedef enum {
  ROUTER_OK = 0,
  ROUTER_MAX
}router_error_code_t;

#define ROUTER_UPSTREAM_LOOP_ITERATIONS_MAX     32u
#define ROUTER_DOWNSTREAM_LOOP_ITERATIONS_MAX   32u

typedef struct router_ctx_tag router_ctx_t;

typedef void (*router_error_callback_t)(router_ctx_t *ctx, router_error_code_t code, void *userdata);
typedef void (*router_signal_rx_callback_t)(router_ctx_t *ctx, const can_message_t *message, void *userdata);

typedef struct {
    router_signal_rx_callback_t signal_rx_callback;
    router_error_callback_t error_callback;
    void *callback_userdata;
}router_init_ctx_t;

typedef struct {
    bool active;
    const router_config_can_isotp_t *config;

    can_ctx_t *can_ctx;
    isotp_ctx_t *isotp_ctx;
    uds_ctx_t *uds_ctx;
    obd2_ctx_t *obd2_ctx;
    router_ctx_t *router_ctx;

    uint8_t upstream_data[ISOTP_PAYLOAD_LEN_MAX];
    uint16_t upstream_data_len;

    uint8_t downstream_data[ISOTP_PAYLOAD_LEN_MAX];
    uint16_t downstream_data_len;

    struct {
        can_message_t message_downstream;
        bool message_downstream_pending;

        bool isotp_upstream_pending;
        bool obd2_upstream_pending;
        bool uds_upstream_pending;

        bool isotp_downstream_pending;
    }runtime;
}router_diag_isotp_ctx_t;

typedef struct {
    bool active;
    const router_config_kwp_t *config;

    kwp_ctx_t *kwp_ctx;
    uds_ctx_t *uds_ctx;
    obd2_ctx_t *obd2_ctx;
    router_ctx_t *router_ctx;
}router_diag_kwp_ctx_t;

typedef struct {
    router_diag_isotp_ctx_t isotp_ctx[ECU_COMM_ISOTP_MAX];
    router_diag_kwp_ctx_t kwp_ctx[ECU_COMM_KWP_MAX];

}router_diag_ctx_t;

typedef struct {
    signals_runtime_messages_fifo_item_t message;
    bool signal_message_pending;
}router_signals_downstream_signal_ctx_t;

typedef struct {
    signals_runtime_messages_fifo_item_t message;
    bool can_message_pending;
}router_signals_downstream_can_ctx_t;

typedef struct {
    router_signals_downstream_signal_ctx_t signals[ECU_COMM_SIGNALS_MAX];
    router_signals_downstream_can_ctx_t cans[ECU_COMM_CAN_MAX];
}router_signals_downstream_ctx_t;

typedef struct {
    const router_config_can_signals_list_item_t *config_signal_item;
    router_ctx_t *router_ctx;
    can_ctx_t *can_ctx;
    signals_runtime_messages_fifo_item_t message;
}router_signals_upstream_rx_callback_ctx_t;

typedef struct {
    router_signals_upstream_rx_callback_ctx_t rx_cb_ctx[ROUTER_SIGNAL_UPSTREAM_LIST_LENGTH];
}router_signals_upstream_ctx_t;

typedef struct {
    can_ctx_t *can_ctxs[ECU_COMM_CAN_MAX];
    signals_ctx_t *signals_ctxs[ECU_COMM_SIGNALS_MAX];

    router_signals_downstream_ctx_t downstream;
    router_signals_upstream_ctx_t upstream;

}router_signals_ctx_t;

typedef struct router_ctx_tag {
    router_config_t config;
    router_init_ctx_t init;
    bool initialized;
    bool configured;

    router_signals_ctx_t signals;
    router_diag_ctx_t diag;

    router_error_code_t error_code;
    bool reset_trigger;

    router_signal_rx_callback_t signal_rx_callback_func;
    void *signal_rx_callback_usrdata;

}router_ctx_t;

#endif /* COMMUNICATION_ROUTER_INC_ROUTER_TYPES_H_ */
