/*
 * ecu_ipc.c
 */

#include "ecu_ipc.h"
#include "config_ipc.h"
#include "compiler.h"

typedef enum {
  ECU_IPC_JOB_IDLE = 0,
  ECU_IPC_JOB_PENDING,
  ECU_IPC_JOB_DONE
}ecu_ipc_job_state_t;

typedef enum {
  ECU_IPC_TRANSFER_BEGIN = 0,
  ECU_IPC_TRANSFER_WAIT,
  ECU_IPC_TRANSFER_RELEASE
}ecu_ipc_transfer_state_t;

typedef struct {
    bool lock;
    ecu_ipc_job_state_t state;
    const void *source;
    void *destination;
    uint32_t size;
    uint32_t endpoint;
    uint32_t received;
    error_t result;
}ecu_ipc_job_t;

typedef struct {
    ecu_ipc_job_t job;
    bool sender;
    uint32_t size;
    union {
        ipc_buffer_writer_ctx_t writer;
        ipc_buffer_reader_ctx_t reader;
    }ctx;
}ecu_ipc_buffer_ctx_t;

typedef struct {
    ecu_ipc_job_t job;
    bool sender;
    uint32_t size;
    const void *buffer;
    void *bus_buffer;
    uint32_t received_endpoint;
    ecu_ipc_transfer_state_t state;
    error_t completion;
    union {
        ipc_transfer_sender_ctx_t sender;
        ipc_transfer_receiver_ctx_t receiver;
    }ctx;
}ecu_ipc_transfer_ctx_t;

static ecu_ipc_buffer_ctx_t ecu_ipc_buffers[ECU_IPC_BUFFER_MAX];
static ecu_ipc_transfer_ctx_t ecu_ipc_transfers[ECU_IPC_TRANSFER_MAX];
static bool ecu_ipc_initialized;
static bool ecu_ipc_started;

static error_t ecu_ipc_buffer_init(ecu_ipc_buffer_t instance);
static error_t ecu_ipc_transfer_init(ecu_ipc_transfer_t instance);
static error_t ecu_ipc_poll_job(ecu_ipc_job_t *job, const void *source, void *destination, uint32_t size, uint32_t endpoint);
static void ecu_ipc_buffer_loop_comm(ecu_ipc_buffer_ctx_t *ctx);
static void ecu_ipc_transfer_loop_comm(ecu_ipc_transfer_ctx_t *ctx);
static error_t ecu_ipc_transfer_poll_send(ecu_ipc_transfer_ctx_t *ctx);
static error_t ecu_ipc_transfer_poll_receive(ecu_ipc_transfer_ctx_t *ctx);

error_t ecu_ipc_init(void)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ecu_ipc_initialized, err = E_INVALACT);

    for(unsigned i = 0; i < ECU_IPC_BUFFER_MAX; i++) {
      err = ecu_ipc_buffer_init(i);
      BREAK_IF(err != E_OK);
    }
    BREAK_IF(err != E_OK);

    for(unsigned i = 0; i < ECU_IPC_TRANSFER_MAX; i++) {
      err = ecu_ipc_transfer_init(i);
      BREAK_IF(err != E_OK);
    }
    BREAK_IF(err != E_OK);

    ecu_ipc_initialized = true;

  } while(0);

  return err;
}

void ecu_ipc_start(void)
{
  if(ecu_ipc_initialized) {
    ATOMIC_STORE(ecu_ipc_started, true);
  }
}

void ecu_ipc_loop_comm(void)
{
  if(ATOMIC_LOAD(ecu_ipc_started)) {
    for(unsigned i = 0; i < ECU_IPC_BUFFER_MAX; i++) {
      ecu_ipc_buffer_loop_comm(&ecu_ipc_buffers[i]);
    }
    for(unsigned i = 0; i < ECU_IPC_TRANSFER_MAX; i++) {
      ecu_ipc_transfer_loop_comm(&ecu_ipc_transfers[i]);
    }
  }
}

error_t ecu_ipc_buffer_send(ecu_ipc_buffer_t instance, const void *buffer, uint32_t size)
{
  error_t err = E_OK;
  ecu_ipc_buffer_ctx_t *ctx;

  do {
    BREAK_IF_ACTION((uint32_t)instance >= ECU_IPC_BUFFER_MAX || buffer == NULL || size == 0u, err = E_PARAM);
    BREAK_IF_ACTION(ATOMIC_LOAD(ecu_ipc_started) == false, err = E_NOTRDY);
    ctx = &ecu_ipc_buffers[instance];
    BREAK_IF_ACTION(ctx->sender == false, err = E_INVALACT);
    BREAK_IF_ACTION(size >= ctx->size, err = E_OVERFLOW);
    BREAK_IF_ACTION(ATOMIC_TRY_LOCK(ctx->job.lock) == false, err = E_AGAIN);

    err = ecu_ipc_poll_job(&ctx->job, buffer, NULL, size, 0u);

    ATOMIC_RELEASE(ctx->job.lock);

  } while(0);

  return err;
}

