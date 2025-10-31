/*
 * versioned_router.h
 *
 *  Created on: Oct 23, 2025
 *      Author: VHEMaster
 */

#ifndef CONFIG_VERSIONED_COMMUNICATION_INC_VERSIONED_ROUTER_H_
#define CONFIG_VERSIONED_COMMUNICATION_INC_VERSIONED_ROUTER_H_

#include "common.h"
#include "config_comm_types.h"

#define ROUTER_SIGNAL_UPSTREAM_LIST_LENGTH    (16)
#define ROUTER_SIGNAL_DOWNSTREAM_LIST_LENGTH  (16)

typedef enum {
  ROUTER_CONFIG_VERSION_V1 = 0,
  ROUTER_CONFIG_VERSION_MAX
}router_config_versions_t;

typedef enum {
  ROUTER_CONFIG_CAN_SIGNAL_MSGID_TYPE_RANGE = 0,
  ROUTER_CONFIG_CAN_SIGNAL_MSGID_TYPE_SIGNAL,
  ROUTER_CONFIG_CAN_SIGNAL_MSGID_TYPE_DUAL,
  ROUTER_CONFIG_CAN_SIGNAL_MSGID_TYPE_MAX
}router_config_can_signal_msgid_type_t;

typedef struct {
    bool enabled;
    uint32_t msg_id_1_start;
    uint32_t msg_id_2_end;
    ecu_comm_can_t can_instance;
}router_config_can_signals_list_item_t;

typedef struct {
    router_config_can_signals_list_item_t upstream_list[ROUTER_SIGNAL_UPSTREAM_LIST_LENGTH];
    router_config_can_signals_list_item_t downstream_list[ROUTER_SIGNAL_DOWNSTREAM_LIST_LENGTH];
}router_config_can_signals_t;

typedef struct {
    bool enabled;
    ecu_comm_can_t can_instance;
    ecu_comm_uds_t uds_instance;
    ecu_comm_obd2_t obd2_instance;
    uint32_t upstream_phy_msg_id;
    uint32_t upstream_func_msg_id;
    uint32_t downstream_msg_id;
}router_config_can_isotp_t;

typedef struct {
    bool enabled;
    ecu_comm_obd2_t obd2_instance;
    ecu_comm_uds_t uds_instance;
}router_config_kwp_t;

typedef struct {
    router_config_can_signals_t signals;

}router_config_can_t;

typedef struct {
    router_config_can_isotp_t can_isotp[ECU_COMM_ISOTP_MAX];
    router_config_kwp_t kwp[ECU_COMM_KWP_MAX];

}router_config_diag_t;

typedef struct {
    router_config_can_t can;
    router_config_diag_t diagnostics;

    uint32_t align ALIGNED_CACHE;
}router_config_v1_t ALIGNED_CACHE;

typedef router_config_v1_t router_config_t;

#endif /* CONFIG_VERSIONED_COMMUNICATION_INC_VERSIONED_ROUTER_H_ */
