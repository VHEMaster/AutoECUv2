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

typedef enum {
  ROUTER_OK = 0,
  ROUTER_MAX
}router_error_code_t;

typedef struct router_ctx_tag router_ctx_t;

typedef void (*router_error_callback_t)(router_ctx_t *ctx, router_error_code_t code, void *userdata);
typedef void (*router_signal_rx_callback_t)(router_ctx_t *ctx, const can_message_t *message, void *userdata);

#define ROUTER_SIGNAL_TX_MESSAGES_MAX   4u
#define ROUTER_SIGNAL_TX_ITEMS_MAX      4u

typedef struct {
    ecu_config_parameter_id_t parameter_id;
    float multiplier;
    float offset;
    uint8_t byte_offset;
}router_signal_tx_item_t;

typedef struct {
    bool enabled;
    uint32_t message_id;
    time_delta_us_t period;
    uint8_t signals_count;
    router_signal_tx_item_t signals[ROUTER_SIGNAL_TX_ITEMS_MAX];
}router_signal_tx_message_config_t;

typedef struct {
    uint8_t messages_count;
    const router_signal_tx_message_config_t *messages;
}router_signal_tx_config_t;

typedef struct {
    router_signal_rx_callback_t signal_rx_callback;
    router_error_callback_t error_callback;
    void *callback_userdata;
    const router_signal_tx_config_t *signals_tx;
}router_init_ctx_t;

typedef struct {
    bool active;
    const router_config_can_isotp_t *config;

    can_ctx_t *can_ctx;
    isotp_ctx_t *isotp_ctx;
    uds_ctx_t *uds_ctx;
    obd2_ctx_t *obd2_ctx;

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
}router_diag_kwp_ctx_t;

typedef struct {
    router_diag_isotp_ctx_t isotp_ctx[ECU_COMM_ISOTP_MAX];
    router_diag_kwp_ctx_t kwp_ctx[ECU_COMM_KWP_MAX];

}router_diag_ctx_t;

typedef struct router_ctx_tag {
    router_config_t config;
    router_init_ctx_t init;
    bool initialized;
    bool configured;

    router_diag_ctx_t diag;

    struct {
        time_us_t last_sent[ROUTER_SIGNAL_TX_MESSAGES_MAX];
        bool started[ROUTER_SIGNAL_TX_MESSAGES_MAX];
        can_message_t pending[ROUTER_SIGNAL_TX_MESSAGES_MAX];
        bool pending_valid[ROUTER_SIGNAL_TX_MESSAGES_MAX];
    }signals;

    router_error_code_t error_code;
    bool reset_trigger;

    router_signal_rx_callback_t signal_rx_callback_func;
    void *signal_rx_callback_usrdata;

}router_ctx_t;

#endif /* COMMUNICATION_ROUTER_INC_ROUTER_TYPES_H_ */
