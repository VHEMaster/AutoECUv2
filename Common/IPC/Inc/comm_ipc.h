#ifndef AUTOECU_COMM_IPC_H
#define AUTOECU_COMM_IPC_H
#include "ipc.h"
#include "common.h"
#define COMM_IPC_PARAMETER_READ 0x80000001u
#define COMM_IPC_PARAMETER_WRITE 0x80000002u
typedef struct {
  uint32_t parameter_id;
  ecu_core_runtime_value_ctx_t value;
} comm_ipc_parameter_rpc_t;
#define COMM_IPC_CONFIGURE 1u
#define COMM_IPC_RESET 2u
#define COMM_IPC_KEY(type, instance, method) (1u + (uint32_t)(type) + ((uint32_t)(instance) << 8) + ((method) << 16))
void comm_ipc_parameters_poll(void);
/* CM4 local value pointers are stable; only copied values cross the boundary. */
error_t comm_ipc_parameter_write(ecu_core_runtime_value_ctx_t *parameter);
error_t comm_ipc_parameters_release_writes(void);
error_t comm_ipc_dispatch(uint32_t key, void *payload, uint32_t *length);
#endif
