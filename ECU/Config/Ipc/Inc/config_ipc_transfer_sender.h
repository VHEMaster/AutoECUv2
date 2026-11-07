/*
 * config_ipc_transfer_sender.h
 *
 *  Created on: 8 нояб. 2026 г.
 *      Author: VHEMaster
 */

#ifndef CONFIG_IPC_INC_CONFIG_IPC_TRANSFER_SENDER_H_
#define CONFIG_IPC_INC_CONFIG_IPC_TRANSFER_SENDER_H_

#include "config_ipc.h"
#include "ipc_transfer_sender.h"

error_t ecu_ipc_transfer_sender_init(ecu_ipc_transfer_sender_t instance, ipc_transfer_sender_ctx_t *ctx);

#endif /* CONFIG_IPC_INC_CONFIG_IPC_TRANSFER_SENDER_H_ */
