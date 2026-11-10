/*
 * config_ipc.c
 *
 *  Created on: 7 нояб. 2026 г.
 *      Author: VHEMaster
 */

#include "config_ipc.h"
#include "config_global.h"
#include "compiler.h"
#include "bool.h"

typedef enum {
  ECU_IPC_LOOP_TYPE_MAIN = 0,
  ECU_IPC_LOOP_TYPE_SLOW,
  ECU_IPC_LOOP_TYPE_COMM,
  ECU_IPC_LOOP_TYPE_FAST
}ecu_config_ipc_loop_type_t;

typedef struct {
    bool initialized;
}ecu_config_ipc_instance_ctx_t;

typedef struct {
    ecu_ipc_type_t type;
    ecu_ipc_instance_t instance;
    void *ctx;
    bool initialized;
}ecu_config_ipc_config_ctx_t;

typedef struct {
    ecu_ipc_instance_t instance_max;
}ecu_config_ipc_if_config_ctx_t;

typedef struct {
    uint32_t instance_first;
}ecu_config_ipc_if_instance_ctx_t;


typedef struct {
  ecu_config_ipc_if_config_ctx_t interfaces[ECU_IPC_TYPE_MAX];
  ecu_config_ipc_config_ctx_t ipc[ECU_IPCS_MAX];
}ecu_config_ipc_config_t;

typedef struct {
    const ecu_config_ipc_config_t *config;
    ecu_config_ipc_if_instance_ctx_t interfaces[ECU_IPC_TYPE_MAX];
    ecu_config_ipc_instance_ctx_t ipc[ECU_IPCS_MAX];
}ecu_config_ipc_t;

static ipc_sharedmemory_ctx_t ecu_config_sharedmemory_ctx[ECU_IPC_SHAREDMEMORY_MAX];
static ipc_fifo_writer_ctx_t ecu_config_fifo_writer_ctx[ECU_IPC_FIFO_WRITER_MAX];
static ipc_fifo_reader_ctx_t ecu_config_fifo_reader_ctx[ECU_IPC_FIFO_READER_MAX];
static ipc_buffer_writer_ctx_t ecu_config_buffer_writer_ctx[ECU_IPC_BUFFER_WRITER_MAX];
static ipc_buffer_reader_ctx_t ecu_config_buffer_reader_ctx[ECU_IPC_BUFFER_READER_MAX];
static ipc_transfer_sender_ctx_t ecu_config_transfer_sender_ctx[ECU_IPC_TRANSFER_SENDER_MAX];
static ipc_transfer_receiver_ctx_t ecu_config_transfer_receiver_ctx[ECU_IPC_TRANSFER_RECEIVER_MAX];
static ipc_rpc_caller_ctx_t ecu_config_rpc_caller_ctx[ECU_IPC_RPC_CALLER_MAX];
static ipc_rpc_executor_ctx_t ecu_config_rpc_executor_ctx[ECU_IPC_RPC_EXECUTOR_MAX];

