/*
 * ipc_types.h
 *
 *  Created on: 4 нояб. 2026 г.
 *      Author: VHEMaster
 */

#ifndef IPC_INC_IPC_TYPES_H_
#define IPC_INC_IPC_TYPES_H_

#include <stdint.h>
#include <string.h>
#include "bool.h"
#include "errors.h"
#include "compiler.h"

#define IPC_UNIQUE_VAL    (0x1FBD1DF5u)

typedef struct {
    uint32_t unique_val;
    uint32_t read;
    uint32_t write;
}ipc_header_ctx_t;

typedef struct {
    uint32_t itemsize;
    uint32_t itemcount;
    uint32_t unique_val;
    uint32_t read;
    uint32_t write;
    ipc_header_ctx_t *header;
    const void *buffer;
    bool synchronized;
}ipc_reader_ctx_t;

typedef struct {
    uint32_t itemsize;
    uint32_t itemcount;
    uint32_t unique_val;
    uint32_t read;
    uint32_t write;
    ipc_header_ctx_t *header;
    void *buffer;
}ipc_writer_ctx_t;

OPTIMIZE_FAST
STATIC_INLINE uint32_t ipc_get_used_to_read(uint32_t size, uint32_t read, uint32_t write)
{
  if (write >= read) {
    return write - read;
  } else {
    return size - (read - write);
  }
}

OPTIMIZE_FAST
STATIC_INLINE uint32_t ipc_get_free_to_write(uint32_t size, uint32_t read, uint32_t write)
{
  uint32_t used = ipc_get_used_to_read(size, read, write);

  return size - used - 1;
}

#endif /* IPC_INC_IPC_TYPES_H_ */
