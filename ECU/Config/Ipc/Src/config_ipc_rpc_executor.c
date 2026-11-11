/*
 * config_ipc_rpc_executor.c
 *
 *  Created on: 10 нояб. 2026 г.
 *      Author: VHEMaster
 */

#include "config_global.h"
#include "config_ipc_rpc_executor.h"
#include "compiler.h"

#define ECU_IPC_RPC_EXECUTOR_BUFFER_SIZE (8192u)

#ifdef CORE_CM7
#define IPC_RPC_EXECUTOR_HEADER_SECTION SHARED_SECTION_EX("rpc_cm4_to_cm7_headers")
#define IPC_RPC_EXECUTOR_BUFFER_SECTION SHARED_SECTION_EX("rpc_cm4_to_cm7_buffers")
#endif /* CORE_CM7 */

#ifdef CORE_CM4
#define IPC_RPC_EXECUTOR_HEADER_SECTION SHARED_SECTION_EX("rpc_cm7_to_cm4_headers")
#define IPC_RPC_EXECUTOR_BUFFER_SECTION SHARED_SECTION_EX("rpc_cm7_to_cm4_buffers")
#endif /* CORE_CM4 */

static error_t ecu_ipc_rpc_executor_start(uint32_t func_id, uint32_t *args_count, void *usrdata);
static error_t ecu_ipc_rpc_executor_unpack(uint32_t arg_index, const void *buffer,
    uint32_t bytes, uint32_t *pos, void *usrdata);
static error_t ecu_ipc_rpc_executor_execute(void *usrdata);
static void ecu_ipc_rpc_executor_finish(void *usrdata);

static IPC_RPC_EXECUTOR_HEADER_SECTION
ipc_rpc_header_ctx_t ecu_config_rpc_executor_headers[ECU_IPC_RPC_EXECUTOR_MAX];

static IPC_RPC_EXECUTOR_BUFFER_SECTION
uint8_t ecu_config_rpc_executor_buffers[ECU_IPC_RPC_EXECUTOR_MAX][ECU_IPC_RPC_EXECUTOR_BUFFER_SIZE];

typedef struct {
    uint32_t args_count;
    ipc_rpc_executor_fsm_unpack_cb_t unpack;
    ipc_rpc_executor_fsm_execute_cb_t execute;

    void *usrdata;
}ecu_ipc_rpc_executor_func_ctx_t;

typedef struct {
    struct {
        ecu_ipc_rpc_executor_t instance;
        uint32_t buffersize;
        ipc_rpc_header_ctx_t *header;
        void *buffer;
    }init;

    ecu_ipc_rpc_executor_func_ctx_t funcs[ECU_IPC_RPC_FUNC_MAX];

    ipc_rpc_executor_ctx_t *ctx;
    ipc_rpc_executor_fsm_ctx_t fsm;

    const ecu_ipc_rpc_executor_func_ctx_t *func;

}ecu_ipc_rpc_executor_ctx_t;

static RAM_SECTION ecu_ipc_rpc_executor_ctx_t ecu_ipc_rpc_executor_ctx[ECU_IPC_RPC_EXECUTOR_MAX] = {
    {
      .init = {
          .instance = ECU_IPC_RPC_EXECUTOR_1,
          .buffersize = sizeof(ecu_config_rpc_executor_buffers[0]),
          .header = &ecu_config_rpc_executor_headers[ECU_IPC_RPC_EXECUTOR_1],
          .buffer = ecu_config_rpc_executor_buffers[ECU_IPC_RPC_EXECUTOR_1],

      },
    }, //ECU_IPC_RPC_EXECUTOR_1
};

error_t ecu_ipc_rpc_executor_init(ecu_ipc_rpc_executor_t instance, ipc_rpc_executor_ctx_t *ctx)
{
  error_t err = E_OK;
  ecu_ipc_rpc_executor_ctx_t *ipc_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_IPC_RPC_EXECUTOR_MAX || ctx == NULL, err = E_PARAM);

    ipc_ctx = &ecu_ipc_rpc_executor_ctx[instance];
    ipc_ctx->ctx = NULL;

    memset(&ipc_ctx->fsm, 0u, sizeof(ipc_rpc_executor_fsm_ctx_t));
    ipc_ctx->func = NULL;

    err = ipc_rpc_executor_init(ctx, ipc_ctx->init.header,
        ipc_ctx->init.buffersize, ipc_ctx->init.buffer);
    BREAK_IF(err != E_OK);

    ipc_ctx->ctx = ctx;

  } while(0);

  return err;
}

