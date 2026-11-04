/*
 * ipc_buffer_reader.h
 *
 *  Created on: 4 нояб. 2026 г.
 *      Author: VHEMaster
 */

#ifndef IPC_INC_IPC_BUFFER_READER_H_
#define IPC_INC_IPC_BUFFER_READER_H_

#include "ipc_buffer_types.h"

error_t ipc_buffer_reader_init(ipc_buffer_reader_ctx_t *ctx, uint32_t itemsize, uint32_t itemscount, uint32_t descriptorscount, ipc_buffer_header_ctx_t *header, const ipc_buffer_descriptor_t *descriptors, const void *memorybuffer);

error_t ipc_buffer_reader_lock(ipc_buffer_reader_ctx_t *ctx);
error_t ipc_buffer_reader_unlock(ipc_buffer_reader_ctx_t *ctx);

error_t ipc_buffer_reader_peek(ipc_buffer_reader_ctx_t *ctx, void *buffer, uint32_t *p_itemscount);
error_t ipc_buffer_reader_pull(ipc_buffer_reader_ctx_t *ctx, void *buffer, uint32_t *p_itemscount);
error_t ipc_buffer_reader_begin(ipc_buffer_reader_ctx_t *ctx, uint32_t *p_itemscount);
error_t ipc_buffer_reader_read(ipc_buffer_reader_ctx_t *ctx, void *buffer, uint32_t *p_itemscount);
error_t ipc_buffer_reader_release(ipc_buffer_reader_ctx_t *ctx);
error_t ipc_buffer_reader_cancel(ipc_buffer_reader_ctx_t *ctx);

error_t ipc_buffer_reader_get_memory_used(ipc_buffer_reader_ctx_t *ctx, uint32_t *p_used_items);
error_t ipc_buffer_reader_get_memory_free(ipc_buffer_reader_ctx_t *ctx, uint32_t *p_free_items);

error_t ipc_buffer_reader_get_buffers_used(ipc_buffer_reader_ctx_t *ctx, uint32_t *p_used_buffers);
error_t ipc_buffer_reader_get_buffers_free(ipc_buffer_reader_ctx_t *ctx, uint32_t *p_free_buffers);

error_t ipc_buffer_reader_is_memory_full(ipc_buffer_reader_ctx_t *ctx, bool *p_is_full);
error_t ipc_buffer_reader_is_memory_empty(ipc_buffer_reader_ctx_t *ctx, bool *p_is_empty);

error_t ipc_buffer_reader_is_buffers_full(ipc_buffer_reader_ctx_t *ctx, bool *p_is_full);
error_t ipc_buffer_reader_is_buffers_empty(ipc_buffer_reader_ctx_t *ctx, bool *p_is_empty);

error_t ipc_buffer_reader_is_full(ipc_buffer_reader_ctx_t *ctx, bool *p_is_full);
error_t ipc_buffer_reader_is_empty(ipc_buffer_reader_ctx_t *ctx, bool *p_is_empty);

error_t ipc_buffer_reader_is_sync(ipc_buffer_reader_ctx_t *ctx, bool *p_is_sync);
error_t ipc_buffer_reader_synchronize(ipc_buffer_reader_ctx_t *ctx);

#endif /* IPC_INC_IPC_BUFFER_READER_H_ */
