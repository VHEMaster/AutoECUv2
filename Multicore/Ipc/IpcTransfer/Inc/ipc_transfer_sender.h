/*
 * ipc_transfer_sender.h
 *
 *  Created on: 4 нояб. 2026 г.
 *      Author: VHEMaster
 */

#ifndef IPC_INC_IPC_TRANSFER_SENDER_H_
#define IPC_INC_IPC_TRANSFER_SENDER_H_

#include "ipc_transfer_types.h"

error_t ipc_transfer_sender_init(ipc_transfer_sender_ctx_t *ctx, ipc_transfer_header_ctx_t *header);
error_t ipc_transfer_sender_set_memcpy(ipc_transfer_sender_ctx_t *ctx, ipc_transfer_memcpy_cb_t memcpy_cb, void *memcpy_usrdata);

error_t ipc_transfer_sender_lock(ipc_transfer_sender_ctx_t *ctx);
error_t ipc_transfer_sender_unlock(ipc_transfer_sender_ctx_t *ctx);



#endif /* IPC_INC_IPC_TRANSFER_SENDER_H_ */
