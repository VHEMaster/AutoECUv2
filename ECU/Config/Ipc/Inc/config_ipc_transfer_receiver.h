/*
 * config_ipc_transfer_receiver.h
 *
 *  Created on: 8 нояб. 2026 г.
 *      Author: VHEMaster
 */

#ifndef CONFIG_IPC_INC_CONFIG_IPC_TRANSFER_RECEIVER_H_
#define CONFIG_IPC_INC_CONFIG_IPC_TRANSFER_RECEIVER_H_

#include "config_ipc.h"
#include "ipc_transfer_receiver.h"

error_t ecu_ipc_transfer_receiver_init(ecu_ipc_transfer_receiver_t instance, ipc_transfer_receiver_ctx_t *ctx);

#endif /* CONFIG_IPC_INC_CONFIG_IPC_TRANSFER_RECEIVER_H_ */
