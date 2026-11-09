/*
 * ipc_rpc_types.h
 *
 *  Created on: 9 нояб. 2026 г.
 *      Author: VHEMaster
 */

#ifndef IPC_IPCRPC_INC_IPC_RPC_TYPES_H_
#define IPC_IPCRPC_INC_IPC_RPC_TYPES_H_

#include <stdint.h>
#include "bool.h"
#include "errors.h"
#include "ipc_rpc_defines.h"

typedef struct {
    uint32_t unique_val;
    uint32_t request_id;
    uint32_t response_id;
    uint32_t func_id;
    uint32_t args_bytes;
    error_t status;
    error_t ret;
    bool aborted;
}ipc_rpc_header_ctx_t;

typedef struct {
    ipc_rpc_header_ctx_t *header;
    void *buffer;
    uint32_t buffer_size;

    ATOMIC_LOCK_TYPE lock;
    bool pending;
    uint32_t request_id;
}ipc_rpc_caller_ctx_t;

typedef struct {
    ipc_rpc_header_ctx_t *header;
    const void *buffer;
    uint32_t buffer_size;

    ATOMIC_LOCK_TYPE lock;
    bool pending;
    uint32_t request_id;
}ipc_rpc_executor_ctx_t;

#endif /* IPC_IPCRPC_INC_IPC_RPC_TYPES_H_ */
