/*
 * config_ipc_transfer_receiver.c
 *
 *  Created on: 8 нояб. 2026 г.
 *      Author: VHEMaster
 */

#include "config_ipc_transfer_receiver.h"
#include "compiler.h"

#ifdef CORE_CM7
#define IPC_TRANSFER_RECEIVER_HEADER_SECTION        SHARED_SECTION_EX("transfer_cm4_to_cm7_headers")
#endif /* CORE_CM7 */

#ifdef CORE_CM4
#define IPC_TRANSFER_RECEIVER_HEADER_SECTION        SHARED_SECTION_EX("transfer_cm7_to_cm4_headers")
#endif /* CORE_CM4 */

static IPC_TRANSFER_RECEIVER_HEADER_SECTION
ipc_transfer_header_ctx_t ecu_config_transfer_receiver_headers[ECU_IPC_TRANSFER_RECEIVER_MAX];

typedef struct {
    struct {
        ecu_ipc_transfer_receiver_t instance;
        ipc_transfer_header_ctx_t *header;
        void *transfer;
    }init;
    ipc_transfer_receiver_ctx_t *ctx;
}ecu_ipc_transfer_receiver_ctx_t;

static RAM_SECTION ecu_ipc_transfer_receiver_ctx_t ecu_ipc_transfer_receiver_ctx[ECU_IPC_TRANSFER_RECEIVER_MAX] = {
    {
      .init = {
          .instance = ECU_IPC_TRANSFER_RECEIVER_1,
          .header = &ecu_config_transfer_receiver_headers[ECU_IPC_TRANSFER_RECEIVER_1],
      },
    }, //ECU_IPC_TRANSFER_RECEIVER_1
    {
      .init = {
          .instance = ECU_IPC_TRANSFER_RECEIVER_2,
          .header = &ecu_config_transfer_receiver_headers[ECU_IPC_TRANSFER_RECEIVER_2],
      },
    }, //ECU_IPC_TRANSFER_RECEIVER_2
};

error_t ecu_ipc_transfer_receiver_init(ecu_ipc_transfer_receiver_t instance, ipc_transfer_receiver_ctx_t *ctx)
{
  error_t err = E_OK;
  ecu_ipc_transfer_receiver_ctx_t *ipc_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_IPC_TRANSFER_RECEIVER_MAX || ctx == NULL, err = E_PARAM);

    ipc_ctx = &ecu_ipc_transfer_receiver_ctx[instance];
    ipc_ctx->ctx = ctx;

    err = ipc_transfer_receiver_init(ipc_ctx->ctx, ipc_ctx->init.header);
    BREAK_IF(err != E_OK);

  } while(0);

  return err;
}
