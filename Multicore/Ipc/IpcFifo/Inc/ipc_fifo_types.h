/*
 * ipc_fifo_types.h
 *
 *  Created on: 4 нояб. 2026 г.
 *      Author: VHEMaster
 */

#ifndef IPC_INC_IPC_FIFO_TYPES_H_
#define IPC_INC_IPC_FIFO_TYPES_H_

#include <stdint.h>
#include <string.h>
#include "bool.h"
#include "errors.h"
#include "compiler.h"

#define IPC_FIFO_UNIQUE_VAL     (0x1FBD1DF5u)

typedef struct {
    uint32_t unique_val;
    uint32_t read;
    uint32_t write;
}ipc_fifo_header_ctx_t;

typedef struct {
    uint32_t itemsize;
    uint32_t itemscount;
    uint32_t unique_val;
    uint32_t read;
    uint32_t write;
    ipc_fifo_header_ctx_t *header;
    const void *buffer;
    bool synchronized;
    bool lock;
}ipc_fifo_reader_ctx_t;

typedef struct {
    uint32_t itemsize;
    uint32_t itemscount;
    uint32_t unique_val;
    uint32_t read;
    uint32_t write;
    ipc_fifo_header_ctx_t *header;
    void *buffer;
    bool lock;
}ipc_fifo_writer_ctx_t;

#endif /* IPC_INC_IPC_FIFO_TYPES_H_ */
