/*
 * ecu_ipc_types.h
 */

#ifndef ECU_IPC_INC_ECU_IPC_TYPES_H_
#define ECU_IPC_INC_ECU_IPC_TYPES_H_

#include "ipc_buffer.h"
#include "ipc_transfer.h"

typedef enum {
  ECU_IPC_JOB_IDLE = 0,
  ECU_IPC_JOB_PENDING,
  ECU_IPC_JOB_DONE
}ecu_ipc_job_state_t;

typedef enum {
  ECU_IPC_TRANSFER_FSM_INITIAL = 0,
  ECU_IPC_TRANSFER_FSM_WAIT,
  ECU_IPC_TRANSFER_FSM_RELEASE
}ecu_ipc_transfer_fsm_t;

typedef struct {
    bool lock;
    ecu_ipc_job_state_t state;
    const void *source;
    void *destination;
    uint32_t size;
    uint32_t endpoint;
    uint32_t received;
    error_t result;
}ecu_ipc_job_t;

typedef struct {
    ecu_ipc_job_t job;
    bool sender;
    uint32_t size;
    union {
        ipc_buffer_writer_ctx_t writer;
        ipc_buffer_reader_ctx_t reader;
    }ctx;
}ecu_ipc_buffer_ctx_t;

typedef struct {
    ecu_ipc_job_t job;
    bool sender;
    uint32_t size;
    const void *buffer;
    void *bus_buffer;
    uint32_t received_endpoint;
    ecu_ipc_transfer_fsm_t fsm_process;
    error_t completion;
    union {
        ipc_transfer_sender_ctx_t sender;
        ipc_transfer_receiver_ctx_t receiver;
    }ctx;
}ecu_ipc_transfer_ctx_t;

#endif /* ECU_IPC_INC_ECU_IPC_TYPES_H_ */
