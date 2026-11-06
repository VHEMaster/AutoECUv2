/*
 * ipc_transfer_receiver.h
 *
 *  Created on: 4 нояб. 2026 г.
 *      Author: VHEMaster
 */

#ifndef IPC_INC_IPC_TRANSFER_RECEIVER_H_
#define IPC_INC_IPC_TRANSFER_RECEIVER_H_

#include "ipc_transfer_types.h"

error_t ipc_transfer_receiver_init(ipc_transfer_receiver_ctx_t *ctx, ipc_transfer_header_ctx_t *header);

error_t ipc_transfer_receiver_lock(ipc_transfer_receiver_ctx_t *ctx);
error_t ipc_transfer_receiver_unlock(ipc_transfer_receiver_ctx_t *ctx);

error_t ipc_transfer_receiver_poll_for_start(ipc_transfer_receiver_ctx_t *ctx, uint32_t *p_endpoint, uint32_t *p_requested_size);
error_t ipc_transfer_receiver_send_pointer(ipc_transfer_receiver_ctx_t *ctx, void *pointer);
error_t ipc_transfer_receiver_send_reject(ipc_transfer_receiver_ctx_t *ctx, error_t errcode);
error_t ipc_transfer_receiver_poll_cplt(ipc_transfer_receiver_ctx_t *ctx);
error_t ipc_transfer_receiver_release(ipc_transfer_receiver_ctx_t *ctx);

error_t ipc_transfer_receiver_is_sync(ipc_transfer_receiver_ctx_t *ctx, bool *p_is_sync);
error_t ipc_transfer_receiver_synchronize(ipc_transfer_receiver_ctx_t *ctx);

#endif /* IPC_INC_IPC_TRANSFER_RECEIVER_H_ */
