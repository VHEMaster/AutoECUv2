/*
 * ipc_transfer_sender.c
 *
 *  Created on: 4 нояб. 2026 г.
 *      Author: VHEMaster
 */

#include "ipc_transfer_sender.h"

#include "ipc_transfer_private.h"
#include "ipc_private.h"
#include "compiler.h"

static void ipc_transfer_sender_retire(ipc_transfer_sender_ctx_t *ctx);
static error_t ipc_transfer_sender_read_operation(const ipc_transfer_sender_ctx_t *ctx, ipc_transfer_operation_t *p_operation);

error_t ipc_transfer_sender_init(ipc_transfer_sender_ctx_t *ctx, ipc_transfer_header_ctx_t *header)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(header == NULL, err = E_PARAM);

    memset(ctx, 0u, sizeof(ipc_transfer_sender_ctx_t));
    ctx->header = header;
    ctx->unique_val = IPC_TRANSFER_UNIQUE_VAL;

    ATOMIC_STORE(header->unique_val, 0u);
    ATOMIC_STORE_RELAXED(header->operation, IPC_TRANSFER_OPERATION_IDLE);
    header->request_cnt = 0u;
    header->endpoint = 0u;
    header->pointer = NULL;
    header->requested_size = 0u;
    header->reject_errcode = E_OK;
    ATOMIC_STORE(header->unique_val, IPC_TRANSFER_UNIQUE_VAL);

  } while(0);

  return err;
}

error_t ipc_transfer_sender_set_memcpy(ipc_transfer_sender_ctx_t *ctx, uint32_t threshold_bytes,
    ipc_transfer_memcpy_cb_t memcpy_cb, void *memcpy_usrdata)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->operation != IPC_TRANSFER_OPERATION_IDLE, err = E_INVALACT);

    ctx->memcpy_cb = memcpy_cb;
    ctx->memcpy_usrdata = memcpy_usrdata;
    ctx->memcpy_threshold = threshold_bytes;

  } while(0);

  return err;
}

error_t ipc_transfer_sender_lock(ipc_transfer_sender_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ATOMIC_TRY_LOCK(ctx->lock) == false, err = E_AGAIN);
  } while(0);

  return err;
}

error_t ipc_transfer_sender_unlock(ipc_transfer_sender_ctx_t *ctx)
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

error_t ipc_transfer_sender_request(ipc_transfer_sender_ctx_t *ctx, uint32_t endpoint, const void *pointer, uint32_t requested_size)
{
  error_t err = E_OK;
  ipc_transfer_operation_t operation;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(pointer == NULL, err = E_PARAM);
    BREAK_IF_ACTION(requested_size == 0u, err = E_PARAM);
    BREAK_IF_ACTION(ATOMIC_ISLOCKED(ctx->lock) == false, err = E_INVALACT);
    BREAK_IF_ACTION(ctx->operation != IPC_TRANSFER_OPERATION_IDLE, err = E_BUSY);

    err = ipc_transfer_read_operation(ctx->header, &operation);
    BREAK_IF(err != E_OK);
    BREAK_IF_ACTION(operation != IPC_TRANSFER_OPERATION_IDLE, err = E_BUSY);

    ctx->pointer = pointer;
    ctx->requested_size = requested_size;
    ctx->endpoint = endpoint;
    ctx->request_cnt++;
    ctx->operation = IPC_TRANSFER_OPERATION_REQUEST;

    ATOMIC_STORE_RELAXED(ctx->header->request_cnt, ctx->request_cnt);
    ATOMIC_STORE_RELAXED(ctx->header->endpoint, endpoint);
    ATOMIC_STORE_RELAXED(ctx->header->requested_size, requested_size);
    ATOMIC_STORE(ctx->header->operation, IPC_TRANSFER_OPERATION_REQUEST);

  } while(0);

  return err;
}

