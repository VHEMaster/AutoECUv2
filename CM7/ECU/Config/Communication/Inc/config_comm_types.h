/*
 * config_comm_types.h
 *
 *  Created on: Oct 23, 2025
 *      Author: vhemaster
 */

#ifndef CONFIG_COMMUNICATION_INC_CONFIG_COMM_TYPES_H_
#define CONFIG_COMMUNICATION_INC_CONFIG_COMM_TYPES_H_

#include "common.h"

#define ECU_COMM_NONE           (255)
#define ECU_COMME_INSTANCE_MAX  (2)

typedef uint32_t ecu_comm_instance_t;

typedef enum {
  ECU_COMM_TYPE_CAN = 0,
  ECU_COMM_TYPE_ISOTP,
  ECU_COMM_TYPE_MAX
}ecu_comm_type_t;

typedef enum {
  ECU_COMM_CAN_1 = 0,
  ECU_COMM_CAN_MAX
}ecu_comm_can_t;

typedef enum {
  ECU_COMM_ISOTP_1 = 0,
  ECU_COMM_ISOTP_MAX
}ecu_comm_isotp_t;

#endif /* CONFIG_COMMUNICATION_INC_CONFIG_COMM_TYPES_H_ */
