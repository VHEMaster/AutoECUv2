/*
 * config_global_ipc.c
 *
 *  Created on: Nov 23, 2026
 */

#include "config_global_ipc.h"
#include "config_ipc.h"
#include "ipc_rpc_pack.h"
#ifdef CORE_CM7
#include "config_ipc_rpc_executor.h"
#endif /* CORE_CM7 */
#ifdef CORE_CM4
#include "ipc_rpc_caller_fsm.h"
#endif /* CORE_CM4 */

typedef struct {
    ecu_config_type_t type;
    ecu_index_type_t index;
    ecu_instance_t instance;
    uint32_t offset;
    uint32_t bytes;
}ecu_config_global_rpc_request_t;

typedef struct {
    ecu_config_global_rpc_request_t request;
#ifdef CORE_CM7
    ecu_config_global_runtime_ctx_t *global;
    const void *argument;
    uint32_t response_bytes;
    uint8_t response[ECU_CONFIG_GLOBAL_IPC_BYTES_MAX];
#endif /* CORE_CM7 */
#ifdef CORE_CM4
    ecu_ipc_rpc_func_t func;
    ipc_rpc_caller_ctx_t *caller;
    ipc_rpc_caller_fsm_ctx_t fsm;
    void *response;
    const void *argument;
    uint32_t response_bytes;
#endif /* CORE_CM4 */
}ecu_config_global_rpc_ctx_t;

static ecu_config_global_rpc_ctx_t ecu_config_global_rpc_ctx = {0};

#ifdef CORE_CM7
static error_t ecu_config_global_rpc_item(ecu_config_global_rpc_ctx_t *ctx,
    const ecu_config_global_rpc_request_t *request, const ecu_config_generic_config_t **config,
    ecu_instance_t *instance_max, ecu_index_type_t *item_max)
{
  error_t err = E_OK;
  const ecu_config_device_config_t *items = NULL;
  const ecu_config_generic_config_t *generic_items = NULL;

  do {
    BREAK_IF_ACTION(ctx->global == NULL || ctx->global->global_ready == false, err = E_NOTRDY);

    switch(request->type) {
      case ECU_CONFIG_TYPE_DEVICE:
        items = ctx->global->devices_config;
        *item_max = ctx->global->devices_count;
        break;
      case ECU_CONFIG_TYPE_SENSOR:
        items = ctx->global->sensors_config;
        *item_max = ctx->global->sensors_count;
        break;
      case ECU_CONFIG_TYPE_MODULE:
        items = ctx->global->modules_config;
        *item_max = ctx->global->modules_count;
        break;
      case ECU_CONFIG_TYPE_TIMING:
        items = ctx->global->timings_config;
        *item_max = ctx->global->timings_count;
        break;
      case ECU_CONFIG_TYPE_CALIBRATION:
        generic_items = ctx->global->calibrations_config;
        *item_max = ctx->global->calibrations_count;
        break;
      case ECU_CONFIG_TYPE_RUNTIME:
        generic_items = ctx->global->runtimes_config;
        *item_max = ctx->global->runtimes_count;
        break;
      case ECU_CONFIG_TYPE_COMM:
        items = ctx->global->comms_config;
        *item_max = ctx->global->comms_count;
        break;
      default:
        err = E_PARAM;
        break;
    }
    BREAK_IF(err != E_OK);
    BREAK_IF(config == NULL);
    BREAK_IF_ACTION(request->index >= *item_max, err = E_PARAM);

    if(items != NULL) {
      *instance_max = items[request->index].instances_count;
      *config = &items[request->index].generic;
    } else {
      *instance_max = 1u;
      *config = &generic_items[request->index];
    }

  } while(0);

  return err;
}

