/*
 * ipc_private.h
 *
 *  Created on: 4 нояб. 2026 г.
 *      Author: VHEMaster
 */

#ifndef IPC_INC_IPC_PRIVATE_H_
#define IPC_INC_IPC_PRIVATE_H_

#include "compiler.h"

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

#endif /* IPC_INC_IPC_PRIVATE_H_ */
