/*
 * config_ipc.h
 */

#ifndef CONFIG_INC_CONFIG_IPC_H_
#define CONFIG_INC_CONFIG_IPC_H_

#include "ecu_ipc.h"
#include "ipc_buffer.h"
#include "ipc_transfer.h"

typedef struct {
    bool sender;
    ipc_buffer_header_ctx_t *header;
    ipc_buffer_descriptor_t *descriptors;
    uint32_t descriptorscount;
    void *buffer;
    uint32_t size;
}ecu_config_ipc_buffer_t;

typedef struct {
    bool sender;
    ipc_transfer_header_ctx_t *header;
    void *buffer;
    void *bus_buffer;
    uint32_t size;
}ecu_config_ipc_transfer_t;

void ecu_config_ipc_buffer(ecu_ipc_buffer_t instance, ecu_config_ipc_buffer_t *config);
void ecu_config_ipc_transfer(ecu_ipc_transfer_t instance, ecu_config_ipc_transfer_t *config);

#endif /* CONFIG_INC_CONFIG_IPC_H_ */