static error_t ecu_config_global_rpc_access(ecu_ipc_rpc_func_t func,
    const ecu_config_global_rpc_request_t *request, void *response, const void *argument)
{
  error_t err = E_OK;
  ecu_config_global_rpc_ctx_t *ctx = &ecu_config_global_rpc_ctx;
  const ecu_config_generic_config_t *config;
  ecu_config_global_header_t header;
  ecu_index_type_t item_max;
  ecu_instance_t instance_max;
  ecu_config_type_t type_max = ECU_CONFIG_TYPE_ALL;
  uint8_t *data;

  do {
    BREAK_IF_ACTION(ctx->global == NULL || ctx->global->global_ready == false, err = E_NOTRDY);
    if(func == ECU_IPC_RPC_FUNC_CONFIG_GLOBAL_GET_TYPE_MAX) {
      memcpy(response, &type_max, sizeof(type_max));
      break;
    }

    err = ecu_config_global_rpc_item(ctx, request,
        func == ECU_IPC_RPC_FUNC_CONFIG_GLOBAL_GET_ITEM_MAX ? NULL : &config, &instance_max, &item_max);
    BREAK_IF(err != E_OK);

    if(func == ECU_IPC_RPC_FUNC_CONFIG_GLOBAL_GET_ITEM_MAX) {
      memcpy(response, &item_max, sizeof(item_max));
      break;
    }
    if(func == ECU_IPC_RPC_FUNC_CONFIG_GLOBAL_GET_INSTANCE_MAX) {
      memcpy(response, &instance_max, sizeof(instance_max));
      break;
    }

    BREAK_IF_ACTION(request->instance >= instance_max, err = E_PARAM);
    BREAK_IF_ACTION(config->versions_count == 0u || config->versions_count > ECU_CONFIG_ITEM_VERSIONS_MAX ||
        config->data_ptr == NULL, err = E_FAULT);

    if(func == ECU_IPC_RPC_FUNC_CONFIG_GLOBAL_GET_HEADER) {
      header.version = config->versions[config->versions_count - 1u].version;
      header.size = config->data_size;
      memcpy(response, &header, sizeof(header));
      break;
    }

    BREAK_IF_ACTION(request->bytes > ECU_CONFIG_GLOBAL_IPC_BYTES_MAX || request->offset > config->data_size ||
        request->bytes > config->data_size - request->offset, err = E_PARAM);
    BREAK_IF(request->bytes == 0u);

    data = (uint8_t *)config->data_ptr + (size_t)config->data_size * request->instance + request->offset;
    switch(func) {
      case ECU_IPC_RPC_FUNC_CONFIG_GLOBAL_READ:
        memcpy(response, data, request->bytes);
        break;
      case ECU_IPC_RPC_FUNC_CONFIG_GLOBAL_WRITE:
        memcpy(data, argument, request->bytes);
        break;
      default:
        err = E_PARAM;
        break;
    }

  } while(0);

  return err;
}

static error_t ecu_config_global_rpc_unpack(uint32_t arg_index, const void *buffer,
    uint32_t capacity, uint32_t *p_pos, void *usrdata)
{
  ecu_config_global_rpc_ctx_t *ctx = usrdata;

  switch(arg_index) {
    case 0:
      ctx->argument = NULL;
      return ipc_rpc_unpack(buffer, capacity, &ctx->request, sizeof(ctx->request), p_pos,
          ALIGNOF(ecu_config_global_rpc_request_t));
    case 1:
      RETURN_IF(ctx->request.bytes > ECU_CONFIG_GLOBAL_IPC_BYTES_MAX, E_PARAM);
      return ipc_rpc_unpack_ptr(buffer, capacity, &ctx->argument, ctx->request.bytes, p_pos, 1u);
    default:
      return E_PARAM;
  }
}

static error_t ecu_config_global_rpc_execute(ecu_config_global_rpc_ctx_t *ctx,
    ecu_ipc_rpc_func_t func, uint32_t response_bytes, uint32_t *p_resp_count, error_t *p_result)
{
  ctx->response_bytes = response_bytes;
  *p_result = ecu_config_global_rpc_access(func, &ctx->request, ctx->response, ctx->argument);
  *p_resp_count = *p_result == E_OK && func != ECU_IPC_RPC_FUNC_CONFIG_GLOBAL_WRITE ? 1u : 0u;

  return E_OK;
}

static error_t ecu_config_global_rpc_get_type_max_execute(uint32_t *p_resp_count, error_t *p_result, void *usrdata)
{
  ecu_config_global_rpc_ctx_t *ctx = usrdata;

  return ecu_config_global_rpc_execute(ctx, ECU_IPC_RPC_FUNC_CONFIG_GLOBAL_GET_TYPE_MAX,
      sizeof(ecu_config_type_t), p_resp_count, p_result);
}

