/*
 * ipc_transfer_receiver.c
 *
 *  Created on: 4 нояб. 2026 г.
 *      Author: VHEMaster
 */

#include "ipc_transfer_receiver.h"

#include "ipc_transfer_private.h"
#include "ipc_private.h"
#include "compiler.h"

static error_t ipc_transfer_receiver_read_operation(ipc_transfer_receiver_ctx_t *ctx, ipc_transfer_operation_t *p_operation);

error_t ipc_transfer_receiver_init(ipc_transfer_receiver_ctx_t *ctx, ipc_transfer_header_ctx_t *header)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(header == NULL, err = E_PARAM);

    memset(ctx, 0u, sizeof(ipc_transfer_receiver_ctx_t));
    ctx->header = header;

  } while(0);

  return err;
}

error_t ipc_transfer_receiver_lock(ipc_transfer_receiver_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ATOMIC_TRY_LOCK(ctx->lock) == false, err = E_AGAIN);
  } while(0);

  return err;
}

error_t ipc_transfer_receiver_unlock(ipc_transfer_receiver_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ATOMIC_ISLOCKED(ctx->lock) == false, err = E_INVALACT);
    BREAK_IF_ACTION(ctx->operation != IPC_TRANSFER_OPERATION_IDLE, err = E_BUSY);
    ATOMIC_RELEASE(ctx->lock);
  } while(0);

  return err;
}

error_t ipc_transfer_receiver_poll_for_start(ipc_transfer_receiver_ctx_t *ctx, uint32_t *p_endpoint, uint32_t *p_requested_size)
{
  error_t err = E_OK;
  error_t reject_errcode;
  ipc_transfer_operation_t operation;
  uint32_t capacity;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(p_endpoint == NULL, err = E_PARAM);
    BREAK_IF_ACTION(p_requested_size == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ATOMIC_ISLOCKED(ctx->lock) == false, err = E_INVALACT);
    BREAK_IF_ACTION(ctx->operation != IPC_TRANSFER_OPERATION_IDLE, err = E_BUSY);

    err = ipc_transfer_receiver_read_operation(ctx, &operation);
    BREAK_IF(err != E_OK);
    BREAK_IF_ACTION(operation != IPC_TRANSFER_OPERATION_REQUEST, err = E_AGAIN);

    capacity = *p_requested_size;
    ctx->request_cnt = ctx->header->request_cnt;
    ctx->endpoint = ctx->header->endpoint;
    ctx->requested_size = ctx->header->requested_size;
    ctx->operation = IPC_TRANSFER_OPERATION_REQUEST;

    if(ctx->requested_size == 0u || ctx->requested_size > capacity) {
      reject_errcode = ctx->requested_size == 0u ? E_PARAM : E_OVERFLOW;
      err = ipc_transfer_receiver_send_reject(ctx, reject_errcode);
      BREAK_IF(err != E_OK);
      err = E_AGAIN;
      break;
    }

    *p_endpoint = ctx->endpoint;
    *p_requested_size = ctx->requested_size;

  } while(0);

  return err;
}

error_t ipc_transfer_receiver_send_pointer(ipc_transfer_receiver_ctx_t *ctx, void *pointer)
{
  error_t err = E_OK;
  ipc_transfer_operation_t operation;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(pointer == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ATOMIC_ISLOCKED(ctx->lock) == false, err = E_INVALACT);
    BREAK_IF_ACTION(ctx->operation != IPC_TRANSFER_OPERATION_REQUEST, err = E_INVALACT);

    err = ipc_transfer_receiver_read_operation(ctx, &operation);
    BREAK_IF(err != E_OK);
    BREAK_IF_ACTION(operation != IPC_TRANSFER_OPERATION_REQUEST, err = E_FAULT);

    ctx->pointer = pointer;
    ctx->operation = IPC_TRANSFER_OPERATION_PROVIDE;

    ATOMIC_STORE_RELAXED(ctx->header->pointer, pointer);
    ATOMIC_STORE(ctx->header->operation, IPC_TRANSFER_OPERATION_PROVIDE);

  } while(0);

  return err;
}

