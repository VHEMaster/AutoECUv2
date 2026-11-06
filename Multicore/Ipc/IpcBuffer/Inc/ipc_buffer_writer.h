/*
 * ipc_buffer_writer.h
 *
 *  Created on: 4 нояб. 2026 г.
 *      Author: VHEMaster
 */

#ifndef IPC_INC_IPC_BUFFER_WRITER_H_
#define IPC_INC_IPC_BUFFER_WRITER_H_

#include "ipc_buffer_types.h"

error_t ipc_buffer_writer_init(ipc_buffer_writer_ctx_t *ctx, uint32_t itemsize, uint32_t itemscount, uint32_t descriptorscount, ipc_buffer_header_ctx_t *header, ipc_buffer_descriptor_t *descriptors, void *memorybuffer);
error_t ipc_buffer_writer_set_memcpy(ipc_buffer_writer_ctx_t *ctx, uint32_t threshold_bytes, ipc_buffer_memcpy_cb_t memcpy_cb, void *memcpy_usrdata);

error_t ipc_buffer_writer_lock(ipc_buffer_writer_ctx_t *ctx);
error_t ipc_buffer_writer_unlock(ipc_buffer_writer_ctx_t *ctx);

error_t ipc_buffer_writer_push(ipc_buffer_writer_ctx_t *ctx, const void *buffer, uint32_t itemscount);
error_t ipc_buffer_writer_begin(ipc_buffer_writer_ctx_t *ctx, uint32_t itemscount);
error_t ipc_buffer_writer_write(ipc_buffer_writer_ctx_t *ctx, const void *buffer, uint32_t itemscount);
error_t ipc_buffer_writer_commit(ipc_buffer_writer_ctx_t *ctx);
error_t ipc_buffer_writer_cancel(ipc_buffer_writer_ctx_t *ctx);

error_t ipc_buffer_writer_get_memory_used(ipc_buffer_writer_ctx_t *ctx, uint32_t *p_used_items);
error_t ipc_buffer_writer_get_memory_free(ipc_buffer_writer_ctx_t *ctx, uint32_t *p_free_items);

error_t ipc_buffer_writer_get_buffers_used(ipc_buffer_writer_ctx_t *ctx, uint32_t *p_used_buffers);
error_t ipc_buffer_writer_get_buffers_free(ipc_buffer_writer_ctx_t *ctx, uint32_t *p_free_buffers);

error_t ipc_buffer_writer_is_memory_full(ipc_buffer_writer_ctx_t *ctx, bool *p_is_full);
error_t ipc_buffer_writer_is_memory_empty(ipc_buffer_writer_ctx_t *ctx, bool *p_is_empty);

error_t ipc_buffer_writer_is_buffers_full(ipc_buffer_writer_ctx_t *ctx, bool *p_is_full);
error_t ipc_buffer_writer_is_buffers_empty(ipc_buffer_writer_ctx_t *ctx, bool *p_is_empty);

error_t ipc_buffer_writer_is_full(ipc_buffer_writer_ctx_t *ctx, bool *p_is_full);
error_t ipc_buffer_writer_is_empty(ipc_buffer_writer_ctx_t *ctx, bool *p_is_empty);

#endif /* IPC_INC_IPC_BUFFER_WRITER_H_ */
