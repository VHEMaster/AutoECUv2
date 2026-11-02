/*
 * middlelayer.h
 *
 *  Created on: Apr 3, 2024
 *      Author: VHEMaster
 */

#ifndef MIDDLELAYER_INC_MIDDLELAYER_H_
#define MIDDLELAYER_INC_MIDDLELAYER_H_

#include "common.h"
#include "middlelayer_types.h"

void middlelayer_ll_init(void);
void middlelayer_init(void);
void middlelayer_start(void);
void middlelayer_loop(void);

void middlelayer_fault(middlelayer_fault_type_t fault_type);

#endif /* MIDDLELAYER_INC_MIDDLELAYER_H_ */
