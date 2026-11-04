/*
 * ipc_writer.h
 *
 *  Created on: 4 нояб. 2026 г.
 *      Author: VHEMaster
 */

#ifndef IPC_INC_IPC_WRITER_H_
#define IPC_INC_IPC_WRITER_H_

#include "ipc_types.h"

error_t ipc_writer_init(ipc_writer_ctx_t *ctx, uint32_t itemsize, uint32_t itemcount, ipc_header_ctx_t *header, void *buffer);
error_t ipc_writer_push(ipc_writer_ctx_t *ctx, const void *buffer);

error_t ipc_writer_get_used(ipc_writer_ctx_t *ctx, uint32_t *p_used_elem);
error_t ipc_writer_get_free(ipc_writer_ctx_t *ctx, uint32_t *p_free_elem);

error_t ipc_writer_is_full(ipc_writer_ctx_t *ctx, bool *p_is_full);
error_t ipc_writer_is_empty(ipc_writer_ctx_t *ctx, bool *p_is_empty);

error_t ipc_writer_synchronize(ipc_reader_ctx_t *ctx);

#endif /* IPC_INC_IPC_WRITER_H_ */
