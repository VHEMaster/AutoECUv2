/*
 * middlelayer_ipc.c
 *
 *  Created on: 7 нояб. 2026 г.
 *      Author: VHEMaster
 */

#include <string.h>
#include "middlelayer_ipc.h"
#include "compiler.h"
#include "config_ipc.h"

typedef struct {

}middlelayer_ipc_ctx_t;

static middlelayer_ipc_ctx_t middlelayer_ipc_ctx = {0};

void middlelayer_ipc_init(void)
{
  error_t err = E_OK;

  //TODO: Create DTC of IPC init
  (void)err;

  memset(&middlelayer_ipc_ctx, 0u, sizeof(middlelayer_ipc_ctx));

}

void middlelayer_ipc_loop_main(void)
{
  // Nothing to do here
}

void middlelayer_ipc_loop_slow(void)
{
  // Nothing to do here
}

void middlelayer_ipc_loop_comm(void)
{
  // TODO: implement
}

ITCM_FUNC void middlelayer_ipc_loop_fast(void)
{
 // Nothing to do here
}
