/*
 * ipc_buffer_private.c
 *
 *  Created on: 4 нояб. 2026 г.
 *      Author: VHEMaster
 */

#include "ipc_buffer_private.h"

uint32_t ipc_buffer_advance(uint32_t count, uint32_t position, uint32_t amount)
{
  uint32_t tail = count - position;

  return amount >= tail ? amount - tail : position + amount;
}

void ipc_buffer_copy_out(void *destination, const void *memory, uint32_t itemsize, uint32_t count, uint32_t position, uint32_t amount)
{
  uint32_t first = MIN(amount, count - position);

  memcpy(destination, memory + position * itemsize, first * itemsize);

  if(amount > first) {
    memcpy(destination + first * itemsize, memory, (amount - first) * itemsize);
  }
}

void ipc_buffer_copy_in(void *memory, const void *source, uint32_t itemsize, uint32_t count, uint32_t position, uint32_t amount)
{
  uint32_t first = MIN(amount, count - position);

  memcpy(memory + position * itemsize, source,
      first * itemsize);

  if(amount > first) {
    memcpy(memory, source + first * itemsize, (amount - first) * itemsize);
  }
}
