/*
 * ipc_transfer_types.h
 *
 *  Created on: 4 нояб. 2026 г.
 *      Author: VHEMaster
 */

#ifndef IPC_INC_IPC_TRANSFER_TYPES_H_
#define IPC_INC_IPC_TRANSFER_TYPES_H_

#include "ipc_fifo_types.h"

#define IPC_TRANSFER_UNIQUE_VAL   (0x408A442Au)

typedef enum {
    IPC_TRANSFER_MEMCPY_START = 0,
    IPC_TRANSFER_MEMCPY_POLL
}ipc_transfer_memcpy_action_t;

typedef error_t (*ipc_transfer_memcpy_cb_t)(void *memory, const void *source,
    uint32_t bytes, ipc_transfer_memcpy_action_t action, void *usrdata);

typedef enum {
  IPC_TRANSFER_OPERATION_IDLE = 0,
  IPC_TRANSFER_OPERATION_REQUEST,
  IPC_TRANSFER_OPERATION_REJECT,
  IPC_TRANSFER_OPERATION_CANCEL,
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
    error_t reject_errcode;

}ipc_transfer_header_ctx_t;

typedef struct {
    ipc_transfer_header_ctx_t *header;
    bool lock;

    ipc_transfer_operation_t operation;

    void *pointer;
    uint32_t requested_size;
    uint32_t request_cnt;
    uint32_t endpoint;

}ipc_transfer_receiver_ctx_t;

typedef struct {

    ipc_transfer_header_ctx_t *header;
    bool lock;

    ipc_transfer_memcpy_cb_t memcpy_cb;
    void *memcpy_usrdata;
    uint32_t memcpy_threshold;
    bool cancel_pending;

    uint32_t unique_val;
    ipc_transfer_operation_t operation;

    const void *pointer;
    void *destination;
    uint32_t requested_size;
    uint32_t request_cnt;
    uint32_t endpoint;

}ipc_transfer_sender_ctx_t;

#endif /* IPC_INC_IPC_TRANSFER_TYPES_H_ */
