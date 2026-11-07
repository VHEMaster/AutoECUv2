/*
 * config_ipc_types.h
 *
 *  Created on: 7 нояб. 2026 г.
 *      Author: VHEMaster
 */

#ifndef CONFIG_IPC_INC_CONFIG_IPC_TYPES_H_
#define CONFIG_IPC_INC_CONFIG_IPC_TYPES_H_

#include "common.h"

#define ECU_IPC_NONE               (255)

#define ECU_IPC_INSTANCE_MAX       (4)

#define ECU_IPCS_MAX (                \
    ECU_IPC_FIFO_WRITER_MAX         + \
    ECU_IPC_FIFO_READER_MAX         + \
    ECU_IPC_BUFFER_WRITER_MAX       + \
    ECU_IPC_BUFFER_READER_MAX       + \
    ECU_IPC_TRANSFER_SENDER_MAX     + \
    ECU_IPC_TRANSFER_RECEIVER_MAX)

typedef uint32_t ecu_ipc_instance_t;

typedef enum {
  ECU_IPC_TYPE_FIFO_WRITER = 0,
  ECU_IPC_TYPE_FIFO_READER,
  ECU_IPC_TYPE_BUFFER_WRITER,
  ECU_IPC_TYPE_BUFFER_READER,
  ECU_IPC_TYPE_TRANSFER_SENDER,
  ECU_IPC_TYPE_TRANSFER_RECEIVER,
  ECU_IPC_TYPE_MAX
}ecu_ipc_type_t;

typedef enum {
  ECU_IPC_FIFO_WRITER_1 = 0,
  ECU_IPC_FIFO_WRITER_2,
  ECU_IPC_FIFO_WRITER_MAX
}ecu_ipc_fifo_writer_t;

typedef enum {
  ECU_IPC_FIFO_READER_1 = 0,
  ECU_IPC_FIFO_READER_2,
  ECU_IPC_FIFO_READER_MAX
}ecu_ipc_fifo_reader_t;

typedef enum {
  ECU_IPC_BUFFER_WRITER_1 = 0,
  ECU_IPC_BUFFER_WRITER_2,
  ECU_IPC_BUFFER_WRITER_MAX
}ecu_ipc_buffer_writer_t;

typedef enum {
  ECU_IPC_BUFFER_READER_1 = 0,
  ECU_IPC_BUFFER_READER_2,
  ECU_IPC_BUFFER_READER_MAX
}ecu_ipc_buffer_reader_t;

typedef enum {
  ECU_IPC_TRANSFER_SENDER_1 = 0,
  ECU_IPC_TRANSFER_SENDER_2,
  ECU_IPC_TRANSFER_SENDER_MAX
}ecu_ipc_transfer_sender_t;

typedef enum {
  ECU_IPC_TRANSFER_RECEIVER_1 = 0,
  ECU_IPC_TRANSFER_RECEIVER_2,
  ECU_IPC_TRANSFER_RECEIVER_MAX
}ecu_ipc_transfer_receiver_t;

typedef void (*ecu_ipc_loop_func_t)(void *ctx);


#endif /* CONFIG_IPC_INC_CONFIG_IPC_TYPES_H_ */
