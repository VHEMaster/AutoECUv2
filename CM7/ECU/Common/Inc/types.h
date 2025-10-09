/*
 * types.h
 *
 *  Created on: Jun 8, 2024
 *      Author: VHEMaster
 */

#ifndef COMMON_INC_TYPES_H_
#define COMMON_INC_TYPES_H_

#define ECU_CAN_MESSAGE_PAYLOAD_LEN   8

typedef enum {
  ECU_BANK_1 = 0,
  ECU_BANK_2,
  ECU_BANK_MAX,
}ecu_bank_t;

typedef enum {
  ECU_BANK_BIT_NONE = 0,
  ECU_BANK_BIT_1 = 1,
  ECU_BANK_BIT_2 = 2,
  ECU_BANK_BIT_ALL = ECU_BANK_BIT_1 | ECU_BANK_BIT_2,
}ecu_bank_bit_t;

typedef enum {
  ECU_CYLINDER_1 = 0,
  ECU_CYLINDER_2,
  ECU_CYLINDER_3,
  ECU_CYLINDER_4,
  ECU_CYLINDER_5,
  ECU_CYLINDER_6,
  ECU_CYLINDER_7,
  ECU_CYLINDER_8,
  ECU_CYLINDER_MAX,
}ecu_cylinder_t;

#endif /* COMMON_INC_TYPES_H_ */
