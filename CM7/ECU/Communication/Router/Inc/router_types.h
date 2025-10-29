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

typedef struct {
    router_signal_rx_callback_t signal_rx_callback;
    router_error_callback_t error_callback;
    void *callback_userdata;
}router_init_ctx_t;

typedef struct {
    bool active;
    can_ctx_t *can_ctx;
    isotp_ctx_t *isotp_ctx;
    const router_config_can_isotp_pair_t *pair_config;

    can_message_t message_downstream;
    bool message_downstream_pending;
}router_diag_can_isotp_pair_ctx_t;

typedef struct {
    bool active;
    isotp_ctx_t *isotp_ctx;
    uds_ctx_t *uds_ctx;
    const router_config_isotp_uds_pair_t *pair_config;
}router_diag_isotp_uds_pair_ctx_t;

typedef struct {
    bool active;
    kwp_ctx_t *kwp_ctx;
    uds_ctx_t *uds_ctx;
    const router_config_kwp_uds_pair_t *pair_config;
}router_diag_kwp_uds_pair_ctx_t;

typedef struct {
    bool active;
    isotp_ctx_t *isotp_ctx;
    obd2_ctx_t *obd2_ctx;
    const router_config_isotp_obd2_pair_t *pair_config;
}router_diag_isotp_obd2_pair_ctx_t;

typedef struct {
    bool active;
    kwp_ctx_t *kwp_ctx;
    obd2_ctx_t *obd2_ctx;
    const router_config_kwp_obd2_pair_t *pair_config;
}router_diag_kwp_obd2_pair_ctx_t;


typedef struct {
    router_diag_can_isotp_pair_ctx_t can_isotp_pairs[ROUTER_CONFIG_CAN_ISOTP_PAIR_MAX];
    router_diag_isotp_uds_pair_ctx_t isotp_uds_pairs[ROUTER_CONFIG_ISOTP_UDS_PAIR_MAX];
    router_diag_kwp_uds_pair_ctx_t kwp_uds_pairs[ROUTER_CONFIG_KWP_UDS_PAIR_MAX];
    router_diag_isotp_obd2_pair_ctx_t isotp_obd2_pairs[ROUTER_CONFIG_ISOTP_OBD2_PAIR_MAX];
    router_diag_kwp_obd2_pair_ctx_t kwp_obd2_pairs[ROUTER_CONFIG_KWP_OBD2_PAIR_MAX];

}router_diag_ctx_t;

typedef struct router_ctx_tag {
    router_config_t config;
    router_init_ctx_t init;
    bool initialized;
    bool configured;

    router_diag_ctx_t diag;

    router_error_code_t error_code;
    bool reset_trigger;

    router_signal_rx_callback_t signal_rx_callback_func;
    void *signal_rx_callback_usrdata;

}router_ctx_t;

#endif /* COMMUNICATION_ROUTER_INC_ROUTER_TYPES_H_ */
