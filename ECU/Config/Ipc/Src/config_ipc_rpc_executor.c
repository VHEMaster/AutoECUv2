/*
 * config_ipc_rpc_executor.c
 *
 *  Created on: 10 нояб. 2026 г.
 *      Author: VHEMaster
 */

#include "config_ipc_rpc_executor.h"
#include "compiler.h"

#define ECU_IPC_RPC_EXECUTOR_BUFFER_SIZE (8192u)

#ifdef CORE_CM7
#define IPC_RPC_EXECUTOR_HEADER_SECTION SHARED_SECTION_EX("rpc_cm4_to_cm7_headers")
#define IPC_RPC_EXECUTOR_BUFFER_SECTION SHARED_SECTION_EX("rpc_cm4_to_cm7_buffers")
#endif /* CORE_CM7 */

#ifdef CORE_CM4
#define IPC_RPC_EXECUTOR_HEADER_SECTION SHARED_SECTION_EX("rpc_cm7_to_cm4_headers")
#define IPC_RPC_EXECUTOR_BUFFER_SECTION SHARED_SECTION_EX("rpc_cm7_to_cm4_buffers")
#endif /* CORE_CM4 */

static IPC_RPC_EXECUTOR_HEADER_SECTION
ipc_rpc_header_ctx_t ecu_config_rpc_executor_headers[ECU_IPC_RPC_EXECUTOR_MAX];

static IPC_RPC_EXECUTOR_BUFFER_SECTION
uint8_t ecu_config_rpc_executor_buffers[ECU_IPC_RPC_EXECUTOR_MAX][ECU_IPC_RPC_EXECUTOR_BUFFER_SIZE];

typedef struct {
    struct {
        ecu_ipc_rpc_executor_t instance;
        uint32_t buffersize;
        ipc_rpc_header_ctx_t *header;
        void *buffer;
    }init;
    ipc_rpc_executor_ctx_t *ctx;
}ecu_ipc_rpc_executor_ctx_t;

static RAM_SECTION ecu_ipc_rpc_executor_ctx_t ecu_ipc_rpc_executor_ctx[ECU_IPC_RPC_EXECUTOR_MAX] = {
    {
      .init = {
          .instance = ECU_IPC_RPC_EXECUTOR_1,
          .buffersize = sizeof(ecu_config_rpc_executor_buffers[0]),
          .header = &ecu_config_rpc_executor_headers[ECU_IPC_RPC_EXECUTOR_1],
          .buffer = ecu_config_rpc_executor_buffers[ECU_IPC_RPC_EXECUTOR_1],
      },
    }, //ECU_IPC_RPC_EXECUTOR_1
};

error_t ecu_ipc_rpc_executor_init(ecu_ipc_rpc_executor_t instance, ipc_rpc_executor_ctx_t *ctx)
{
  error_t err = E_OK;
  ecu_ipc_rpc_executor_ctx_t *ipc_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_IPC_RPC_EXECUTOR_MAX || ctx == NULL, err = E_PARAM);

    ipc_ctx = &ecu_ipc_rpc_executor_ctx[instance];
    ipc_ctx->ctx = ctx;

    err = ipc_rpc_executor_init(ipc_ctx->ctx, ipc_ctx->init.header,
        ipc_ctx->init.buffersize, ipc_ctx->init.buffer);
    BREAK_IF(err != E_OK);

  } while(0);

  return err;
}
