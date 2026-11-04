/*
 * ipc_reader.h
 *
 *  Created on: 4 нояб. 2026 г.
 *      Author: VHEMaster
 */

#ifndef IPC_INC_IPC_READER_H_
#define IPC_INC_IPC_READER_H_

#include "ipc_types.h"

error_t ipc_reader_init(ipc_reader_ctx_t *ctx, uint32_t itemsize, uint32_t itemcount, ipc_header_ctx_t *header, const void *buffer);
error_t ipc_reader_peek(ipc_reader_ctx_t *ctx, void *buffer);
error_t ipc_reader_pull(ipc_reader_ctx_t *ctx, void *buffer);

error_t ipc_reader_get_used(ipc_reader_ctx_t *ctx, uint32_t *p_used_elem);
error_t ipc_reader_get_free(ipc_reader_ctx_t *ctx, uint32_t *p_free_elem);

error_t ipc_reader_is_full(ipc_reader_ctx_t *ctx, bool *p_is_full);
error_t ipc_reader_is_empty(ipc_reader_ctx_t *ctx, bool *p_is_empty);
error_t ipc_reader_is_sync(ipc_reader_ctx_t *ctx, bool *p_is_sync);
error_t ipc_reader_synchronize(ipc_reader_ctx_t *ctx);

#endif /* IPC_INC_IPC_READER_H_ */