static error_t ecu_config_global_rpc_get_item_max_execute(uint32_t *p_resp_count, error_t *p_result, void *usrdata)
{
  ecu_config_global_rpc_ctx_t *ctx = usrdata;

  return ecu_config_global_rpc_execute(ctx, ECU_IPC_RPC_FUNC_CONFIG_GLOBAL_GET_ITEM_MAX,
      sizeof(ecu_index_type_t), p_resp_count, p_result);
}

static error_t ecu_config_global_rpc_get_instance_max_execute(uint32_t *p_resp_count, error_t *p_result, void *usrdata)
{
  ecu_config_global_rpc_ctx_t *ctx = usrdata;

  return ecu_config_global_rpc_execute(ctx, ECU_IPC_RPC_FUNC_CONFIG_GLOBAL_GET_INSTANCE_MAX,
      sizeof(ecu_instance_t), p_resp_count, p_result);
}

static error_t ecu_config_global_rpc_get_header_execute(uint32_t *p_resp_count, error_t *p_result, void *usrdata)
{
  ecu_config_global_rpc_ctx_t *ctx = usrdata;

  return ecu_config_global_rpc_execute(ctx, ECU_IPC_RPC_FUNC_CONFIG_GLOBAL_GET_HEADER,
      sizeof(ecu_config_global_header_t), p_resp_count, p_result);
}

static error_t ecu_config_global_rpc_read_execute(uint32_t *p_resp_count, error_t *p_result, void *usrdata)
{
  ecu_config_global_rpc_ctx_t *ctx = usrdata;

  return ecu_config_global_rpc_execute(ctx, ECU_IPC_RPC_FUNC_CONFIG_GLOBAL_READ,
      ctx->request.bytes, p_resp_count, p_result);
}

static error_t ecu_config_global_rpc_write_execute(uint32_t *p_resp_count, error_t *p_result, void *usrdata)
{
  ecu_config_global_rpc_ctx_t *ctx = usrdata;

  return ecu_config_global_rpc_execute(ctx, ECU_IPC_RPC_FUNC_CONFIG_GLOBAL_WRITE,
      0u, p_resp_count, p_result);
}

static error_t ecu_config_global_rpc_pack(uint32_t arg_index, void *buffer,
    uint32_t capacity, uint32_t *p_pos, void *usrdata)
{
  ecu_config_global_rpc_ctx_t *ctx = usrdata;

  RETURN_IF(arg_index != 0u, E_PARAM);
  return ipc_rpc_pack(buffer, capacity, ctx->response, ctx->response_bytes, p_pos, 1u);
}

error_t ecu_config_global_ipc_init(ecu_config_global_runtime_ctx_t *ctx)
{
  error_t err = E_OK;
  ecu_config_global_rpc_ctx_t *rpc_ctx = &ecu_config_global_rpc_ctx;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    rpc_ctx->global = ctx;

    err = ecu_ipc_rpc_executor_register_func(ECU_IPC_RPC_EXECUTOR_1, ECU_IPC_RPC_FUNC_CONFIG_GLOBAL_GET_TYPE_MAX, 1u,
        ecu_config_global_rpc_unpack, ecu_config_global_rpc_get_type_max_execute, ecu_config_global_rpc_pack, rpc_ctx);
    BREAK_IF(err != E_OK);

    err = ecu_ipc_rpc_executor_register_func(ECU_IPC_RPC_EXECUTOR_1, ECU_IPC_RPC_FUNC_CONFIG_GLOBAL_GET_ITEM_MAX, 1u,
        ecu_config_global_rpc_unpack, ecu_config_global_rpc_get_item_max_execute, ecu_config_global_rpc_pack, rpc_ctx);
    BREAK_IF(err != E_OK);

    err = ecu_ipc_rpc_executor_register_func(ECU_IPC_RPC_EXECUTOR_1, ECU_IPC_RPC_FUNC_CONFIG_GLOBAL_GET_INSTANCE_MAX, 1u,
        ecu_config_global_rpc_unpack, ecu_config_global_rpc_get_instance_max_execute, ecu_config_global_rpc_pack, rpc_ctx);
    BREAK_IF(err != E_OK);

    err = ecu_ipc_rpc_executor_register_func(ECU_IPC_RPC_EXECUTOR_1, ECU_IPC_RPC_FUNC_CONFIG_GLOBAL_GET_HEADER, 1u,
        ecu_config_global_rpc_unpack, ecu_config_global_rpc_get_header_execute, ecu_config_global_rpc_pack, rpc_ctx);
    BREAK_IF(err != E_OK);

    err = ecu_ipc_rpc_executor_register_func(ECU_IPC_RPC_EXECUTOR_1, ECU_IPC_RPC_FUNC_CONFIG_GLOBAL_READ, 1u,
        ecu_config_global_rpc_unpack, ecu_config_global_rpc_read_execute, ecu_config_global_rpc_pack, rpc_ctx);
    BREAK_IF(err != E_OK);

    err = ecu_ipc_rpc_executor_register_func(ECU_IPC_RPC_EXECUTOR_1, ECU_IPC_RPC_FUNC_CONFIG_GLOBAL_WRITE, 2u,
        ecu_config_global_rpc_unpack, ecu_config_global_rpc_write_execute, ecu_config_global_rpc_pack, rpc_ctx);
    BREAK_IF(err != E_OK);

  } while(0);

  return err;
}
#endif /* CORE_CM7 */