error_t ecu_ipc_rpc_executor_register_func(ecu_ipc_rpc_executor_t instance, uint32_t func_id, uint32_t args_count,
    ipc_rpc_executor_fsm_unpack_cb_t unpack, ipc_rpc_executor_fsm_execute_cb_t execute, void *usrdata)
{
  error_t err = E_OK;
  ecu_ipc_rpc_executor_ctx_t *ipc_ctx;
  ecu_ipc_rpc_executor_func_ctx_t *func;

  do {
    BREAK_IF_ACTION(instance >= ECU_IPC_RPC_EXECUTOR_MAX, err = E_PARAM);
    BREAK_IF_ACTION(func_id >= ECU_IPC_RPC_FUNC_MAX, err = E_PARAM);
    BREAK_IF_ACTION(unpack == NULL, err = E_PARAM);
    BREAK_IF_ACTION(execute == NULL, err = E_PARAM);

    ipc_ctx = &ecu_ipc_rpc_executor_ctx[instance];
    func = &ipc_ctx->funcs[func_id];
    func->args_count = args_count;
    func->unpack = unpack;
    func->execute = execute;
    func->usrdata = usrdata;

  } while(0);

  return err;
}

void ecu_ipc_rpc_executor_loop(ecu_ipc_rpc_executor_t instance)
{
  ecu_ipc_rpc_executor_ctx_t *ipc_ctx;

  do {
    BREAK_IF(instance >= ECU_IPC_RPC_EXECUTOR_MAX);

    ipc_ctx = &ecu_ipc_rpc_executor_ctx[instance];

    if(ipc_ctx->ctx != NULL) {
      (void)ipc_rpc_executor_fsm(ipc_ctx->ctx, &ipc_ctx->fsm,
          ecu_ipc_rpc_executor_start, ecu_ipc_rpc_executor_unpack,
          ecu_ipc_rpc_executor_execute, ecu_ipc_rpc_executor_finish, ipc_ctx);
    }

  } while(0);
}

static error_t ecu_ipc_rpc_executor_start(uint32_t func_id, uint32_t *args_count, void *usrdata)
{
  ecu_ipc_rpc_executor_ctx_t *ipc_ctx = usrdata;
  const ecu_ipc_rpc_executor_func_ctx_t *func;

  RETURN_IF(func_id >= ECU_IPC_RPC_FUNC_MAX, E_PARAM);

  func = &ipc_ctx->funcs[func_id];
  RETURN_IF(func->execute == NULL, E_NOTSUPPORT);

  ipc_ctx->func = func;
  *args_count = func->args_count;

  return E_OK;
}

static error_t ecu_ipc_rpc_executor_unpack(uint32_t arg_index, const void *buffer,
    uint32_t bytes, uint32_t *pos, void *usrdata)
{
  ecu_ipc_rpc_executor_ctx_t *ipc_ctx = usrdata;
  const ecu_ipc_rpc_executor_func_ctx_t *func = ipc_ctx->func;

  RETURN_IF(func->unpack == NULL, E_INVALACT);
  return func->unpack(arg_index, buffer, bytes, pos, func->usrdata);
}

static error_t ecu_ipc_rpc_executor_execute(void *usrdata)
{
  ecu_ipc_rpc_executor_ctx_t *ipc_ctx = usrdata;
  const ecu_ipc_rpc_executor_func_ctx_t *func = ipc_ctx->func;

  RETURN_IF(func->execute == NULL, E_INVALACT);
  return func->execute(func->usrdata);
}

static void ecu_ipc_rpc_executor_finish(void *usrdata)
{
  ecu_ipc_rpc_executor_ctx_t *ipc_ctx = usrdata;

  ipc_ctx->func = NULL;
}
