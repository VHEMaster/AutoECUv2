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

error_t ipc_rpc_caller_set_memcpy(ipc_rpc_caller_ctx_t *ctx, uint32_t threshold_bytes,
    ipc_memcpy_cb_t memcpy_cb, void *memcpy_usrdata);

error_t ipc_rpc_caller_lock(ipc_rpc_caller_ctx_t *ctx);
error_t ipc_rpc_caller_unlock(ipc_rpc_caller_ctx_t *ctx);

error_t ipc_rpc_caller_get_args_buffer(ipc_rpc_caller_ctx_t *ctx, uint32_t *p_args_bytes, void **args);
error_t ipc_rpc_caller_call(ipc_rpc_caller_ctx_t *ctx, uint32_t func_id, uint32_t args_bytes, const void *args);
/* Read the response before unlocking or starting another request. */
error_t ipc_rpc_caller_poll(ipc_rpc_caller_ctx_t *ctx, error_t *p_ret,
    uint32_t *p_resp_count, uint32_t *p_resp_bytes, const void **p_resp);
error_t ipc_rpc_caller_abort(ipc_rpc_caller_ctx_t *ctx);

#endif /* IPC_IPCRPC_INC_IPC_RPC_CALLER_H_ */
