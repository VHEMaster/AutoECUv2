/*
 * ipc_rpc_executor.h
 *
 *  Created on: 9 нояб. 2026 г.
 *      Author: VHEMaster
 */

#ifndef IPC_IPCRPC_INC_IPC_RPC_EXECUTOR_H_
#define IPC_IPCRPC_INC_IPC_RPC_EXECUTOR_H_

#include "ipc_rpc_types.h"

error_t ipc_rpc_executor_init(ipc_rpc_executor_ctx_t *ctx, ipc_rpc_header_ctx_t *header, uint32_t buffer_size, void *buffer);

error_t ipc_rpc_executor_lock(ipc_rpc_executor_ctx_t *ctx);
error_t ipc_rpc_executor_unlock(ipc_rpc_executor_ctx_t *ctx);

error_t ipc_rpc_executor_poll(ipc_rpc_executor_ctx_t *ctx, uint32_t *p_func_id,
    uint32_t *p_args_bytes, const void **args, uint32_t *p_resp_capacity, void **p_resp);
error_t ipc_rpc_executor_return(ipc_rpc_executor_ctx_t *ctx, error_t ret, uint32_t resp_count, uint32_t resp_bytes);

#endif /* IPC_IPCRPC_INC_IPC_RPC_EXECUTOR_H_ */
