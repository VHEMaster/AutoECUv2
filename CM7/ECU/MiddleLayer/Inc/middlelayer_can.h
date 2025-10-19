/*
 * middlelayer_can.h
 *
 *  Created on: Oct 19, 2025
 *      Author: VHEMaster
 */

#ifndef MIDDLELAYER_INC_MIDDLELAYER_CAN_H_
#define MIDDLELAYER_INC_MIDDLELAYER_CAN_H_

#include "middlelayer.h"
#include "config_can.h"
#include "can.h"

void middlelayer_can_init(void);
void middlelayer_can_loop_main(void);
void middlelayer_can_loop_slow(void);
void middlelayer_can_loop_fast(void);

error_t middlelayer_can_get_if(can_ctx_t **can_if, ecu_can_if_enum_t interface);

#endif /* MIDDLELAYER_INC_MIDDLELAYER_CAN_H_ */
