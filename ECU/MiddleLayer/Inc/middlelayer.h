/*
 * middlelayer.h
 *
 *  Created on: Apr 3, 2024
 *      Author: VHEMaster
 */

#ifndef MIDDLELAYER_INC_MIDDLELAYER_H_
#define MIDDLELAYER_INC_MIDDLELAYER_H_

#include "common.h"

typedef enum {
  MIDDLELAYER_FAULT_TYPE_HARDFAULT = 0,
  MIDDLELAYER_FAULT_TYPE_MEMMANAGE,
  MIDDLELAYER_FAULT_TYPE_BUSFAULT,
  MIDDLELAYER_FAULT_TYPE_USAGEFAULT,
  MIDDLELAYER_FAULT_TYPE_NMI
}middlelayer_fault_type_t;

void middlelayer_ll_init(void);
void middlelayer_init(void);
void middlelayer_loop(void);

void middlelayer_fault(middlelayer_fault_type_t fault_type);

#endif /* MIDDLELAYER_INC_MIDDLELAYER_H_ */
