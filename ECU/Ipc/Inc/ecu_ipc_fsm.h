/*
 * ecu_ipc_fsm.h
 */

#ifndef ECU_IPC_INC_ECU_IPC_FSM_H_
#define ECU_IPC_INC_ECU_IPC_FSM_H_

#include "ecu_ipc_types.h"

void ecu_ipc_buffer_fsm(ecu_ipc_buffer_ctx_t *ctx);
void ecu_ipc_transfer_fsm(ecu_ipc_transfer_ctx_t *ctx);

#endif /* ECU_IPC_INC_ECU_IPC_FSM_H_ */