#ifdef CORE_CM4
static error_t ecu_config_global_rpc_start(uint32_t *func_id, uint32_t *args_count, void *usrdata)
{
  ecu_config_global_rpc_ctx_t *ctx = usrdata;

  *func_id = ctx->func;
  *args_count = ctx->func == ECU_IPC_RPC_FUNC_CONFIG_GLOBAL_WRITE ? 2u : 1u;

  return E_OK;
}

static error_t ecu_config_global_rpc_pack(uint32_t arg_index, void *buffer,
    uint32_t capacity, uint32_t *p_pos, void *usrdata)
{
  ecu_config_global_rpc_ctx_t *ctx = usrdata;

  switch(arg_index) {
    case 0:
      return ipc_rpc_pack(buffer, capacity, &ctx->request, sizeof(ctx->request), p_pos,
          ALIGNOF(ecu_config_global_rpc_request_t));
    case 1:
      return ipc_rpc_pack(buffer, capacity, ctx->argument, ctx->request.bytes, p_pos, 1u);
    default:
      return E_PARAM;
  }
}

static error_t ecu_config_global_rpc_unpack(uint32_t arg_index, error_t result,
    const void *buffer, uint32_t bytes, uint32_t *p_pos, void *usrdata)
{
  ecu_config_global_rpc_ctx_t *ctx = usrdata;

  RETURN_IF(result != E_OK || ctx->fsm.resp_count != 1u || arg_index != 0u ||
      ctx->func == ECU_IPC_RPC_FUNC_CONFIG_GLOBAL_WRITE || bytes != ctx->response_bytes, E_BADRESP);
  return ipc_rpc_unpack(buffer, bytes, ctx->response, ctx->response_bytes, p_pos, 1u);
}

static void ecu_config_global_rpc_finish(error_t result, void *usrdata)
{
  ecu_config_global_rpc_ctx_t *ctx = usrdata;

  if(result == E_OK && ctx->fsm.arg_index != (ctx->func == ECU_IPC_RPC_FUNC_CONFIG_GLOBAL_WRITE ? 0u : 1u)) {
    ctx->fsm.result = E_BADRESP;
  }
}
#endif /* CORE_CM4 */

