/*
 * config_comm_rpc.c
 *
 *  Created on: 11 нояб. 2026 г.
 */


#include "config_comm_rpc.h"
#include "config_ipc.h"
#include "ipc_rpc_pack.h"

#ifdef CORE_CM7

static error_t ecu_comm_rpc_start(uint32_t *func_id, uint32_t *args_count, void *usrdata)
{
  ecu_comm_rpc_ctx_t *ctx = usrdata;

  *func_id = ctx->func;
  *args_count = ctx->argument == NULL ? 1u : 2u;

  return E_OK;
}

static error_t ecu_comm_rpc_pack(uint32_t arg_index, void *buffer, uint32_t capacity, uint32_t *pos, void *usrdata)
{
  ecu_comm_rpc_ctx_t *ctx = usrdata;

  switch(arg_index) {
    case 0:
      return ipc_rpc_pack(buffer, capacity, &ctx->instance, sizeof(ctx->instance), pos, ALIGNOF(ecu_comm_instance_t));
    case 1:
      RETURN_IF(ctx->argument == NULL, E_PARAM);
      return ipc_rpc_pack(buffer, capacity, ctx->argument, ctx->bytes, pos, ctx->alignment);
    default:
      return E_PARAM;
  }
}

error_t ecu_comm_rpc_call(ecu_comm_rpc_ctx_t *ctx, ecu_ipc_rpc_func_t func,
    ecu_comm_instance_t instance, const void *argument, uint32_t bytes, uint32_t alignment)
{
  error_t err = E_OK;

  do {
    if(ctx->fsm.state == IPC_RPC_CALLER_FSM_IDLE) {
      err = ecu_ipc_get_rpc_caller_ctx(ECU_IPC_RPC_CALLER_1, &ctx->caller);
      BREAK_IF(err != E_OK);
      BREAK_IF_ACTION(ctx->caller == NULL, err = E_FAULT);

      ctx->func = func;
      ctx->instance = instance;
      ctx->argument = argument;
      ctx->bytes = bytes;
      ctx->alignment = alignment;
    } else {
      BREAK_IF_ACTION(ctx->func != func || ctx->instance != instance ||
          ctx->argument != argument || ctx->bytes != bytes || ctx->alignment != alignment, err = E_BUSY);
    }

    err = ipc_rpc_caller_fsm(ctx->caller, &ctx->fsm, ecu_comm_rpc_start, ecu_comm_rpc_pack, NULL, ctx);

  } while(0);

  return err;
}

#endif /* CORE_CM7 */