static const ecu_config_ipc_config_t ecu_config_ipc = {
    .interfaces = {
        {
            .instance_max = ECU_IPC_SHAREDMEMORY_MAX,
        }, //ECU_IPC_TYPE_SHAREDMEMORY
        {
            .instance_max = ECU_IPC_FIFO_WRITER_MAX,
        }, //ECU_IPC_TYPE_FIFO_WRITER
        {
            .instance_max = ECU_IPC_FIFO_READER_MAX,
        }, //ECU_IPC_TYPE_FIFO_READER
        {
            .instance_max = ECU_IPC_BUFFER_WRITER_MAX,
        }, //ECU_IPC_TYPE_BUFFER_WRITER
        {
            .instance_max = ECU_IPC_BUFFER_READER_MAX,
        }, //ECU_IPC_TYPE_BUFFER_READER
        {
            .instance_max = ECU_IPC_TRANSFER_SENDER_MAX,
        }, //ECU_IPC_TYPE_TRANSFER_SENDER
        {
            .instance_max = ECU_IPC_TRANSFER_RECEIVER_MAX,
        }, //ECU_IPC_TYPE_TRANSFER_RECEIVER
        {
            .instance_max = ECU_IPC_RPC_CALLER_MAX,
        }, //ECU_IPC_TYPE_RPC_CALLER
        {
            .instance_max = ECU_IPC_RPC_EXECUTOR_MAX,
        }, //ECU_IPC_TYPE_RPC_EXECUTOR
    },
    .ipc = {
        {
            .type = ECU_IPC_TYPE_SHAREDMEMORY,
            .instance = ECU_IPC_SHAREDMEMORY_1,
            .ctx = &ecu_config_sharedmemory_ctx[ECU_IPC_SHAREDMEMORY_1],
        },
        {
            .type = ECU_IPC_TYPE_SHAREDMEMORY,
            .instance = ECU_IPC_SHAREDMEMORY_2,
            .ctx = &ecu_config_sharedmemory_ctx[ECU_IPC_SHAREDMEMORY_2],
        },
        {
            .type = ECU_IPC_TYPE_FIFO_WRITER,
            .instance = ECU_IPC_FIFO_WRITER_1,
            .ctx = &ecu_config_fifo_writer_ctx[ECU_IPC_FIFO_WRITER_1],
        },
        {
            .type = ECU_IPC_TYPE_FIFO_WRITER,
            .instance = ECU_IPC_FIFO_WRITER_2,
            .ctx = &ecu_config_fifo_writer_ctx[ECU_IPC_FIFO_WRITER_2],
        },
        {
            .type = ECU_IPC_TYPE_FIFO_READER,
            .instance = ECU_IPC_FIFO_READER_1,
            .ctx = &ecu_config_fifo_reader_ctx[ECU_IPC_FIFO_READER_1],
        },
        {
            .type = ECU_IPC_TYPE_FIFO_READER,
            .instance = ECU_IPC_FIFO_READER_2,
            .ctx = &ecu_config_fifo_reader_ctx[ECU_IPC_FIFO_READER_2],
        },
        {
            .type = ECU_IPC_TYPE_BUFFER_WRITER,
            .instance = ECU_IPC_BUFFER_WRITER_1,
            .ctx = &ecu_config_buffer_writer_ctx[ECU_IPC_BUFFER_WRITER_1],
        },
        {
            .type = ECU_IPC_TYPE_BUFFER_WRITER,
            .instance = ECU_IPC_BUFFER_WRITER_2,
            .ctx = &ecu_config_buffer_writer_ctx[ECU_IPC_BUFFER_WRITER_2],
        },
        {
            .type = ECU_IPC_TYPE_BUFFER_READER,
            .instance = ECU_IPC_BUFFER_READER_1,
            .ctx = &ecu_config_buffer_reader_ctx[ECU_IPC_BUFFER_READER_1],
        },
        {
            .type = ECU_IPC_TYPE_BUFFER_READER,
            .instance = ECU_IPC_BUFFER_READER_2,
            .ctx = &ecu_config_buffer_reader_ctx[ECU_IPC_BUFFER_READER_2],
        },
        {
            .type = ECU_IPC_TYPE_TRANSFER_SENDER,
            .instance = ECU_IPC_TRANSFER_SENDER_1,
            .ctx = &ecu_config_transfer_sender_ctx[ECU_IPC_TRANSFER_SENDER_1],
        },
        {
            .type = ECU_IPC_TYPE_TRANSFER_SENDER,
            .instance = ECU_IPC_TRANSFER_SENDER_2,
            .ctx = &ecu_config_transfer_sender_ctx[ECU_IPC_TRANSFER_SENDER_2],
        },
        {
            .type = ECU_IPC_TYPE_TRANSFER_RECEIVER,
            .instance = ECU_IPC_TRANSFER_RECEIVER_1,
            .ctx = &ecu_config_transfer_receiver_ctx[ECU_IPC_TRANSFER_RECEIVER_1],
        },
        {
            .type = ECU_IPC_TYPE_TRANSFER_RECEIVER,
            .instance = ECU_IPC_TRANSFER_RECEIVER_2,
            .ctx = &ecu_config_transfer_receiver_ctx[ECU_IPC_TRANSFER_RECEIVER_2],
        },
        {
            .type = ECU_IPC_TYPE_RPC_CALLER,
            .instance = ECU_IPC_RPC_CALLER_1,
            .ctx = &ecu_config_rpc_caller_ctx[ECU_IPC_RPC_CALLER_1],
        },
        {
            .type = ECU_IPC_TYPE_RPC_EXECUTOR,
            .instance = ECU_IPC_RPC_EXECUTOR_1,
            .ctx = &ecu_config_rpc_executor_ctx[ECU_IPC_RPC_EXECUTOR_1],
        },
    },
};

static RAM_SECTION ecu_config_ipc_t ecu_config_ipc_ctx = {
    .config = &ecu_config_ipc,
};

error_t ecu_ipc_init(void)
{
  error_t err = E_OK;
  const ecu_config_ipc_if_config_ctx_t *interface_config;
  const ecu_config_ipc_config_ctx_t *ipc_config;
  ecu_config_ipc_if_instance_ctx_t *interface_ctx;
  ecu_config_ipc_instance_ctx_t *ipc_ctx;

  for(int i = 0; i < ECU_IPCS_MAX; i++) {
    ipc_config = &ecu_config_ipc.ipc[i];
    ipc_ctx = &ecu_config_ipc_ctx.ipc[i];
    ipc_ctx->initialized = false;

    BREAK_IF_ACTION(ipc_config->type >= ECU_IPC_TYPE_MAX, err = E_FAULT);
    BREAK_IF_ACTION(ipc_config->ctx == NULL, err = E_FAULT);

    interface_config = &ecu_config_ipc.interfaces[ipc_config->type];
    BREAK_IF_ACTION(ipc_config->instance >= interface_config->instance_max, err = E_FAULT);
  }

  for(int i = 0; i < ITEMSOF(ecu_config_ipc.interfaces); i++) {
    interface_config = &ecu_config_ipc.interfaces[i];
    interface_ctx = &ecu_config_ipc_ctx.interfaces[i];

    BREAK_IF_ACTION(interface_config->instance_max > ECU_IPC_INSTANCE_MAX, err = E_FAULT);
    interface_ctx->instance_first = ECU_IPCS_MAX;
    for(int n = 0; n < ECU_IPCS_MAX; n++) {
      ipc_config = &ecu_config_ipc.ipc[n];
      if(ipc_config->type == i) {
        BREAK_IF_ACTION(ipc_config->instance != 0, err = E_FAULT);
        interface_ctx->instance_first = n;
        break;
      }
    }
    BREAK_IF(err != E_OK);
  }

  return err;
}

