/*
 * middlelayer_ipc.c
 *
 *  Created on: 7 нояб. 2026 г.
 *      Author: VHEMaster
 */

#include "middlelayer_ipc.h"
#include "config_ipc.h"

#include "config_ipc_sharedmemory.h"
#include "config_ipc_fifo_writer.h"
#include "config_ipc_fifo_reader.h"
#include "config_ipc_buffer_writer.h"
#include "config_ipc_buffer_reader.h"
#include "config_ipc_transfer_sender.h"
#include "config_ipc_transfer_receiver.h"

#include "compiler.h"

void middlelayer_ipc_init(void)
{
  error_t err = E_OK;
  void *ipc_ctx;

  //TODO: Create DTC of IPC init
  (void)err;

  do {
    err = ecu_ipc_init();
    BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

    for(int i = 0; i < ECU_IPC_SHAREDMEMORY_MAX; i++) {
      err = ecu_ipc_get_ipc_ctx(ECU_IPC_TYPE_SHAREDMEMORY, i, &ipc_ctx);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

      err = ecu_ipc_sharedmemory_init(i, (ipc_sharedmemory_ctx_t *) ipc_ctx);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));
    }
    BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

    for(int i = 0; i < ECU_IPC_FIFO_WRITER_MAX; i++) {
      err = ecu_ipc_get_ipc_ctx(ECU_IPC_TYPE_FIFO_WRITER, i, &ipc_ctx);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

      err = ecu_ipc_fifo_writer_init(i, (ipc_fifo_writer_ctx_t *)ipc_ctx);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));
    }
    BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

    for(int i = 0; i < ECU_IPC_FIFO_READER_MAX; i++) {
      err = ecu_ipc_get_ipc_ctx(ECU_IPC_TYPE_FIFO_READER, i, &ipc_ctx);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

      err = ecu_ipc_fifo_reader_init(i, (ipc_fifo_reader_ctx_t *)ipc_ctx);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));
    }
    BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

    for(int i = 0; i < ECU_IPC_BUFFER_WRITER_MAX; i++) {
      err = ecu_ipc_get_ipc_ctx(ECU_IPC_TYPE_BUFFER_WRITER, i, &ipc_ctx);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

      err = ecu_ipc_buffer_writer_init(i, (ipc_buffer_writer_ctx_t *)ipc_ctx);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));
    }
    BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

    for(int i = 0; i < ECU_IPC_BUFFER_READER_MAX; i++) {
      err = ecu_ipc_get_ipc_ctx(ECU_IPC_TYPE_BUFFER_READER, i, &ipc_ctx);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

      err = ecu_ipc_buffer_reader_init(i, (ipc_buffer_reader_ctx_t *)ipc_ctx);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));
    }
    BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

    for(int i = 0; i < ECU_IPC_TRANSFER_SENDER_MAX; i++) {
      err = ecu_ipc_get_ipc_ctx(ECU_IPC_TYPE_TRANSFER_SENDER, i, &ipc_ctx);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

      err = ecu_ipc_transfer_sender_init(i, (ipc_transfer_sender_ctx_t *)ipc_ctx);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));
    }
    BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

    for(int i = 0; i < ECU_IPC_TRANSFER_RECEIVER_MAX; i++) {
      err = ecu_ipc_get_ipc_ctx(ECU_IPC_TYPE_TRANSFER_RECEIVER, i, &ipc_ctx);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

      err = ecu_ipc_transfer_receiver_init(i, (ipc_transfer_receiver_ctx_t *)ipc_ctx);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));
    }
    BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

  } while(0);
}
