/*
 * ipc_sharedmemory_types.h
 *
 *  Created on: 8 нояб. 2026 г.
 *      Author: VHEMaster
 */

#ifndef IPC_INC_IPC_SHAREDMEMORY_TYPES_H_
#define IPC_INC_IPC_SHAREDMEMORY_TYPES_H_

#include <string.h>
#include "compiler.h"
#include "errors.h"
#include "bool.h"

typedef struct {
    uint32_t size;
    void *buffer;
    ATOMIC_LOCK_TYPE lock;
}ipc_sharedmemory_ctx_t;

#endif /* IPC_INC_IPC_SHAREDMEMORY_TYPES_H_ */
