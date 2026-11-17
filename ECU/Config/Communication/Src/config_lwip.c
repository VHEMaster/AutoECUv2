/*
 * config_lwip.c
 *
 *  Created on: 17 нояб. 2026 г.
 *      Author: VHEMaster
 */

#include "config_lwip.h"
#ifdef CORE_CM7
#include "config_comm_rpc.h"
#endif /* CORE_CM7 */
#ifdef CORE_CM4
#include "config_ipc_rpc_executor.h"
#include "ipc_rpc_pack.h"
#endif /* CORE_CM4 */
#include "config_extern.h"
#include "common.h"

typedef struct ecu_comm_lwip_ctx_tag ecu_comm_lwip_ctx_t;

#ifdef CORE_CM4
static error_t ecu_comm_lwip_rpc_executor_configure_unpack(uint32_t arg_index, const void *buffer, uint32_t capacity, uint32_t *p_pos, void *usrdata);
static error_t ecu_comm_lwip_rpc_executor_configure_execute(uint32_t *p_resp_count, error_t *p_result, void *usrdata);
static error_t ecu_comm_lwip_rpc_executor_reset_unpack(uint32_t arg_index, const void *buffer, uint32_t capacity, uint32_t *p_pos, void *usrdata);
static error_t ecu_comm_lwip_rpc_executor_reset_execute(uint32_t *p_resp_count, error_t *p_result, void *usrdata);
#endif /* CORE_CM4 */

typedef struct ecu_comm_lwip_ctx_tag {
#ifdef CORE_CM7
    ecu_comm_rpc_ctx_t rpc_configure;
    ecu_comm_rpc_ctx_t rpc_reset;
  const lwip_config_t *config_default;
#endif /* CORE_CM7 */
#ifdef CORE_CM4
  lwip_init_ctx_t init;
  lwip_ctx_t *ctx;

    ecu_comm_instance_t rpc_instance;
    const void *rpc_pointer;
#endif /* CORE_CM4 */
}ecu_comm_lwip_ctx_t;

#ifdef CORE_CM7
static const lwip_config_t ecu_comm_lwip_default_config[ECU_COMM_LWIP_MAX] = {
  {

  }, //ECU_LWIP_IF_1
};

static const bool ecu_comm_lwip_enabled_default[ECU_COMM_LWIP_MAX] = {
    true
};
#endif /* CORE_CM7 */

static RAM_SECTION ecu_comm_lwip_ctx_t ecu_comm_lwip_ctx[ECU_COMM_LWIP_MAX] = {
    {
#ifdef CORE_CM4
      .init = {

      },
#endif /* CORE_CM4 */
#ifdef CORE_CM7
      .config_default = &ecu_comm_lwip_default_config[ECU_COMM_LWIP_1],
#endif /* CORE_CM7 */
    },
};

#ifdef CORE_CM4
error_t ecu_comm_lwip_init(ecu_comm_lwip_t instance, lwip_ctx_t *ctx)
{
  error_t err = E_OK;
  ecu_comm_lwip_ctx_t *lwip_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_COMM_LWIP_MAX || ctx == NULL, err = E_PARAM);

    lwip_ctx = &ecu_comm_lwip_ctx[instance];
    lwip_ctx->ctx = ctx;

    err = lwip_init(lwip_ctx->ctx, &lwip_ctx->init);
    BREAK_IF(err != E_OK);

    err = ecu_comm_set_comm_enabled(ECU_COMM_TYPE_LWIP, instance, false);
    BREAK_IF(err != E_OK);

    err = ecu_ipc_rpc_executor_register_func(ECU_IPC_RPC_EXECUTOR_1, ECU_IPC_RPC_FUNC_LWIP_CONFIGURE, 2u,
        ecu_comm_lwip_rpc_executor_configure_unpack, ecu_comm_lwip_rpc_executor_configure_execute, NULL, lwip_ctx);
    BREAK_IF(err != E_OK);

    err = ecu_ipc_rpc_executor_register_func(ECU_IPC_RPC_EXECUTOR_1, ECU_IPC_RPC_FUNC_LWIP_RESET, 1u,
        ecu_comm_lwip_rpc_executor_reset_unpack, ecu_comm_lwip_rpc_executor_reset_execute, NULL, lwip_ctx);
    BREAK_IF(err != E_OK);

  } while(0);

  return err;
}
#endif /* CORE_CM4 */

#ifdef CORE_CM7
error_t ecu_comm_lwip_get_default_config(ecu_comm_lwip_t instance, lwip_config_t *config)
{
  error_t err = E_OK;
  ecu_comm_lwip_ctx_t *lwip_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_COMM_LWIP_MAX || config == NULL, err = E_PARAM);

    lwip_ctx = &ecu_comm_lwip_ctx[instance];

    memcpy(config, lwip_ctx->config_default, sizeof(lwip_config_t));
    config->enabled = ecu_comm_lwip_enabled_default[instance];

  } while(0);

  return err;
}
#endif /* CORE_CM7 */

