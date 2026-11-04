/*
 * ipc_buffer_private.h
 *
 *  Created on: 4 нояб. 2026 г.
 *      Author: VHEMaster
 */

#ifndef IPC_IPCBUFFER_INC_IPC_BUFFER_PRIVATE_H_
#define IPC_IPCBUFFER_INC_IPC_BUFFER_PRIVATE_H_

#include "ipc_buffer_types.h"

uint32_t ipc_buffer_advance(uint32_t count, uint32_t position, uint32_t amount);

void ipc_buffer_copy_out(void *destination, const void *memory, uint32_t itemsize, uint32_t count, uint32_t position, uint32_t amount);
void ipc_buffer_copy_in(void *memory, const void *source, uint32_t itemsize, uint32_t count, uint32_t position, uint32_t amount);

void ipc_buffer_copy_begin(ipc_buffer_copy_ctx_t *ctx, void *memory,
    const void *buffer, uint32_t itemsize, uint32_t count, uint32_t position,
    uint32_t amount, bool output, ipc_buffer_memcpy_cb_t memcpy_cb, void *memcpy_usrdata);

error_t ipc_buffer_copy_process(ipc_buffer_copy_ctx_t *ctx);


#endif /* IPC_IPCBUFFER_INC_IPC_BUFFER_PRIVATE_H_ */
