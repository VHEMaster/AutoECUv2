/*
 * ipc_sharedmemory.h
 *
 *  Created on: 8 нояб. 2026 г.
 *      Author: VHEMaster
 */

#ifndef IPC_INC_IPC_SHAREDMEMORY_H_
#define IPC_INC_IPC_SHAREDMEMORY_H_

#include "ipc_sharedmemory_types.h"

error_t ipc_sharedmemory_init(ipc_sharedmemory_ctx_t *ctx, uint32_t bytes, void *buffer);

error_t ipc_sharedmemory_lock(ipc_sharedmemory_ctx_t *ctx);
error_t ipc_sharedmemory_unlock(ipc_sharedmemory_ctx_t *ctx);

error_t ipc_sharedmemory_get_buffer(ipc_sharedmemory_ctx_t *ctx, void **buffer, uint32_t *bytes);

#endif /* IPC_INC_IPC_SHAREDMEMORY_H_ */
