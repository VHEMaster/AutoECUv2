/*
 * can_types.h
 *
 *  Created on: Oct 7, 2025
 *      Author: VHEMaster
 */

#ifndef COMMUNICATION_CAN_INC_CAN_TYPES_H_
#define COMMUNICATION_CAN_INC_CAN_TYPES_H_

#include "common.h"

#define CAN_MESSAGE_PAYLOAD_LEN_MAX   8u
#define CAN_MESSAGE_EXTENDED_ID_FLAG  0x80000000u

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

typedef struct {
    uint32_t id;
    uint8_t len;
    uint8_t payload[CAN_MESSAGE_PAYLOAD_LEN_MAX];
}can_message_t;

#endif /* COMMUNICATION_CAN_INC_CAN_TYPES_H_ */