error_t ecu_comm_lwip_configure(ecu_comm_lwip_t instance, const lwip_config_t *config)
{
  error_t err = E_OK;
  ecu_comm_lwip_ctx_t *lwip_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_COMM_LWIP_MAX || config == NULL, err = E_PARAM);

    lwip_ctx = &ecu_comm_lwip_ctx[instance];

#ifdef CORE_CM7
    err = ecu_comm_rpc_call(&lwip_ctx->rpc_configure, ECU_IPC_RPC_FUNC_LWIP_CONFIGURE,
        instance, config, sizeof(*config), ALIGNOF(lwip_config_t));
#endif /* CORE_CM7 */

#ifdef CORE_CM4
    err = lwip_configure(lwip_ctx->ctx, config);
    BREAK_IF(err != E_OK);

    err = ecu_comm_set_comm_enabled(ECU_COMM_TYPE_LWIP, instance, lwip_ctx->ctx->config.enabled);
    BREAK_IF(err != E_OK);
#endif /* CORE_CM4 */

  } while(0);

  return err;
}

error_t ecu_comm_lwip_reset(ecu_comm_lwip_t instance)
{
  error_t err = E_OK;
  ecu_comm_lwip_ctx_t *lwip_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_COMM_LWIP_MAX, err = E_PARAM);

    lwip_ctx = &ecu_comm_lwip_ctx[instance];

#ifdef CORE_CM7
    err = ecu_comm_rpc_call(&lwip_ctx->rpc_reset, ECU_IPC_RPC_FUNC_LWIP_RESET,
        instance, NULL, 0u, 1u);
#endif /* CORE_CM7 */

#ifdef CORE_CM4
    err = lwip_reset(lwip_ctx->ctx);
#endif /* CORE_CM4 */

  } while(0);

  return err;
}

#ifdef CORE_CM4
static error_t ecu_comm_lwip_rpc_executor_configure_unpack(uint32_t arg_index, const void *buffer, uint32_t capacity, uint32_t *p_pos, void *usrdata)
{
  error_t err = E_OK;
  ecu_comm_lwip_ctx_t *lwip_ctx = usrdata;

  do {
    BREAK_IF_ACTION(lwip_ctx == NULL, err = E_PARAM);

    switch(arg_index) {
      case 0:
        err = ipc_rpc_unpack(buffer, capacity,
            &lwip_ctx->rpc_instance, sizeof(ecu_comm_instance_t),
            p_pos, ALIGNOF(ecu_comm_instance_t));
        break;
      case 1:
        err = ipc_rpc_unpack_ptr(buffer, capacity,
            &lwip_ctx->rpc_pointer, sizeof(lwip_config_t),
            p_pos, ALIGNOF(lwip_config_t));
        break;
      default:
        err = E_PARAM;
        break;
    }

  } while(0);

  return err;
}

static error_t ecu_comm_lwip_rpc_executor_configure_execute(uint32_t *p_resp_count, error_t *p_result, void *usrdata)
{
  error_t err = E_OK;
  ecu_comm_lwip_ctx_t *lwip_ctx = usrdata;

  do {
    BREAK_IF_ACTION(lwip_ctx == NULL, err = E_PARAM);

    err = ecu_comm_lwip_configure(lwip_ctx->rpc_instance, lwip_ctx->rpc_pointer);
    if(err != E_AGAIN) {
      *p_result = err;
      err = E_OK;
    }

  } while(0);

  return err;
}

static error_t ecu_comm_lwip_rpc_executor_reset_unpack(uint32_t arg_index, const void *buffer, uint32_t capacity, uint32_t *p_pos, void *usrdata)
{
  error_t err = E_OK;
  ecu_comm_lwip_ctx_t *lwip_ctx = usrdata;

  do {
    BREAK_IF_ACTION(lwip_ctx == NULL, err = E_PARAM);

    switch(arg_index) {
      case 0:
        err = ipc_rpc_unpack(buffer, capacity,
            &lwip_ctx->rpc_instance, sizeof(ecu_comm_instance_t),
            p_pos, ALIGNOF(ecu_comm_instance_t));
        break;
      default:
        err = E_PARAM;
        break;
    }

  } while(0);

  return err;
}

static error_t ecu_comm_lwip_rpc_executor_reset_execute(uint32_t *p_resp_count, error_t *p_result, void *usrdata)
{
  error_t err = E_OK;
  ecu_comm_lwip_ctx_t *lwip_ctx = usrdata;

  do {
    BREAK_IF_ACTION(lwip_ctx == NULL, err = E_PARAM);

    err = ecu_comm_lwip_reset(lwip_ctx->rpc_instance);
    if(err != E_AGAIN) {
      *p_result = err;
      err = E_OK;
    }

  } while(0);

  return err;
}

#endif /* CORE_CM4 */
