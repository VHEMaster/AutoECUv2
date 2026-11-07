/*
 * config_ipc.c
 */

#include "config_ipc_private.h"

#include <stddef.h>

/* CM7 startup clears .sharedbss before CM4 initializes its endpoints. */
#if UINTPTR_MAX == UINT32_MAX
_Static_assert(sizeof(ipc_buffer_header_ctx_t) == 24u, "IPC buffer shared header size");
_Static_assert(sizeof(ipc_transfer_header_ctx_t) == 28u, "IPC transfer shared header size");
_Static_assert(sizeof(ipc_buffer_descriptor_t) == 8u, "IPC buffer shared descriptor size");
#endif

ecu_config_ipc_shared_t ecu_config_ipc_shared __attribute__((section(".sharedbss.ipc"))) ALIGNED(ECU_IPC_SHARED_ALIGNMENT);

extern uint8_t __ipc_shared_bus_start__[];

void ecu_config_ipc_buffer(ecu_ipc_buffer_t instance, ecu_config_ipc_buffer_t *config)
{
#ifdef CORE_CM4
  config->sender = instance == ECU_IPC_BUFFER_CM4_TO_CM7;
#else /* CORE_CM7 */
  config->sender = instance == ECU_IPC_BUFFER_CM7_TO_CM4;
#endif
  config->header = &ecu_config_ipc_shared.buffer_headers[instance];
  config->descriptors = ecu_config_ipc_shared.descriptors[instance];
  config->descriptorscount = ECU_IPC_BUFFER_DESCRIPTORS;
  config->buffer = ecu_config_ipc_shared.buffers[instance];
  config->size = ECU_IPC_BUFFER_SIZE;
}

void ecu_config_ipc_transfer(ecu_ipc_transfer_t instance, ecu_config_ipc_transfer_t *config)
{
#ifdef CORE_CM4
  config->sender = instance == ECU_IPC_TRANSFER_CM4_TO_CM7;
#else /* CORE_CM7 */
  config->sender = instance == ECU_IPC_TRANSFER_CM7_TO_CM4;
#endif
  config->header = &ecu_config_ipc_shared.transfer_headers[instance];
  config->buffer = ecu_config_ipc_shared.transfers[instance];
  config->bus_buffer = __ipc_shared_bus_start__ + offsetof(ecu_config_ipc_shared_t, transfers) +
      (uint32_t)instance * ECU_IPC_TRANSFER_SIZE;
  config->size = ECU_IPC_TRANSFER_SIZE;
}
