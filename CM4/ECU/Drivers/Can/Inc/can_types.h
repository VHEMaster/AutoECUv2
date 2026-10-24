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

typedef struct {
    uint32_t id;
    uint8_t len;
    uint8_t payload[CAN_MESSAGE_PAYLOAD_LEN_MAX];
}can_message_t;

typedef enum {
  CAN_SIGNAL_BYTEORDER_INTEL = 0,
  CAN_SIGNAL_BYTEORDER_MOTOROLA,
  CAN_SIGNAL_BYTEORDER_MAX
}can_signal_byteorder_t;

typedef enum {
  CAN_SIGNAL_TYPE_UINT = 0,
  CAN_SIGNAL_TYPE_SINT,
  CAN_SIGNAL_TYPE_UFLT,
  CAN_SIGNAL_TYPE_SFLT,
  CAN_SIGNAL_TYPE_MAX
}can_signal_type_t;

typedef struct {
    can_signal_byteorder_t byteorder;
    can_signal_type_t type;
    uint8_t bitlength;
    uint16_t startbit;
    float offset;
    float gain;
}can_signal_description_t;

typedef struct {
    uint32_t raw_value;
    float phy_value;
}can_signal_value_t;

typedef struct {
    can_signal_description_t description;
    can_signal_value_t value;
}can_signal_t;

#endif /* COMMUNICATION_CAN_INC_CAN_TYPES_H_ */
