/*
 * ipc_rpc_private.h
 */

#ifndef IPC_IPCRPC_SRC_IPC_RPC_PRIVATE_H_
#define IPC_IPCRPC_SRC_IPC_RPC_PRIVATE_H_

#include "ipc_rpc_types.h"

STATIC_INLINE
uint32_t ipc_rpc_response_capacity(const void *buffer, uint32_t buffer_size, uint32_t args_bytes)
{
  const uintptr_t mask = IPC_RPC_RESPONSE_ALIGNMENT - 1u;
  const uint32_t padding = (uint32_t)((0u - ((uintptr_t)buffer + args_bytes)) & mask);
  const uint32_t tail = buffer_size - args_bytes;

  return padding < tail ? tail - padding : 0u;
}

#endif /* IPC_IPCRPC_SRC_IPC_RPC_PRIVATE_H_ */
