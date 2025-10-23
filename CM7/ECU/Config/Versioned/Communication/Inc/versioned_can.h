/*
 * versioned_can.h
 *
 *  Created on: Oct 23, 2025
 *      Author: VHEMaster
 */

#ifndef CONFIG_VERSIONED_COMMUNICATION_INC_VERSIONED_CAN_H_
#define CONFIG_VERSIONED_COMMUNICATION_INC_VERSIONED_CAN_H_

#include "common.h"

typedef enum {
  CAN_CONFIG_VERSION_V1 = 0,
  CAN_CONFIG_VERSION_MAX
}can_config_versions_t;

#define CAN_RX_FILTER_MAX (16)

typedef enum {
  CAN_BAUDRATE_500KBPS = 0,
  CAN_BAUDRATE_1MBPS,
  CAN_BAUDRATE_250KBPS,
  CAN_BAUDRATE_125KBPS,
  CAN_BAUDRATE_MAX,
}can_baudrate_t;

typedef enum {
  CAN_FIFO_0 = 0,
  CAN_FIFO_1,
  CAN_FIFO_MAX
}can_fifo_t;

typedef enum {
  CAN_FILTER_CONFIG_DISABLE = 0,
  CAN_FILTER_CONFIG_TO_RXFIFO0,
  CAN_FILTER_CONFIG_TO_RXFIFO1,
  CAN_FILTER_CONFIG_REJECT,
  CAN_FILTER_CONFIG_HP,
  CAN_FILTER_CONFIG_TO_RXFIFO0_HP,
  CAN_FILTER_CONFIG_TO_RXFIFO1_HP,
  CAN_FILTER_CONFIG_TO_RXBUFFER,
  CAN_FILTER_CONFIG_MAX
}ecu_config_filter_config_t;

typedef enum {
  CAN_FILTER_IDTYPE_STD = 0,
  CAN_FILTER_IDTYPE_EXT,
  CAN_FILTER_IDTYPE_MAX
}ecu_config_filter_idtype_t;

typedef enum {
  CAN_FILTER_TYPE_RANGE = 0,
  CAN_FILTER_TYPE_DUAL,
  CAN_FILTER_TYPE_MASK,
  CAN_FILTER_TYPE_MAX
}ecu_config_filter_type_t;

typedef struct {
    ecu_config_filter_config_t filter_config;
    ecu_config_filter_type_t filter_type;
    ecu_config_filter_idtype_t id_type;
    uint32_t rx_buffer_index;
    uint32_t id1_filter;
    uint32_t id2_mask;
}can_config_filter_t;

typedef struct {
    bool reject_remote_std;
    bool reject_remote_ext;
    bool reject_non_matching_std;
    bool reject_non_matching_ext;
    can_fifo_t non_matching_std_fifo;
    can_fifo_t non_matching_ext_fifo;
}can_cfg_global_filter_t;

typedef struct {
    can_baudrate_t baudrate;
    can_cfg_global_filter_t global_filter;
    can_config_filter_t filter_config[CAN_RX_FILTER_MAX];
    uint32_t align ALIGNED_CACHE;
}can_config_v1_t ALIGNED_CACHE;

typedef can_config_v1_t can_config_t;

#endif /* CONFIG_VERSIONED_COMMUNICATION_INC_VERSIONED_CAN_H_ */
