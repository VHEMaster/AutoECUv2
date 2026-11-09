/*
 * ipc_rpc_caller.h
 *
 *  Created on: 9 нояб. 2026 г.
 *      Author: VHEMaster
 */

#ifndef IPC_IPCRPC_INC_IPC_RPC_CALLER_H_
#define IPC_IPCRPC_INC_IPC_RPC_CALLER_H_

#include "ipc_rpc_types.h"

error_t ipc_rpc_caller_init(ipc_rpc_caller_ctx_t *ctx, ipc_rpc_header_ctx_t *header, uint32_t buffer_size, void *buffer);

error_t ipc_rpc_caller_lock(ipc_rpc_caller_ctx_t *ctx);
error_t ipc_rpc_caller_unlock(ipc_rpc_caller_ctx_t *ctx);

error_t ipc_rpc_caller_call(ipc_rpc_caller_ctx_t *ctx, uint32_t func_id, uint32_t args_bytes, const void *args);
error_t ipc_rpc_caller_poll(ipc_rpc_caller_ctx_t *ctx, error_t *p_ret);
error_t ipc_rpc_caller_abort(ipc_rpc_caller_ctx_t *ctx);

#endif /* IPC_IPCRPC_INC_IPC_RPC_CALLER_H_ */