error_t ecu_ipc_get_ipc_ctx(ecu_ipc_type_t type, ecu_ipc_instance_t instance, void **ctx)
{
  error_t err = E_FAULT;
  const ecu_config_ipc_if_instance_ctx_t *interface;
  const ecu_config_ipc_if_config_ctx_t *interface_config;
  const ecu_config_ipc_config_ctx_t *ipc_config;

  do {
    BREAK_IF(type >= ECU_IPC_TYPE_MAX);
    interface_config = &ecu_config_ipc.interfaces[type];
    interface = &ecu_config_ipc_ctx.interfaces[type];
    BREAK_IF(instance >= interface_config->instance_max);
    BREAK_IF(interface->instance_first >= ECU_IPCS_MAX);

    ipc_config = &ecu_config_ipc.ipc[interface->instance_first];
    ipc_config = &ipc_config[instance];
    *ctx = ipc_config->ctx;
    err = E_OK;
  } while(0);

  return err;
}

error_t ecu_ipc_get_type_max(ecu_ipc_type_t *type_max)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(type_max == NULL, err = E_PARAM);

    *type_max = ECU_IPC_TYPE_MAX;

  } while(0);

  return err;
}

error_t ecu_ipc_get_instance_max(ecu_ipc_type_t type, ecu_ipc_instance_t *instance_max)
{
  error_t err = E_OK;
  const ecu_config_ipc_if_config_ctx_t *interface_config;

  do {
    BREAK_IF_ACTION(type >= ECU_IPC_TYPE_MAX, err = E_PARAM);
    BREAK_IF_ACTION(instance_max == NULL, err = E_PARAM);

    interface_config = &ecu_config_ipc.interfaces[type];
    *instance_max = interface_config->instance_max;

  } while(0);

  return err;
}

error_t ecu_ipc_get_sharedmemory_ctx(ecu_ipc_sharedmemory_t instance, ipc_sharedmemory_ctx_t **ctx)
{
  return ecu_ipc_get_ipc_ctx(ECU_IPC_TYPE_SHAREDMEMORY, instance, (void**)ctx);
}

error_t ecu_ipc_get_fifo_writer_ctx(ecu_ipc_fifo_writer_t instance, ipc_fifo_writer_ctx_t **ctx)
{
  return ecu_ipc_get_ipc_ctx(ECU_IPC_TYPE_FIFO_WRITER, instance, (void**)ctx);
}

error_t ecu_ipc_get_fifo_reader_ctx(ecu_ipc_fifo_reader_t instance, ipc_fifo_reader_ctx_t **ctx)
{
  return ecu_ipc_get_ipc_ctx(ECU_IPC_TYPE_FIFO_READER, instance, (void**)ctx);
}

error_t ecu_ipc_get_buffer_writer_ctx(ecu_ipc_buffer_writer_t instance, ipc_buffer_writer_ctx_t **ctx)
{
  return ecu_ipc_get_ipc_ctx(ECU_IPC_TYPE_BUFFER_WRITER, instance, (void**)ctx);
}

error_t ecu_ipc_get_buffer_reader_ctx(ecu_ipc_buffer_reader_t instance, ipc_buffer_reader_ctx_t **ctx)
{
  return ecu_ipc_get_ipc_ctx(ECU_IPC_TYPE_BUFFER_READER, instance, (void**)ctx);
}

error_t ecu_ipc_get_transfer_sender_ctx(ecu_ipc_transfer_sender_t instance, ipc_transfer_sender_ctx_t **ctx)
{
  return ecu_ipc_get_ipc_ctx(ECU_IPC_TYPE_TRANSFER_SENDER, instance, (void**)ctx);
}

error_t ecu_ipc_get_transfer_receiver_ctx(ecu_ipc_transfer_receiver_t instance, ipc_transfer_receiver_ctx_t **ctx)
{
  return ecu_ipc_get_ipc_ctx(ECU_IPC_TYPE_TRANSFER_RECEIVER, instance, (void**)ctx);
}

error_t ecu_ipc_get_rpc_caller_ctx(ecu_ipc_rpc_caller_t instance, ipc_rpc_caller_ctx_t **ctx)
{
  return ecu_ipc_get_ipc_ctx(ECU_IPC_TYPE_RPC_CALLER, instance, (void**)ctx);
}

error_t ecu_ipc_get_rpc_executor_ctx(ecu_ipc_rpc_executor_t instance, ipc_rpc_executor_ctx_t **ctx)
{
  return ecu_ipc_get_ipc_ctx(ECU_IPC_TYPE_RPC_EXECUTOR, instance, (void**)ctx);
}
