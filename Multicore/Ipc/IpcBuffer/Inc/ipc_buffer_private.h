/*
 * ipc_buffer_private.h
 *
 *  Created on: 4 нояб. 2026 г.
 *      Author: VHEMaster
 */

#ifndef IPC_IPCBUFFER_INC_IPC_BUFFER_PRIVATE_H_
#define IPC_IPCBUFFER_INC_IPC_BUFFER_PRIVATE_H_

#include "ipc_buffer_types.h"

STATIC_INLINE uint32_t ipc_buffer_advance(uint32_t count, uint32_t position, uint32_t amount)
{
  uint32_t tail = count - position;

  return amount >= tail ? amount - tail : position + amount;
}

STATIC_INLINE void ipc_buffer_copy_out(void *destination, const void *memory, uint32_t itemsize, uint32_t count, uint32_t position, uint32_t amount)
{
  uint32_t first = MIN(amount, count - position);
  uint8_t *output = destination;
  const uint8_t *input = memory;

  memcpy(output, input + position * itemsize, first * itemsize);

  if(amount > first) {
    memcpy(output + first * itemsize, input, (amount - first) * itemsize);
  }
}

STATIC_INLINE void ipc_buffer_copy_in(void *memory, const void *source, uint32_t itemsize, uint32_t count, uint32_t position, uint32_t amount)
{
  uint32_t first = MIN(amount, count - position);
  uint8_t *output = memory;
  const uint8_t *input = source;

  memcpy(output + position * itemsize, input, first * itemsize);

  if(amount > first) {
    memcpy(output, input + first * itemsize, (amount - first) * itemsize);
  }
}

void ipc_buffer_copy_begin(ipc_buffer_copy_ctx_t *ctx, void *memory,
    const void *buffer, uint32_t itemsize, uint32_t count, uint32_t position,
    uint32_t amount, bool output, uint32_t memcpy_threshold,
    ipc_memcpy_cb_t memcpy_cb, void *memcpy_usrdata);

error_t ipc_buffer_copy_process(ipc_buffer_copy_ctx_t *ctx);


#endif /* IPC_IPCBUFFER_INC_IPC_BUFFER_PRIVATE_H_ */
