/*
 * ipc_types.h
 *
 *  Created on: 4 нояб. 2026 г.
 *      Author: VHEMaster
 */

#ifndef IPC_INC_IPC_TYPES_H_
#define IPC_INC_IPC_TYPES_H_

#include <stdint.h>
#include "bool.h"
#include "errors.h"

typedef struct {
    uint32_t itemsize;
    uint32_t itemcount;
    uint32_t read;
    uint32_t write;
    const void *buffer;
}ipc_reader_ctx_t;

typedef struct {
    uint32_t itemsize;
    uint32_t itemcount;
    uint32_t read;
    uint32_t write;
    void *buffer;
}ipc_writer_ctx_t;

#endif /* IPC_INC_IPC_TYPES_H_ */
