/*
 * config_ipc_sharedmemory.h
 *
 *  Created on: 8 нояб. 2026 г.
 *      Author: VHEMaster
 */

#ifndef CONFIG_IPC_INC_CONFIG_IPC_SHAREDMEMORY_H_
#define CONFIG_IPC_INC_CONFIG_IPC_SHAREDMEMORY_H_

#include "config_ipc.h"
#include "ipc_sharedmemory.h"

error_t ecu_ipc_sharedmemory_init(ecu_ipc_sharedmemory_t instance, ipc_sharedmemory_ctx_t *ctx);

#endif /* CONFIG_IPC_INC_CONFIG_IPC_SHAREDMEMORY_H_ */
