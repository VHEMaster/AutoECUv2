/*
 * config_ipc_private.h
 */

#ifndef CONFIG_INC_CONFIG_IPC_PRIVATE_H_
#define CONFIG_INC_CONFIG_IPC_PRIVATE_H_

#include "config_ipc.h"
#include "compiler.h"

#define ECU_IPC_BUFFER_SIZE              (1024u)
#define ECU_IPC_BUFFER_DESCRIPTORS       (16u)
#define ECU_IPC_TRANSFER_SIZE            (1024u)
#define ECU_IPC_SHARED_ALIGNMENT         (32u)

typedef struct {
    ipc_buffer_header_ctx_t buffer_headers[ECU_IPC_BUFFER_MAX];
    ipc_transfer_header_ctx_t transfer_headers[ECU_IPC_TRANSFER_MAX];
    ipc_buffer_descriptor_t descriptors[ECU_IPC_BUFFER_MAX][ECU_IPC_BUFFER_DESCRIPTORS];
    uint8_t buffers[ECU_IPC_BUFFER_MAX][ECU_IPC_BUFFER_SIZE] ALIGNED(ECU_IPC_SHARED_ALIGNMENT);
    uint8_t transfers[ECU_IPC_TRANSFER_MAX][ECU_IPC_TRANSFER_SIZE] ALIGNED(ECU_IPC_SHARED_ALIGNMENT);
}ecu_config_ipc_shared_t;

#endif /* CONFIG_INC_CONFIG_IPC_PRIVATE_H_ */
