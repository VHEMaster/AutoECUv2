/*
 * ipc_transfer_private.c
 *
 *  Created on: 4 нояб. 2026 г.
 *      Author: VHEMaster
 */

#include "ipc_transfer_private.h"

error_t ipc_transfer_read_operation(const ipc_transfer_header_ctx_t *header, ipc_transfer_operation_t *p_operation)
{
  error_t err = E_OK;
  ipc_transfer_operation_t operation;

  do {
    BREAK_IF_ACTION(header == NULL, err = E_INVALACT);
    BREAK_IF_ACTION(ATOMIC_LOAD(header->unique_val) != IPC_TRANSFER_UNIQUE_VAL, err = E_AGAIN);

    operation = ATOMIC_LOAD(header->operation);
    BREAK_IF_ACTION(operation >= IPC_TRANSFER_OPERATION_MAX, err = E_FAULT);
    *p_operation = operation;

  } while(0);

  return err;
}
