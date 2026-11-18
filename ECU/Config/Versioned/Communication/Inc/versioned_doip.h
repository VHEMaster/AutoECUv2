/*
 * versioned_doip.h
 *
 *  Created on: Nov 18, 2026
 *      Author: VHEMaster
 */

#ifndef CONFIG_VERSIONED_COMMUNICATION_INC_VERSIONED_DOIP_H_
#define CONFIG_VERSIONED_COMMUNICATION_INC_VERSIONED_DOIP_H_

#include "common.h"
#include "time.h"

typedef enum {
  DOIP_CONFIG_VERSION_V1 = 0,
  DOIP_CONFIG_VERSION_MAX
}doip_config_versions_t;

typedef struct {
    bool enabled;
    uint8_t protocol_version;
    uint16_t port;
    uint16_t logical_address;
    uint16_t functional_address;
    uint16_t tester_address_start;
    uint16_t tester_address_end;
    uint8_t max_connections;
    uint8_t vin[17];
    uint8_t eid[6];
    uint8_t gid[6];
    uint32_t anouncement_amount;
    time_delta_us_t anouncement_period;
    time_delta_us_t initial_timeout;
    time_delta_us_t inactivity_timeout;
    time_delta_us_t alive_timeout;
    time_delta_us_t receive_timeout;

    uint32_t align ALIGNED_CACHE;
}doip_config_v1_t ALIGNED_CACHE;

typedef doip_config_v1_t doip_config_t;

#endif /* CONFIG_VERSIONED_COMMUNICATION_INC_VERSIONED_DOIP_H_ */
