/*
 * middlelayer_ipc.h
 *
 *  Created on: 7 нояб. 2026 г.
 *      Author: VHEMaster
 */

#ifndef MIDDLELAYER_INC_MIDDLELAYER_IPC_H_
#define MIDDLELAYER_INC_MIDDLELAYER_IPC_H_

#include "middlelayer.h"
#include "config_ipc.h"
#include "ipc.h"

void middlelayer_ipc_init(void);
void middlelayer_ipc_loop_main(void);
void middlelayer_ipc_loop_slow(void);
void middlelayer_ipc_loop_comm(void);
void middlelayer_ipc_loop_fast(void);

#endif /* MIDDLELAYER_INC_MIDDLELAYER_IPC_H_ */
