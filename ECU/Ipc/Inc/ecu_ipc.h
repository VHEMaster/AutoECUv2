/*
 * ecu_ipc.h
 */

#ifndef ECU_IPC_INC_ECU_IPC_H_
#define ECU_IPC_INC_ECU_IPC_H_

#include <stdint.h>
#include "errors.h"

typedef enum {
  ECU_IPC_BUFFER_CM4_TO_CM7 = 0,
  ECU_IPC_BUFFER_CM7_TO_CM4,
  ECU_IPC_BUFFER_MAX
}ecu_ipc_buffer_t;

typedef enum {
  ECU_IPC_TRANSFER_CM4_TO_CM7 = 0,
  ECU_IPC_TRANSFER_CM7_TO_CM4,
  ECU_IPC_TRANSFER_MAX
}ecu_ipc_transfer_t;

error_t ecu_ipc_init(void);
void ecu_ipc_start(void);
void ecu_ipc_loop_comm(void);

/* Call again on E_AGAIN with the same source or destination and size/capacity.
 * Keep the storage valid until the operation returns a terminal result. */
error_t ecu_ipc_buffer_send(ecu_ipc_buffer_t instance, const void *buffer, uint32_t size);
error_t ecu_ipc_buffer_receive(ecu_ipc_buffer_t instance, void *buffer, uint32_t *p_size);
error_t ecu_ipc_transfer_send(ecu_ipc_transfer_t instance, uint32_t endpoint, const void *buffer, uint32_t size);

/* The returned shared view is valid until the next receive on this instance. */
error_t ecu_ipc_transfer_receive(ecu_ipc_transfer_t instance, const void **p_buffer, uint32_t *p_size, uint32_t *p_endpoint);

#endif /* ECU_IPC_INC_ECU_IPC_H_ */
