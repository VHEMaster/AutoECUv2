/*
 * config_ipc_sharedmemory.c
 *
 *  Created on: 8 нояб. 2026 г.
 *      Author: VHEMaster
 */

#include "config_ipc_sharedmemory.h"
#include "compiler.h"

#define ECU_IPC_SHAREDMEMORY_BUFFER_SIZE (1024u)

#define IPC_SHAREDMEMORY_BUFFER_SECTION SHARED_SECTION_EX("sharedmemory")

static IPC_SHAREDMEMORY_BUFFER_SECTION
uint8_t ecu_config_sharedmemory_buffers[ECU_IPC_SHAREDMEMORY_MAX][ECU_IPC_SHAREDMEMORY_BUFFER_SIZE];

typedef struct {
    ipc_sharedmemory_ctx_t *ctx;
    struct {
        ecu_ipc_sharedmemory_t instance;
        uint32_t size;
        void *buffer;
    }init;
}ecu_ipc_sharedmemory_ctx_t;

static RAM_SECTION ecu_ipc_sharedmemory_ctx_t ecu_ipc_sharedmemory_ctx[ECU_IPC_SHAREDMEMORY_MAX] = {
    {
      .init = {
          .instance = ECU_IPC_SHAREDMEMORY_1,
          .size = sizeof(ecu_config_sharedmemory_buffers[0]),
          .buffer = ecu_config_sharedmemory_buffers[ECU_IPC_SHAREDMEMORY_1],
      },
    }, //ECU_IPC_SHAREDMEMORY_1
    {
      .init = {
          .instance = ECU_IPC_SHAREDMEMORY_2,
          .size = sizeof(ecu_config_sharedmemory_buffers[0]),
          .buffer = ecu_config_sharedmemory_buffers[ECU_IPC_SHAREDMEMORY_2],
      },
    }, //ECU_IPC_SHAREDMEMORY_2
};

error_t ecu_ipc_sharedmemory_init(ecu_ipc_sharedmemory_t instance, ipc_sharedmemory_ctx_t *ctx)
{
  error_t err = E_OK;
  ecu_ipc_sharedmemory_ctx_t *ipc_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_IPC_SHAREDMEMORY_MAX, err = E_PARAM);

    ipc_ctx = &ecu_ipc_sharedmemory_ctx[instance];
    ipc_ctx->ctx = ctx;

    err = ipc_sharedmemory_init(ipc_ctx->ctx, ipc_ctx->init.size, ipc_ctx->init.buffer);
    BREAK_IF(err != E_OK);

  } while(0);

  return err;
}