static error_t ecu_config_global_rpc_call(ecu_ipc_rpc_func_t func, ecu_config_type_t type,
    ecu_index_type_t index, ecu_instance_t instance, uint32_t offset, uint32_t bytes,
    void *response, uint32_t response_bytes, const void *argument)
{
  ecu_config_global_rpc_request_t request = {
      .type = type,
      .index = index,
      .instance = instance,
      .offset = offset,
      .bytes = bytes,
  };
#ifdef CORE_CM4
  error_t err = E_OK;
  ecu_config_global_rpc_ctx_t *ctx = &ecu_config_global_rpc_ctx;

  do {
    if(ctx->fsm.state == IPC_RPC_CALLER_FSM_IDLE) {
      err = ecu_ipc_get_rpc_caller_ctx(ECU_IPC_RPC_CALLER_1, &ctx->caller);
      BREAK_IF(err != E_OK);
      BREAK_IF_ACTION(ctx->caller == NULL, err = E_NOTRDY);

      ctx->func = func;
      ctx->request = request;
      ctx->response = response;
      ctx->response_bytes = response_bytes;
      ctx->argument = argument;
    } else {
      BREAK_IF_ACTION(ctx->func != func || ctx->request.type != type || ctx->request.index != index ||
          ctx->request.instance != instance || ctx->request.offset != offset || ctx->request.bytes != bytes ||
          ctx->response != response || ctx->response_bytes != response_bytes || ctx->argument != argument, err = E_BUSY);
    }

    err = ipc_rpc_caller_fsm(ctx->caller, &ctx->fsm, ecu_config_global_rpc_start, ecu_config_global_rpc_pack,
        ecu_config_global_rpc_unpack, ecu_config_global_rpc_finish, ctx);

  } while(0);

  return err;
#endif /* CORE_CM4 */
#ifdef CORE_CM7
  (void)response_bytes;
  return ecu_config_global_rpc_access(func, &request, response, argument);
#endif /* CORE_CM7 */
}

error_t ecu_config_global_get_type_max(ecu_config_type_t *type_max)
{
  RETURN_IF(type_max == NULL, E_PARAM);
  return ecu_config_global_rpc_call(ECU_IPC_RPC_FUNC_CONFIG_GLOBAL_GET_TYPE_MAX, 0u, 0u, 0u, 0u, 0u,
      type_max, sizeof(*type_max), NULL);
}

error_t ecu_config_global_get_item_max(ecu_config_type_t type, ecu_index_type_t *item_max)
{
  RETURN_IF(item_max == NULL, E_PARAM);
  return ecu_config_global_rpc_call(ECU_IPC_RPC_FUNC_CONFIG_GLOBAL_GET_ITEM_MAX, type, 0u, 0u, 0u, 0u,
      item_max, sizeof(*item_max), NULL);
}

error_t ecu_config_global_get_instance_max(ecu_config_type_t type, ecu_index_type_t index, ecu_instance_t *instance_max)
{
  RETURN_IF(instance_max == NULL, E_PARAM);
  return ecu_config_global_rpc_call(ECU_IPC_RPC_FUNC_CONFIG_GLOBAL_GET_INSTANCE_MAX, type, index, 0u, 0u, 0u,
      instance_max, sizeof(*instance_max), NULL);
}

error_t ecu_config_global_get_header(ecu_config_type_t type, ecu_index_type_t index,
    ecu_instance_t instance, ecu_config_global_header_t *header)
{
  RETURN_IF(header == NULL, E_PARAM);
  return ecu_config_global_rpc_call(ECU_IPC_RPC_FUNC_CONFIG_GLOBAL_GET_HEADER, type, index, instance, 0u, 0u,
      header, sizeof(*header), NULL);
}

error_t ecu_config_global_read(ecu_config_type_t type, ecu_index_type_t index,
    ecu_instance_t instance, uint32_t offset, void *data, uint32_t bytes)
{
  RETURN_IF(bytes > ECU_CONFIG_GLOBAL_IPC_BYTES_MAX || (bytes != 0u && data == NULL), E_PARAM);
  return ecu_config_global_rpc_call(ECU_IPC_RPC_FUNC_CONFIG_GLOBAL_READ, type, index, instance, offset, bytes,
      data, bytes, NULL);
}

error_t ecu_config_global_write(ecu_config_type_t type, ecu_index_type_t index,
    ecu_instance_t instance, uint32_t offset, const void *data, uint32_t bytes)
{
  RETURN_IF(bytes > ECU_CONFIG_GLOBAL_IPC_BYTES_MAX || (bytes != 0u && data == NULL), E_PARAM);
  return ecu_config_global_rpc_call(ECU_IPC_RPC_FUNC_CONFIG_GLOBAL_WRITE, type, index, instance, offset, bytes,
      NULL, 0u, data);
}
