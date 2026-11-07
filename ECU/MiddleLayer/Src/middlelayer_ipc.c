/*
 * middlelayer_ipc.c
 */

#include "middlelayer_ipc.h"
#include "ecu_ipc.h"
#include "compiler.h"

void middlelayer_ipc_init(void)
{
  if(ecu_ipc_init() != E_OK) {
    BREAKPOINT(0);
  }
}

void middlelayer_ipc_start(void)
{
  ecu_ipc_start();
}

void middlelayer_ipc_loop_comm(void)
{
  ecu_ipc_loop_comm();
}