error_t ecu_ipc_buffer_receive(ecu_ipc_buffer_t instance, void *buffer, uint32_t *p_size)
{
  error_t err = E_OK;
  ecu_ipc_buffer_ctx_t *ctx;

  do {
    BREAK_IF_ACTION((uint32_t)instance >= ECU_IPC_BUFFER_MAX || buffer == NULL || p_size == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ATOMIC_LOAD(ecu_ipc_started) == false, err = E_NOTRDY);
    ctx = &ecu_ipc_buffers[instance];
    BREAK_IF_ACTION(ctx->sender, err = E_INVALACT);
    BREAK_IF_ACTION(ATOMIC_TRY_LOCK(ctx->job.lock) == false, err = E_AGAIN);

    err = ecu_ipc_poll_job(&ctx->job, NULL, buffer, *p_size, 0u);
    if(err == E_OK) {
      *p_size = ctx->job.received;
    }

    ATOMIC_RELEASE(ctx->job.lock);

  } while(0);

  return err;
}

error_t ecu_ipc_transfer_send(ecu_ipc_transfer_t instance, uint32_t endpoint, const void *buffer, uint32_t size)
{
  error_t err = E_OK;
  ecu_ipc_transfer_ctx_t *ctx;

  do {
    BREAK_IF_ACTION((uint32_t)instance >= ECU_IPC_TRANSFER_MAX || buffer == NULL || size == 0u, err = E_PARAM);
    BREAK_IF_ACTION(ATOMIC_LOAD(ecu_ipc_started) == false, err = E_NOTRDY);
    ctx = &ecu_ipc_transfers[instance];
    BREAK_IF_ACTION(ctx->sender == false, err = E_INVALACT);
    BREAK_IF_ACTION(size > ctx->size, err = E_OVERFLOW);
    BREAK_IF_ACTION(ATOMIC_TRY_LOCK(ctx->job.lock) == false, err = E_AGAIN);

    err = ecu_ipc_poll_job(&ctx->job, buffer, NULL, size, endpoint);

    ATOMIC_RELEASE(ctx->job.lock);

  } while(0);

  return err;
}

error_t ecu_ipc_transfer_receive(ecu_ipc_transfer_t instance, const void **p_buffer, uint32_t *p_size, uint32_t *p_endpoint)
{
  error_t err = E_OK;
  ecu_ipc_transfer_ctx_t *ctx;

  do {
    BREAK_IF_ACTION((uint32_t)instance >= ECU_IPC_TRANSFER_MAX || p_buffer == NULL ||
        p_size == NULL || p_endpoint == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ATOMIC_LOAD(ecu_ipc_started) == false, err = E_NOTRDY);
    ctx = &ecu_ipc_transfers[instance];
    BREAK_IF_ACTION(ctx->sender, err = E_INVALACT);
    BREAK_IF_ACTION(ATOMIC_TRY_LOCK(ctx->job.lock) == false, err = E_AGAIN);

    err = ecu_ipc_poll_job(&ctx->job, NULL, NULL, 0u, 0u);
    if(err == E_OK) {
      *p_buffer = ctx->buffer;
      *p_size = ctx->job.received;
      *p_endpoint = ctx->received_endpoint;
    }

    ATOMIC_RELEASE(ctx->job.lock);

  } while(0);

  return err;
}

static error_t ecu_ipc_buffer_init(ecu_ipc_buffer_t instance)
{
  error_t err = E_OK;
  ecu_config_ipc_buffer_t config;
  ecu_ipc_buffer_ctx_t *ctx = &ecu_ipc_buffers[instance];

  do {
    ecu_config_ipc_buffer(instance, &config);
    ctx->sender = config.sender;
    ctx->size = config.size;

    /* Only the COMM worker uses the low-level endpoint. */
    if(ctx->sender) {
      err = ipc_buffer_writer_init(&ctx->ctx.writer, 1u, config.size,
          config.descriptorscount, config.header, config.descriptors, config.buffer);
      BREAK_IF(err != E_OK);
      err = ipc_buffer_writer_lock(&ctx->ctx.writer);
    } else {
      err = ipc_buffer_reader_init(&ctx->ctx.reader, 1u, config.size,
          config.descriptorscount, config.header, config.descriptors, config.buffer);
      BREAK_IF(err != E_OK);
      err = ipc_buffer_reader_lock(&ctx->ctx.reader);
    }

  } while(0);

  return err;
}

