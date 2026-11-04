/*
 * ipc_buffer_types.h
 *
 *  Created on: 4 нояб. 2026 г.
 *      Author: VHEMaster
 */

#ifndef IPC_INC_IPC_BUFFER_TYPES_H_
#define IPC_INC_IPC_BUFFER_TYPES_H_

#include "ipc_fifo_types.h"

#define IPC_BUFFER_UNIQUE_VAL    (0x1FBD1DF5u)

typedef struct {
    uint32_t unique_val;
    ipc_fifo_header_ctx_t fifo_header;
    uint32_t read;
    uint32_t write;
}ipc_buffer_header_ctx_t;

typedef struct {
    uint32_t itemsize;
    uint32_t itemcount;
    uint32_t read;
    uint32_t write;
    ipc_buffer_header_ctx_t *header;
    ipc_fifo_reader_ctx_t fifo_ctx;
    const void *buffer;
}ipc_buffer_reader_ctx_t;

typedef struct {
    uint32_t itemsize;
    uint32_t itemcount;
    uint32_t read;
    uint32_t write;
    ipc_buffer_header_ctx_t *header;
    ipc_fifo_writer_ctx_t fifo_ctx;
    void *buffer;
}ipc_buffer_writer_ctx_t;

OPTIMIZE_FAST
STATIC_INLINE uint32_t ipc_buffer_get_used_to_read(uint32_t size, uint32_t read, uint32_t write)
{
  if (write >= read) {
    return write - read;
  } else {
    return size - (read - write);
  }
}

OPTIMIZE_FAST
STATIC_INLINE uint32_t ipc_buffer_get_free_to_write(uint32_t size, uint32_t read, uint32_t write)
{
  uint32_t used = ipc_buffer_get_used_to_read(size, read, write);

  return size - used - 1;
}

#endif /* IPC_INC_IPC_BUFFER_TYPES_H_ */