error_t ipc_transfer_receiver_send_reject(ipc_transfer_receiver_ctx_t *ctx, error_t errcode)
{
  error_t err = E_OK;
  ipc_transfer_operation_t operation;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(errcode == E_OK || errcode == E_AGAIN, err = E_PARAM);
    BREAK_IF_ACTION(ATOMIC_ISLOCKED(ctx->lock) == false, err = E_INVALACT);
    BREAK_IF_ACTION(ctx->operation != IPC_TRANSFER_OPERATION_REQUEST, err = E_INVALACT);

    err = ipc_transfer_receiver_read_operation(ctx, &operation);
    BREAK_IF(err != E_OK);
    BREAK_IF_ACTION(operation != IPC_TRANSFER_OPERATION_REQUEST, err = E_FAULT);

    ctx->operation = IPC_TRANSFER_OPERATION_IDLE;
    ATOMIC_STORE_RELAXED(ctx->header->reject_errcode, errcode);
    ATOMIC_STORE(ctx->header->operation, IPC_TRANSFER_OPERATION_REJECT);

  } while(0);

  return err;
}

error_t ipc_transfer_receiver_poll_cplt(ipc_transfer_receiver_ctx_t *ctx)
{
  error_t err = E_OK;
  ipc_transfer_operation_t operation;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ATOMIC_ISLOCKED(ctx->lock) == false, err = E_INVALACT);
    BREAK_IF(ctx->operation == IPC_TRANSFER_OPERATION_COMPLETE);
    BREAK_IF_ACTION(ctx->operation == IPC_TRANSFER_OPERATION_CANCEL, err = E_ABORTED);
    BREAK_IF_ACTION(ctx->operation != IPC_TRANSFER_OPERATION_PROVIDE, err = E_INVALACT);

    err = ipc_transfer_receiver_read_operation(ctx, &operation);
    BREAK_IF(err != E_OK);
    BREAK_IF_ACTION(operation == IPC_TRANSFER_OPERATION_PROVIDE, err = E_AGAIN);
    BREAK_IF_ACTION(operation != IPC_TRANSFER_OPERATION_COMPLETE &&
        operation != IPC_TRANSFER_OPERATION_CANCEL, err = E_FAULT);

    ctx->operation = operation;
    BREAK_IF_ACTION(operation == IPC_TRANSFER_OPERATION_CANCEL, err = E_ABORTED);

  } while(0);

  return err;
}

error_t ipc_transfer_receiver_release(ipc_transfer_receiver_ctx_t *ctx)
{
  error_t err = E_OK;
  ipc_transfer_operation_t operation;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ATOMIC_ISLOCKED(ctx->lock) == false, err = E_INVALACT);
    BREAK_IF_ACTION(ctx->operation != IPC_TRANSFER_OPERATION_COMPLETE &&
        ctx->operation != IPC_TRANSFER_OPERATION_CANCEL, err = E_INVALACT);

    err = ipc_transfer_receiver_read_operation(ctx, &operation);
    BREAK_IF(err != E_OK);
    BREAK_IF_ACTION(operation != ctx->operation, err = E_FAULT);

    ctx->pointer = NULL;
    ctx->requested_size = 0u;
    ctx->operation = IPC_TRANSFER_OPERATION_IDLE;
    ATOMIC_STORE(ctx->header->operation, IPC_TRANSFER_OPERATION_RELEASE);

  } while(0);

  return err;
}

error_t ipc_transfer_receiver_is_sync(ipc_transfer_receiver_ctx_t *ctx, bool *p_is_sync)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(p_is_sync == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ATOMIC_ISLOCKED(ctx->lock) == false, err = E_INVALACT);

    *p_is_sync = ipc_transfer_receiver_synchronize(ctx) == E_OK;

  } while(0);

  return err;
}

error_t ipc_transfer_receiver_synchronize(ipc_transfer_receiver_ctx_t *ctx)
{
  error_t err = E_OK;
  ipc_transfer_operation_t operation;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ATOMIC_ISLOCKED(ctx->lock) == false, err = E_INVALACT);

    err = ipc_transfer_receiver_read_operation(ctx, &operation);

  } while(0);

  return err;
}

static error_t ipc_transfer_receiver_read_operation(ipc_transfer_receiver_ctx_t *ctx, ipc_transfer_operation_t *p_operation)
{
  error_t err = ipc_transfer_read_operation(ctx->header, p_operation);

  if(err == E_OK && ctx->operation != IPC_TRANSFER_OPERATION_IDLE &&
      ctx->header->request_cnt != ctx->request_cnt) {
    err = E_FAULT;
  }

  return err;
}
