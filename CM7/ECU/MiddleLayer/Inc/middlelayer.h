/*
 * middlelayer.h
 *
 *  Created on: Apr 3, 2024
 *      Author: VHEMaster
 */

#ifndef MIDDLELAYER_INC_MIDDLELAYER_H_
#define MIDDLELAYER_INC_MIDDLELAYER_H_

#include "main.h"

#define MIDDLELAYER_TIMEBASE_MASK 0x03FFFFFF

void middlelayer_init(void);
void middlelayer_ll_init(void);
void middlelayer_loop(void);

#endif /* MIDDLELAYER_INC_MIDDLELAYER_H_ */
