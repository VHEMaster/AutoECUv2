/*
 * ipc_buffer_types.h
 *
 *  Created on: 4 нояб. 2026 г.
 *      Author: VHEMaster
 */

#ifndef IPC_INC_IPC_BUFFER_TYPES_H_
#define IPC_INC_IPC_BUFFER_TYPES_H_

#include "ipc_fifo_types.h"

#define IPC_BUFFER_UNIQUE_VAL    (0x5F3759DFu)

typedef struct {
    uint32_t offset;
    uint32_t size;
}ipc_buffer_descriptor_t;

typedef struct {
    uint32_t unique_val;
    ipc_fifo_header_ctx_t fifo_header;
    uint32_t read;
    uint32_t write;
}ipc_buffer_header_ctx_t;

typedef struct {
    uint32_t itemsize;
    uint32_t itemscount;
    uint32_t read;
    uint32_t write;
    ipc_buffer_header_ctx_t *header;
    ipc_fifo_reader_ctx_t fifo_ctx;
    const void *memorybuffer;
    const ipc_buffer_descriptor_t *descriptors;
    uint32_t unique_val;
    bool synchronized;
    bool lock;
    bool started;
    uint32_t size;
    uint32_t left;
}ipc_buffer_reader_ctx_t;

typedef struct {
    uint32_t itemsize;
    uint32_t itemscount;
    uint32_t read;
    uint32_t write;
    ipc_buffer_header_ctx_t *header;
    ipc_fifo_writer_ctx_t fifo_ctx;
    void *memorybuffer;
    ipc_buffer_descriptor_t *descriptors;
    bool lock;
    bool started;
    uint32_t size;
    uint32_t left;
}ipc_buffer_writer_ctx_t;

#endif /* IPC_INC_IPC_BUFFER_TYPES_H_ */