static error_t ecu_ipc_transfer_init(ecu_ipc_transfer_t instance)
{
  error_t err = E_OK;
  ecu_config_ipc_transfer_t config;
  ecu_ipc_transfer_ctx_t *ctx = &ecu_ipc_transfers[instance];

  do {
    ecu_config_ipc_transfer(instance, &config);
    ctx->sender = config.sender;
    ctx->size = config.size;
    ctx->buffer = config.buffer;
    ctx->bus_buffer = config.bus_buffer;

    if(ctx->sender) {
      err = ipc_transfer_sender_init(&ctx->ctx.sender, config.header);
      BREAK_IF(err != E_OK);
      err = ipc_transfer_sender_lock(&ctx->ctx.sender);
    } else {
      err = ipc_transfer_receiver_init(&ctx->ctx.receiver, config.header);
      BREAK_IF(err != E_OK);
      err = ipc_transfer_receiver_lock(&ctx->ctx.receiver);
    }

  } while(0);

  return err;
}

static error_t ecu_ipc_poll_job(ecu_ipc_job_t *job, const void *source, void *destination, uint32_t size, uint32_t endpoint)
{
  error_t err = E_AGAIN;

  if(job->state == ECU_IPC_JOB_IDLE) {
    job->source = source;
    job->destination = destination;
    job->size = size;
    job->endpoint = endpoint;
    job->state = ECU_IPC_JOB_PENDING;
  } else if(job->source != source || job->destination != destination ||
      job->size != size || job->endpoint != endpoint) {
    err = E_BUSY;
  } else if(job->state == ECU_IPC_JOB_DONE) {
    err = job->result;
    job->source = NULL;
    job->destination = NULL;
    job->state = ECU_IPC_JOB_IDLE;
  }

  return err;
}

static void ecu_ipc_buffer_loop_comm(ecu_ipc_buffer_ctx_t *ctx)
{
  error_t err;
  uint32_t size;
  ecu_ipc_job_t *job = &ctx->job;

  if(ATOMIC_TRY_LOCK(job->lock)) {
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

static void ecu_ipc_transfer_loop_comm(ecu_ipc_transfer_ctx_t *ctx)
{
  error_t err;
  ecu_ipc_job_t *job = &ctx->job;

  if(ATOMIC_TRY_LOCK(job->lock)) {
    if(job->state == ECU_IPC_JOB_PENDING) {
      err = ctx->sender ? ecu_ipc_transfer_poll_send(ctx) : ecu_ipc_transfer_poll_receive(ctx);
      if(err != E_AGAIN) {
        ctx->state = ECU_IPC_TRANSFER_BEGIN;
        job->result = err;
        job->state = ECU_IPC_JOB_DONE;
      }
    }
    ATOMIC_RELEASE(job->lock);
  }
}

static error_t ecu_ipc_transfer_poll_send(ecu_ipc_transfer_ctx_t *ctx)
{
  error_t err;
  ecu_ipc_job_t *job = &ctx->job;

  switch(ctx->state) {
    case ECU_IPC_TRANSFER_BEGIN:
      err = ipc_transfer_sender_request(&ctx->ctx.sender, job->endpoint, job->source, job->size);
      if(err == E_OK) {
        ctx->state = ECU_IPC_TRANSFER_WAIT;
        err = E_AGAIN;
      }
      break;

    case ECU_IPC_TRANSFER_WAIT:
      err = ipc_transfer_sender_poll_cplt(&ctx->ctx.sender);
      if(err == E_OK) {
        ctx->state = ECU_IPC_TRANSFER_RELEASE;
        err = E_AGAIN;
      }
      break;

    case ECU_IPC_TRANSFER_RELEASE:
      err = ipc_transfer_sender_poll_release(&ctx->ctx.sender);
      break;

    default:
      err = E_FAULT;
      break;
  }

  return err;
}

static error_t ecu_ipc_transfer_poll_receive(ecu_ipc_transfer_ctx_t *ctx)
{
  error_t err;
  uint32_t size;

  switch(ctx->state) {
    case ECU_IPC_TRANSFER_BEGIN:
      size = ctx->size;
      err = ipc_transfer_receiver_poll_for_start(&ctx->ctx.receiver, &ctx->received_endpoint, &size);
      if(err == E_OK) {
        err = ipc_transfer_receiver_send_pointer(&ctx->ctx.receiver, ctx->bus_buffer);
        if(err == E_OK) {
          ctx->job.received = size;
          ctx->state = ECU_IPC_TRANSFER_WAIT;
          err = E_AGAIN;
        }
      }
      break;

    case ECU_IPC_TRANSFER_WAIT:
      err = ipc_transfer_receiver_poll_cplt(&ctx->ctx.receiver);
      if(err == E_OK || err == E_ABORTED) {
        ctx->completion = err;
        ctx->state = ECU_IPC_TRANSFER_RELEASE;
        err = E_AGAIN;
      }
      break;

    case ECU_IPC_TRANSFER_RELEASE:
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
