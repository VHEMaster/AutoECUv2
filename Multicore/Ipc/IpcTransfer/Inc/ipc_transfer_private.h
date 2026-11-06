/*
 * ipc_transfer_private.h
 *
 *  Created on: 4 нояб. 2026 г.
 *      Author: VHEMaster
 */

#ifndef IPC_IPCTRANSFER_INC_IPC_TRANSFER_PRIVATE_H_
#define IPC_IPCTRANSFER_INC_IPC_TRANSFER_PRIVATE_H_

#include "ipc_transfer_types.h"

error_t ipc_transfer_read_operation(const ipc_transfer_header_ctx_t *header, ipc_transfer_operation_t *p_operation);

#endif /* IPC_IPCTRANSFER_INC_IPC_TRANSFER_PRIVATE_H_ */
