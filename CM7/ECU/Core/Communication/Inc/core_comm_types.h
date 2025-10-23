/*
 * core_comm_types.h
 *
 *  Created on: Oct 23, 2025
 *      Author: VHEMaster
 */

#ifndef CORE_COMMUNICATION_INC_CORE_COMM_TYPES_H_
#define CORE_COMMUNICATION_INC_CORE_COMM_TYPES_H_

#include "common.h"
#include "isotp_types.h"

typedef enum {
  COMM_ISOTP_CHANNEL_OBD = 0,
  COMM_ISOTP_CHANNEL_MAX
}ecu_core_comm_isotp_channel_t;

typedef struct {
    isotp_ctx_t isotp[COMM_ISOTP_CHANNEL_MAX];
}ecu_core_comm_ctx_t;

#endif /* CORE_COMMUNICATION_INC_CORE_COMM_TYPES_H_ */
