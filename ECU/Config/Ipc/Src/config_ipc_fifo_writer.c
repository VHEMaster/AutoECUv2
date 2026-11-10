/*
 * config_ipc_fifo_writer.c
 *
 *  Created on: 8 нояб. 2026 г.
 *      Author: VHEMaster
 */

#include "config_ipc_fifo_writer.h"
#include "compiler.h"

#define ECU_IPC_FIFO_WRITER_BUFFER_SIZE (128u)

#ifdef CORE_CM7
#define IPC_FIFO_WRITER_HEADER_SECTION SHARED_SECTION_EX("fifo_cm7_to_cm4_headers")
#define IPC_FIFO_WRITER_BUFFER_SECTION SHARED_SECTION_EX("fifo_cm7_to_cm4_buffers")
#endif /* CORE_CM7 */

#ifdef CORE_CM4
#define IPC_FIFO_WRITER_HEADER_SECTION SHARED_SECTION_EX("fifo_cm4_to_cm7_headers")
#define IPC_FIFO_WRITER_BUFFER_SECTION SHARED_SECTION_EX("fifo_cm4_to_cm7_buffers")
#endif /* CORE_CM4 */

static IPC_FIFO_WRITER_HEADER_SECTION
ipc_fifo_header_ctx_t ecu_config_fifo_writer_headers[ECU_IPC_FIFO_WRITER_MAX];

static IPC_FIFO_WRITER_BUFFER_SECTION
uint32_t ecu_config_fifo_writer_buffers[ECU_IPC_FIFO_WRITER_MAX][ECU_IPC_FIFO_WRITER_BUFFER_SIZE];

typedef struct {
    struct {
        ecu_ipc_fifo_writer_t instance;
        uint32_t itemsize;
        uint32_t itemscount;
        ipc_fifo_header_ctx_t *header;
        void *buffer;
    }init;
    ipc_fifo_writer_ctx_t *ctx;
}ecu_ipc_fifo_writer_ctx_t;

static RAM_SECTION ecu_ipc_fifo_writer_ctx_t ecu_ipc_fifo_writer_ctx[ECU_IPC_FIFO_WRITER_MAX] = {
    {
      .init = {
          .instance = ECU_IPC_FIFO_WRITER_1,
          .itemsize = sizeof(ecu_config_fifo_writer_buffers[0][0]),
          .itemscount = ITEMSOF(ecu_config_fifo_writer_buffers[0]),
          .header = &ecu_config_fifo_writer_headers[ECU_IPC_FIFO_WRITER_1],
          .buffer = ecu_config_fifo_writer_buffers[ECU_IPC_FIFO_WRITER_1],
      },
    }, //ECU_IPC_FIFO_WRITER_1
    {
      .init = {
          .instance = ECU_IPC_FIFO_WRITER_2,
          .itemsize = sizeof(ecu_config_fifo_writer_buffers[0][0]),
          .itemscount = ITEMSOF(ecu_config_fifo_writer_buffers[0]),
          .header = &ecu_config_fifo_writer_headers[ECU_IPC_FIFO_WRITER_2],
          .buffer = ecu_config_fifo_writer_buffers[ECU_IPC_FIFO_WRITER_2],
      },
    }, //ECU_IPC_FIFO_WRITER_2
};

error_t ecu_ipc_fifo_writer_init(ecu_ipc_fifo_writer_t instance, ipc_fifo_writer_ctx_t *ctx)
{
  error_t err = E_OK;
  ecu_ipc_fifo_writer_ctx_t *ipc_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_IPC_FIFO_WRITER_MAX || ctx == NULL, err = E_PARAM);

    ipc_ctx = &ecu_ipc_fifo_writer_ctx[instance];
    ipc_ctx->ctx = ctx;

    err = ipc_fifo_writer_init(ipc_ctx->ctx, ipc_ctx->init.itemsize,
        ipc_ctx->init.itemscount, ipc_ctx->init.header, ipc_ctx->init.buffer);
    BREAK_IF(err != E_OK);

  } while(0);

  return err;
}
