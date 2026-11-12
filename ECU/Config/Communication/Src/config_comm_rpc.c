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

    err = ipc_rpc_caller_fsm(ctx->caller, &ctx->fsm, ecu_comm_rpc_start, ecu_comm_rpc_pack, NULL, NULL, ctx);

  } while(0);

  return err;
}

static error_t ecu_comm_rpc_read_unpack(uint32_t arg_index, error_t result,
    const void *buffer, uint32_t bytes, uint32_t *pos, void *usrdata)
{
  error_t err = E_OK;
  ecu_comm_rpc_ctx_t *ctx = usrdata;
  uint32_t field_pos;

  do {
    BREAK_IF_ACTION(result != E_OK, err = E_BADRESP);
    if(ctx->response_length == NULL) {
      BREAK_IF_ACTION(ctx->fsm.resp_count != 1u || arg_index != 0u ||
          bytes != ctx->response_bytes, err = E_BADRESP);
      err = ipc_rpc_unpack(buffer, bytes, ctx->response, ctx->response_bytes, pos, ctx->response_alignment);
      break;
    }

    BREAK_IF_ACTION(ctx->fsm.resp_count != 2u, err = E_BADRESP);
    switch(arg_index) {
      case 0:
        err = ipc_rpc_unpack(buffer, bytes, &ctx->read_length, sizeof(ctx->read_length), pos, ALIGNOF(uint16_t));
        BREAK_IF(err != E_OK);
        BREAK_IF_ACTION(ctx->read_length > ctx->read_capacity, err = E_BADRESP);
        break;
      case 1:
        err = ipc_rpc_field_pos(bytes, *pos, ctx->read_length, ctx->response_alignment, &field_pos);
        BREAK_IF(err != E_OK);
        BREAK_IF_ACTION(field_pos + ctx->read_length != bytes, err = E_BADRESP);
        err = ipc_rpc_unpack(buffer, bytes, ctx->response, ctx->read_length, pos, ctx->response_alignment);
        break;
      default:
        err = E_BADRESP;
        break;
    }

  } while(0);

  return err;
}

error_t ecu_comm_rpc_read(ecu_comm_rpc_ctx_t *ctx, ecu_ipc_rpc_func_t func,
    ecu_comm_instance_t instance, void *response, uint32_t response_bytes,
    uint32_t response_alignment, uint16_t *response_length)
{
  error_t err = E_OK;

  do {
    if(ctx->fsm.state == IPC_RPC_CALLER_FSM_IDLE) {
      err = ecu_ipc_get_rpc_caller_ctx(ECU_IPC_RPC_CALLER_1, &ctx->caller);
      BREAK_IF(err != E_OK);
      BREAK_IF_ACTION(ctx->caller == NULL, err = E_FAULT);

      ctx->func = func;
      ctx->instance = instance;
      ctx->response = response;
      ctx->response_bytes = response_bytes;
      ctx->response_alignment = response_alignment;
      ctx->response_length = response_length;
      ctx->read_capacity = response_length != NULL ? *response_length : 0u;
      ctx->argument = response_length != NULL ? &ctx->read_capacity : NULL;
      ctx->bytes = sizeof(ctx->read_capacity);
      ctx->alignment = ALIGNOF(uint16_t);
    } else {
      BREAK_IF_ACTION(ctx->func != func || ctx->instance != instance || ctx->response != response ||
          ctx->response_bytes != response_bytes || ctx->response_alignment != response_alignment ||
          ctx->response_length != response_length ||
          (response_length != NULL && ctx->read_capacity != *response_length), err = E_BUSY);
    }

    err = ipc_rpc_caller_fsm(ctx->caller, &ctx->fsm, ecu_comm_rpc_start, ecu_comm_rpc_pack,
        ecu_comm_rpc_read_unpack, NULL, ctx);
    BREAK_IF_ACTION(err == E_NOTRDY, err = E_AGAIN);
    BREAK_IF(err != E_OK);
    BREAK_IF_ACTION(ctx->fsm.arg_index != (response_length != NULL ? 2u : 1u), err = E_BADRESP);

    if(response_length != NULL) {
      *response_length = ctx->read_length;
    }

  } while(0);

  return err;
}

#endif /* CORE_CM7 */