error_t ipc_transfer_sender_poll_cplt(ipc_transfer_sender_ctx_t *ctx)
{
  error_t err = E_OK;
  ipc_transfer_operation_t operation;
  ipc_transfer_memcpy_action_t action;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ATOMIC_ISLOCKED(ctx->lock) == false, err = E_INVALACT);
    BREAK_IF_ACTION(ctx->cancel_pending, err = E_INVALACT);
    BREAK_IF(ctx->operation == IPC_TRANSFER_OPERATION_COMPLETE);
    BREAK_IF_ACTION(ctx->operation != IPC_TRANSFER_OPERATION_REQUEST &&
        ctx->operation != IPC_TRANSFER_OPERATION_TRANSFER, err = E_INVALACT);

    err = ipc_transfer_sender_read_operation(ctx, &operation);
    BREAK_IF(err != E_OK);

    if(ctx->operation == IPC_TRANSFER_OPERATION_REQUEST) {
      BREAK_IF_ACTION(operation == IPC_TRANSFER_OPERATION_REQUEST, err = E_AGAIN);
      if(operation == IPC_TRANSFER_OPERATION_REJECT) {
        err = ctx->header->reject_errcode;
        ipc_transfer_sender_retire(ctx);
        break;
      }

      BREAK_IF_ACTION(operation != IPC_TRANSFER_OPERATION_PROVIDE, err = E_FAULT);
      ctx->destination = ATOMIC_LOAD_RELAXED(ctx->header->pointer);
      BREAK_IF_ACTION(ctx->destination == NULL, err = E_FAULT);

      action = IPC_TRANSFER_MEMCPY_START;
    } else {
      BREAK_IF_ACTION(operation != IPC_TRANSFER_OPERATION_PROVIDE, err = E_FAULT);
      action = IPC_TRANSFER_MEMCPY_POLL;
    }

    if(ctx->memcpy_cb != NULL && ctx->requested_size >= ctx->memcpy_threshold) {
      err = ctx->memcpy_cb(ctx->destination, ctx->pointer, ctx->requested_size, action, ctx->memcpy_usrdata);
      BREAK_IF_ACTION(err == E_AGAIN, ctx->operation = IPC_TRANSFER_OPERATION_TRANSFER);
    } else {
      err = E_NOTSUPPORT;
    }

    if(err != E_OK) {
      memcpy(ctx->destination, ctx->pointer, ctx->requested_size);
      err = E_OK;
    }

    ctx->operation = IPC_TRANSFER_OPERATION_COMPLETE;
    ATOMIC_STORE(ctx->header->operation, IPC_TRANSFER_OPERATION_COMPLETE);

  } while(0);

  return err;
}

error_t ipc_transfer_sender_poll_release(ipc_transfer_sender_ctx_t *ctx)
{
  error_t err = E_OK;
  ipc_transfer_operation_t operation;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ATOMIC_ISLOCKED(ctx->lock) == false, err = E_INVALACT);
    BREAK_IF_ACTION(ctx->operation != IPC_TRANSFER_OPERATION_COMPLETE &&
        ctx->operation != IPC_TRANSFER_OPERATION_CANCEL, err = E_INVALACT);

    err = ipc_transfer_sender_read_operation(ctx, &operation);
    BREAK_IF(err != E_OK);
    BREAK_IF_ACTION(operation == ctx->operation, err = E_AGAIN);
    BREAK_IF_ACTION(operation != IPC_TRANSFER_OPERATION_RELEASE, err = E_FAULT);

    ipc_transfer_sender_retire(ctx);

  } while(0);

  return err;
}

error_t ipc_transfer_sender_cancel(ipc_transfer_sender_ctx_t *ctx)
{
  error_t err = E_OK;
  ipc_transfer_operation_t operation;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ATOMIC_ISLOCKED(ctx->lock) == false, err = E_INVALACT);
    BREAK_IF(ctx->operation == IPC_TRANSFER_OPERATION_IDLE);

    if(ctx->operation == IPC_TRANSFER_OPERATION_COMPLETE ||
        ctx->operation == IPC_TRANSFER_OPERATION_CANCEL) {
      err = ipc_transfer_sender_poll_release(ctx);
      break;
    }

    ctx->cancel_pending = true;

    err = ipc_transfer_sender_read_operation(ctx, &operation);
    BREAK_IF(err != E_OK);

    if(ctx->operation == IPC_TRANSFER_OPERATION_REQUEST) {

      BREAK_IF_ACTION(operation == IPC_TRANSFER_OPERATION_REQUEST, err = E_AGAIN);
      if(operation == IPC_TRANSFER_OPERATION_REJECT) {
        ipc_transfer_sender_retire(ctx);
        break;
      }
      BREAK_IF_ACTION(operation != IPC_TRANSFER_OPERATION_PROVIDE, err = E_FAULT);
    } else {
      BREAK_IF_ACTION(ctx->operation != IPC_TRANSFER_OPERATION_TRANSFER ||
          operation != IPC_TRANSFER_OPERATION_PROVIDE, err = E_FAULT);

      err = ctx->memcpy_cb(ctx->destination, ctx->pointer, ctx->requested_size,
          IPC_TRANSFER_MEMCPY_POLL, ctx->memcpy_usrdata);
      BREAK_IF(err == E_AGAIN);
    }

    ctx->operation = IPC_TRANSFER_OPERATION_CANCEL;
    ATOMIC_STORE(ctx->header->operation, IPC_TRANSFER_OPERATION_CANCEL);
    err = E_AGAIN;

  } while(0);

  return err;
}

static void ipc_transfer_sender_retire(ipc_transfer_sender_ctx_t *ctx)
{
  ctx->pointer = NULL;
  ctx->destination = NULL;
  ctx->requested_size = 0u;
  ctx->cancel_pending = false;
  ctx->operation = IPC_TRANSFER_OPERATION_IDLE;
  ATOMIC_STORE(ctx->header->operation, IPC_TRANSFER_OPERATION_IDLE);
}

static error_t ipc_transfer_sender_read_operation(const ipc_transfer_sender_ctx_t *ctx, ipc_transfer_operation_t *p_operation)
{
  error_t err = ipc_transfer_read_operation(ctx->header, p_operation);

  if(err == E_OK && ctx->header->request_cnt != ctx->request_cnt) {
    err = E_FAULT;
  }

  return err;
}
