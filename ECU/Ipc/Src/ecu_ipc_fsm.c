/*
 * ecu_ipc_fsm.c
 */

#include "ecu_ipc_fsm.h"
#include "compiler.h"

static error_t ecu_ipc_transfer_fsm_send(ecu_ipc_transfer_ctx_t *ctx);
static error_t ecu_ipc_transfer_fsm_receive(ecu_ipc_transfer_ctx_t *ctx);

void ecu_ipc_buffer_fsm(ecu_ipc_buffer_ctx_t *ctx)
{
  error_t err;
  uint32_t size;
  ecu_ipc_job_t *job = &ctx->job;

  if(ATOMIC_TRY_LOCK(job->lock) == true) {
    if(job->state == ECU_IPC_JOB_PENDING) {
      if(ctx->sender) {
        err = ipc_buffer_writer_push(&ctx->ctx.writer, job->source, job->size);
        if(err == E_OVERFLOW) {
          err = E_AGAIN;
        }
      } else {
        size = job->size;
        err = ipc_buffer_reader_pull(&ctx->ctx.reader, job->destination, &size);
        if(err == E_OK) {
          job->received = size;
        }
      }

      if(err != E_AGAIN) {
        job->result = err;
        job->state = ECU_IPC_JOB_DONE;
      }
    }
    ATOMIC_RELEASE(job->lock);
  }
}

void ecu_ipc_transfer_fsm(ecu_ipc_transfer_ctx_t *ctx)
{
  error_t err;
  ecu_ipc_job_t *job = &ctx->job;

  if(ATOMIC_TRY_LOCK(job->lock) == true) {
    if(job->state == ECU_IPC_JOB_PENDING) {
      err = ctx->sender ? ecu_ipc_transfer_fsm_send(ctx) : ecu_ipc_transfer_fsm_receive(ctx);
      if(err != E_AGAIN) {
        ctx->fsm_process = ECU_IPC_TRANSFER_FSM_INITIAL;
        job->result = err;
        job->state = ECU_IPC_JOB_DONE;
      }
    }
    ATOMIC_RELEASE(job->lock);
  }
}

static error_t ecu_ipc_transfer_fsm_send(ecu_ipc_transfer_ctx_t *ctx)
{
  error_t err;
  ecu_ipc_job_t *job = &ctx->job;

  switch(ctx->fsm_process) {
    case ECU_IPC_TRANSFER_FSM_INITIAL:
      err = ipc_transfer_sender_request(&ctx->ctx.sender, job->endpoint, job->source, job->size);
      if(err == E_OK) {
        ctx->fsm_process = ECU_IPC_TRANSFER_FSM_WAIT;
        err = E_AGAIN;
      }
      break;

    case ECU_IPC_TRANSFER_FSM_WAIT:
      err = ipc_transfer_sender_poll_cplt(&ctx->ctx.sender);
      if(err == E_OK) {
        ctx->fsm_process = ECU_IPC_TRANSFER_FSM_RELEASE;
        err = E_AGAIN;
      }
      break;

    case ECU_IPC_TRANSFER_FSM_RELEASE:
      err = ipc_transfer_sender_poll_release(&ctx->ctx.sender);
      break;

    default:
      err = E_FAULT;
      break;
  }

  return err;
}

static error_t ecu_ipc_transfer_fsm_receive(ecu_ipc_transfer_ctx_t *ctx)
{
  error_t err;
  uint32_t size;

  switch(ctx->fsm_process) {
    case ECU_IPC_TRANSFER_FSM_INITIAL:
      size = ctx->size;
      err = ipc_transfer_receiver_poll_for_start(&ctx->ctx.receiver, &ctx->received_endpoint, &size);
      if(err == E_OK) {
        err = ipc_transfer_receiver_send_pointer(&ctx->ctx.receiver, ctx->bus_buffer);
        if(err == E_OK) {
          ctx->job.received = size;
          ctx->fsm_process = ECU_IPC_TRANSFER_FSM_WAIT;
          err = E_AGAIN;
        }
      }
      break;

    case ECU_IPC_TRANSFER_FSM_WAIT:
      err = ipc_transfer_receiver_poll_cplt(&ctx->ctx.receiver);
      if(err == E_OK || err == E_ABORTED) {
        ctx->completion = err;
        ctx->fsm_process = ECU_IPC_TRANSFER_FSM_RELEASE;
        err = E_AGAIN;
      }
      break;

    case ECU_IPC_TRANSFER_FSM_RELEASE:
      err = ipc_transfer_receiver_release(&ctx->ctx.receiver);
      if(err == E_OK) {
        err = ctx->completion;
      }
      break;

    default:
      err = E_FAULT;
      break;
  }

  return err;
}
