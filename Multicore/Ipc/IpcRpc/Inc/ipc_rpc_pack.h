/*
 * ipc_rpc_defines.h
 *
 *  Created on: 9 нояб. 2026 г.
 *      Author: VHEMaster
 */

#ifndef IPC_IPCRPC_INC_IPC_RPC_DEFINES_H_
#define IPC_IPCRPC_INC_IPC_RPC_DEFINES_H_

#include "compiler.h"
#include "errors.h"

STATIC_INLINE OPTIMIZE_FAST
error_t ipc_rpc_field_pos(uint32_t buffer_length, uint32_t pos,
    uint32_t bytes, uint32_t alignment, uint32_t *p_field_pos)
{
  error_t err = E_OK;
  uint32_t mask, padding, remaining;

  do {
    BREAK_IF_ACTION(alignment == 0u, err = E_PARAM);
    mask = alignment - 1u;
    BREAK_IF_ACTION((alignment & mask) != 0u, err = E_PARAM);
    BREAK_IF_ACTION(pos > buffer_length, err = E_OVERFLOW);

    padding = (0u - pos) & mask;
    remaining = buffer_length - pos;
    BREAK_IF_ACTION(padding > remaining || bytes > remaining - padding, err = E_OVERFLOW);

    *p_field_pos = pos + padding;

  } while(0);

  return err;
}

STATIC_INLINE OPTIMIZE_FAST
error_t ipc_rpc_pack(void *buffer, uint32_t buffer_length,
    const void *field, uint32_t bytes, uint32_t *p_pos, uint32_t alignment)
{
  error_t err = E_OK;
  uint32_t pos, field_pos;

  do {
    BREAK_IF_ACTION(buffer == NULL || p_pos == NULL, err = E_PARAM);
    BREAK_IF_ACTION(bytes != 0u && field == NULL, err = E_PARAM);

    pos = *p_pos;
    err = ipc_rpc_field_pos(buffer_length, pos, bytes, alignment, &field_pos);
    BREAK_IF(err != E_OK);

    if(bytes != 0u) {
      __builtin_memcpy(buffer + field_pos, field, bytes);
    }
    *p_pos = field_pos + bytes;

  } while(0);

  return err;
}

STATIC_INLINE OPTIMIZE_FAST
error_t ipc_rpc_unpack(const void *buffer, uint32_t buffer_length,
    void *field, uint32_t bytes, uint32_t *p_pos, uint32_t alignment)
{
  error_t err = E_OK;
  uint32_t field_pos;

  do {
    BREAK_IF_ACTION(buffer == NULL || p_pos == NULL, err = E_PARAM);
    BREAK_IF_ACTION(bytes != 0u && field == NULL, err = E_PARAM);

    err = ipc_rpc_field_pos(buffer_length, *p_pos, bytes, alignment, &field_pos);
    BREAK_IF(err != E_OK);

    if(bytes != 0u) {
      __builtin_memcpy(field, buffer + field_pos, bytes);
    }
    *p_pos = field_pos + bytes;

  } while(0);

  return err;
}
#endif /* IPC_IPCRPC_INC_IPC_RPC_DEFINES_H_ */
