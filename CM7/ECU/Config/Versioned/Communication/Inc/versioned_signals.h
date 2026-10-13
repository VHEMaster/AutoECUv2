/*
 * versioned_signals.h
 *
 *  Created on: 13 окт. 2026 г.
 *      Author: VHEMaster
 */

#ifndef CONFIG_VERSIONED_COMMUNICATION_INC_VERSIONED_SIGNALS_H_
#define CONFIG_VERSIONED_COMMUNICATION_INC_VERSIONED_SIGNALS_H_

#include "common.h"
#include "config_common_types.h"
#include "config_comm_types.h"
#include "can_types.h"
#include "time.h"

#define SIGNALS_CONFIG_SIGNALS_UPSTREAM_LIST_SIZE_MAX     64u
#define SIGNALS_CONFIG_SIGNALS_DOWNSTREAM_LIST_SIZE_MAX   128u

#define SIGNALS_CONFIG_MESSAGES_UPSTREAM_LIST_SIZE_MAX    24u
#define SIGNALS_CONFIG_MESSAGES_DOWNSTREAM_LIST_SIZE_MAX  64u

typedef enum {
  SIGNALS_CONFIG_VERSION_V1 = 0,
  SIGNALS_CONFIG_VERSION_MAX
}signals_config_versions_t;

typedef struct {
    bool enabled;
    uint32_t message_id;
    can_signal_description_t description;
    ecu_config_parameter_id_t parameter_id;
}signals_config_signal_description_t;

typedef struct {
    bool enabled;
    uint32_t message_id;
    uint8_t message_len;
    time_delta_us_t upstream_timeout;
    time_delta_us_t downstream_periodicity;
    bool has_counter;
    bool has_crc;
    uint8_t counter_mask;
    uint8_t crc_poly;
    uint8_t counter_byte_pos;
    uint8_t crc_byte_pos;
}signals_config_message_description_t;

typedef struct {
    signals_config_signal_description_t upstream_list[SIGNALS_CONFIG_SIGNALS_UPSTREAM_LIST_SIZE_MAX];
    signals_config_signal_description_t downstream_list[SIGNALS_CONFIG_SIGNALS_DOWNSTREAM_LIST_SIZE_MAX];
}signals_config_signals_lists_t;

typedef struct {
    signals_config_message_description_t upstream_list[SIGNALS_CONFIG_MESSAGES_UPSTREAM_LIST_SIZE_MAX];
    signals_config_message_description_t downstream_list[SIGNALS_CONFIG_MESSAGES_DOWNSTREAM_LIST_SIZE_MAX];
}signals_config_messages_lists_t;

typedef struct {
    bool enabled;

    signals_config_messages_lists_t messages_lists;
    signals_config_signals_lists_t signals_lists;

    uint32_t align ALIGNED_CACHE;
}signals_config_v1_t ALIGNED_CACHE;

typedef signals_config_v1_t signals_config_t;

#endif /* CONFIG_VERSIONED_COMMUNICATION_INC_VERSIONED_SIGNALS_H_ */
