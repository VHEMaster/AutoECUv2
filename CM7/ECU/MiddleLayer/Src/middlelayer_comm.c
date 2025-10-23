/*
 * middlelayer_comm.c
 *
 *  Created on: Oct 23, 2025
 *      Author: VHEMaster
 */

#include "middlelayer_comm.h"
#include "config_comm.h"

#include "config_can.h"
#include "config_isotp.h"

#include "compiler.h"

void middlelayer_comm_loop_main(void)
{
  ecu_comm_loop_main();
}

void middlelayer_comm_loop_slow(void)
{
  ecu_comm_loop_slow();
}

void middlelayer_comm_loop_comm(void)
{
  ecu_comm_loop_comm();
}

void middlelayer_comm_init(void)
{
  error_t err = E_OK;
  void *comm_ctx;

  //TODO: Create DTC of comm init
  (void)err;

  do {
    err = ecu_comm_init();

    for(int i = 0; i < ECU_COMM_CAN_MAX; i++) {
      err = ecu_comm_get_comm_ctx(ECU_COMM_TYPE_CAN, i, &comm_ctx);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

      err = ecu_comm_can_init(i, (can_ctx_t *)comm_ctx);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

      err = ecu_comm_set_comm_initialized(ECU_COMM_TYPE_CAN, i, true);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));
    }
    BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

    for(int i = 0; i < ECU_COMM_ISOTP_MAX; i++) {
      err = ecu_comm_get_comm_ctx(ECU_COMM_TYPE_ISOTP, i, &comm_ctx);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

      err = ecu_comm_isotp_init(i, (isotp_ctx_t *)comm_ctx);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

      err = ecu_comm_set_comm_initialized(ECU_COMM_TYPE_ISOTP, i, true);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));
    }
    BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

  } while(0);
}
