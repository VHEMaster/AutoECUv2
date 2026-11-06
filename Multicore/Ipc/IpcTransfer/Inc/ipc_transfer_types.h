/*
 * ipc_transfer_types.h
 *
 *  Created on: 4 нояб. 2026 г.
 *      Author: VHEMaster
 */

#ifndef IPC_INC_IPC_TRANSFER_TYPES_H_
#define IPC_INC_IPC_TRANSFER_TYPES_H_

#include "ipc_fifo_types.h"

#define IPC_TRANSFER_UNIQUE_VAL   (0x5F3759DFu)

typedef enum {
    IPC_BUFFER_MEMCPY_START = 0,
    IPC_BUFFER_MEMCPY_POLL
}ipc_transfer_memcpy_action_t;

typedef error_t (*ipc_transfer_memcpy_cb_t)(void *memory, const void *source,
    uint32_t bytes, ipc_transfer_memcpy_action_t action, void *usrdata);

typedef enum {
  IPC_TRANSFER_OPERATION_IDLE = 0,
  IPC_TRANSFER_OPERATION_REQUEST,
  IPC_TRANSFER_OPERATION_PROVIDE,
  IPC_TRANSFER_OPERATION_TRANSFER,
  IPC_TRANSFER_OPERATION_COMPLETE,
  IPC_TRANSFER_OPERATION_RELEASE,
  IPC_TRANSFER_OPERATION_MAX
}ipc_transfer_operation_t;

typedef struct {
    uint32_t unique_val;
    uint32_t request_cnt;
    ipc_transfer_operation_t operation;
    uint32_t endpoint;
    void *pointer;
    uint32_t requested_size;

}ipc_transfer_header_ctx_t;

typedef struct {
    ipc_transfer_header_ctx_t *header;
    bool lock;

    uint32_t unique_val;
    ipc_transfer_operation_t operation;

    void *transfer_pointer;
    uint32_t transfer_size;
    uint32_t request_id;

}ipc_transfer_receiver_ctx_t;

typedef struct {

    ipc_transfer_header_ctx_t *header;
    bool lock;

    ipc_transfer_memcpy_cb_t memcpy_cb;
    void *memcpy_usrdata;

    uint32_t unique_val;
    ipc_transfer_operation_t operation;

    void *transfer_pointer;
    uint32_t transfer_size;
    uint32_t request_cnt;

}ipc_transfer_sender_ctx_t;

#endif /* IPC_INC_IPC_TRANSFER_TYPES_H_ */
