/*
 * config_lwipdrv.c
 *
 *  Created on: 17 нояб. 2026 г.
 *      Author: VHEMaster
 */

#include "config_lwipdrv.h"
#ifdef CORE_CM7
#include "config_comm_rpc.h"
#endif /* CORE_CM7 */
#ifdef CORE_CM4
#include "config_ipc_rpc_executor.h"
#include "ipc_rpc_pack.h"
#endif /* CORE_CM4 */
#include "config_extern.h"
#include "common.h"

typedef struct ecu_comm_lwipdrv_ctx_tag ecu_comm_lwipdrv_ctx_t;

#ifdef CORE_CM4
static error_t ecu_comm_lwipdrv_rpc_executor_configure_unpack(uint32_t arg_index, const void *buffer, uint32_t capacity, uint32_t *p_pos, void *usrdata);
static error_t ecu_comm_lwipdrv_rpc_executor_configure_execute(uint32_t *p_resp_count, error_t *p_result, void *usrdata);
static error_t ecu_comm_lwipdrv_rpc_executor_reset_unpack(uint32_t arg_index, const void *buffer, uint32_t capacity, uint32_t *p_pos, void *usrdata);
static error_t ecu_comm_lwipdrv_rpc_executor_reset_execute(uint32_t *p_resp_count, error_t *p_result, void *usrdata);
#endif /* CORE_CM4 */

typedef struct ecu_comm_lwipdrv_ctx_tag {
#ifdef CORE_CM7
    ecu_comm_rpc_ctx_t rpc_configure;
    ecu_comm_rpc_ctx_t rpc_reset;
  const lwipdrv_config_t *config_default;
#endif /* CORE_CM7 */
#ifdef CORE_CM4
    lwipdrv_init_ctx_t init;
    lwipdrv_ctx_t *ctx;

    ecu_comm_instance_t rpc_instance;
    const void *rpc_pointer;
#endif /* CORE_CM4 */
}ecu_comm_lwipdrv_ctx_t;

#ifdef CORE_CM7
static const lwipdrv_config_t ecu_comm_lwipdrv_default_config[ECU_COMM_LWIPDRV_MAX] = {
  {
      .ip_addr_our = { 10, 77, 0, 2 },
      .ip_addr_his = { 10, 77, 0, 1 },
      .lcp_echo_enabled = true,
      .lcp_echo_interval = 1u,
      .lcp_echo_fails = 3u,
  }, //ECU_LWIPDRV_IF_1
};

static const bool ecu_comm_lwipdrv_enabled_default[ECU_COMM_LWIPDRV_MAX] = {
    true
};
#endif /* CORE_CM7 */

static RAM_SECTION ecu_comm_lwipdrv_ctx_t ecu_comm_lwipdrv_ctx[ECU_COMM_LWIPDRV_MAX] = {
    {
#ifdef CORE_CM4
      .init = {
          .rs422_instance = ECU_COMM_RS422_1,
      },
#endif /* CORE_CM4 */
#ifdef CORE_CM7
      .config_default = &ecu_comm_lwipdrv_default_config[ECU_COMM_LWIPDRV_1],
#endif /* CORE_CM7 */
    },
};

#ifdef CORE_CM4
error_t ecu_comm_lwipdrv_init(ecu_comm_lwipdrv_t instance, lwipdrv_ctx_t *ctx)
{
  error_t err = E_OK;
  ecu_comm_lwipdrv_ctx_t *lwipdrv_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_COMM_LWIPDRV_MAX || ctx == NULL, err = E_PARAM);

    lwipdrv_ctx = &ecu_comm_lwipdrv_ctx[instance];
    lwipdrv_ctx->ctx = ctx;

    err = ecu_comm_get_rs422_ctx(lwipdrv_ctx->init.rs422_instance, &lwipdrv_ctx->init.rs422_ctx);
    BREAK_IF(err != E_OK);

    err = ecu_comm_get_usb_ctx(ECU_COMM_USB_1, &lwipdrv_ctx->init.usb_ctx);
    BREAK_IF(err != E_OK);

    err = ecu_comm_get_doip_ctx(ECU_COMM_DOIP_1, &lwipdrv_ctx->init.doip_ctx);
    BREAK_IF(err != E_OK);

    err = lwipdrv_init(lwipdrv_ctx->ctx, &lwipdrv_ctx->init);
    BREAK_IF(err != E_OK);

    err = ecu_comm_set_comm_enabled(ECU_COMM_TYPE_LWIPDRV, instance, false);
    BREAK_IF(err != E_OK);

    err = ecu_ipc_rpc_executor_register_func(ECU_IPC_RPC_EXECUTOR_1, ECU_IPC_RPC_FUNC_LWIPDRV_CONFIGURE, 2u,
        ecu_comm_lwipdrv_rpc_executor_configure_unpack, ecu_comm_lwipdrv_rpc_executor_configure_execute, NULL, lwipdrv_ctx);
    BREAK_IF(err != E_OK);

    err = ecu_ipc_rpc_executor_register_func(ECU_IPC_RPC_EXECUTOR_1, ECU_IPC_RPC_FUNC_LWIPDRV_RESET, 1u,
        ecu_comm_lwipdrv_rpc_executor_reset_unpack, ecu_comm_lwipdrv_rpc_executor_reset_execute, NULL, lwipdrv_ctx);
    BREAK_IF(err != E_OK);

  } while(0);

  return err;
}
#endif /* CORE_CM4 */

#ifdef CORE_CM7
error_t ecu_comm_lwipdrv_get_default_config(ecu_comm_lwipdrv_t instance, lwipdrv_config_t *config)
{
  error_t err = E_OK;
  ecu_comm_lwipdrv_ctx_t *lwipdrv_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_COMM_LWIPDRV_MAX || config == NULL, err = E_PARAM);

    lwipdrv_ctx = &ecu_comm_lwipdrv_ctx[instance];

    memcpy(config, lwipdrv_ctx->config_default, sizeof(lwipdrv_config_t));
    config->enabled = ecu_comm_lwipdrv_enabled_default[instance];

  } while(0);

  return err;
}
#endif /* CORE_CM7 */

error_t ecu_comm_lwipdrv_configure(ecu_comm_lwipdrv_t instance, const lwipdrv_config_t *config)
{
  error_t err = E_OK;
  ecu_comm_lwipdrv_ctx_t *lwipdrv_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_COMM_LWIPDRV_MAX || config == NULL, err = E_PARAM);

    lwipdrv_ctx = &ecu_comm_lwipdrv_ctx[instance];

#ifdef CORE_CM7
    err = ecu_comm_rpc_call(&lwipdrv_ctx->rpc_configure, ECU_IPC_RPC_FUNC_LWIPDRV_CONFIGURE,
        instance, config, sizeof(*config), ALIGNOF(lwipdrv_config_t));
#endif /* CORE_CM7 */

#ifdef CORE_CM4
    err = lwipdrv_configure(lwipdrv_ctx->ctx, config);
    BREAK_IF(err != E_OK);

    err = ecu_comm_set_comm_enabled(ECU_COMM_TYPE_LWIPDRV, instance, lwipdrv_ctx->ctx->config.enabled);
    BREAK_IF(err != E_OK);
#endif /* CORE_CM4 */

  } while(0);

  return err;
}

error_t ecu_comm_lwipdrv_reset(ecu_comm_lwipdrv_t instance)
{
  error_t err = E_OK;
  ecu_comm_lwipdrv_ctx_t *lwipdrv_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_COMM_LWIPDRV_MAX, err = E_PARAM);

    lwipdrv_ctx = &ecu_comm_lwipdrv_ctx[instance];

#ifdef CORE_CM7
    err = ecu_comm_rpc_call(&lwipdrv_ctx->rpc_reset, ECU_IPC_RPC_FUNC_LWIPDRV_RESET,
        instance, NULL, 0u, 1u);
#endif /* CORE_CM7 */

#ifdef CORE_CM4
    err = lwipdrv_reset(lwipdrv_ctx->ctx);
#endif /* CORE_CM4 */

  } while(0);

  return err;
}

#ifdef CORE_CM4
static error_t ecu_comm_lwipdrv_rpc_executor_configure_unpack(uint32_t arg_index, const void *buffer, uint32_t capacity, uint32_t *p_pos, void *usrdata)
{
  error_t err = E_OK;
  ecu_comm_lwipdrv_ctx_t *lwipdrv_ctx = usrdata;

  do {
    BREAK_IF_ACTION(lwipdrv_ctx == NULL, err = E_PARAM);

    switch(arg_index) {
      case 0:
        err = ipc_rpc_unpack(buffer, capacity,
            &lwipdrv_ctx->rpc_instance, sizeof(ecu_comm_instance_t),
            p_pos, ALIGNOF(ecu_comm_instance_t));
        break;
      case 1:
        err = ipc_rpc_unpack_ptr(buffer, capacity,
            &lwipdrv_ctx->rpc_pointer, sizeof(lwipdrv_config_t),
            p_pos, ALIGNOF(lwipdrv_config_t));
        break;
      default:
        err = E_PARAM;
        break;
    }

  } while(0);

  return err;
}

static error_t ecu_comm_lwipdrv_rpc_executor_configure_execute(uint32_t *p_resp_count, error_t *p_result, void *usrdata)
{
  error_t err = E_OK;
  ecu_comm_lwipdrv_ctx_t *lwipdrv_ctx = usrdata;

  do {
    BREAK_IF_ACTION(lwipdrv_ctx == NULL, err = E_PARAM);

    err = ecu_comm_lwipdrv_configure(lwipdrv_ctx->rpc_instance, lwipdrv_ctx->rpc_pointer);
    if(err != E_AGAIN) {
      *p_result = err;
      err = E_OK;
    }

  } while(0);

  return err;
}

static error_t ecu_comm_lwipdrv_rpc_executor_reset_unpack(uint32_t arg_index, const void *buffer, uint32_t capacity, uint32_t *p_pos, void *usrdata)
{
  error_t err = E_OK;
  ecu_comm_lwipdrv_ctx_t *lwipdrv_ctx = usrdata;

  do {
    BREAK_IF_ACTION(lwipdrv_ctx == NULL, err = E_PARAM);

    switch(arg_index) {
      case 0:
        err = ipc_rpc_unpack(buffer, capacity,
            &lwipdrv_ctx->rpc_instance, sizeof(ecu_comm_instance_t),
            p_pos, ALIGNOF(ecu_comm_instance_t));
        break;
      default:
        err = E_PARAM;
        break;
    }

  } while(0);

  return err;
}

static error_t ecu_comm_lwipdrv_rpc_executor_reset_execute(uint32_t *p_resp_count, error_t *p_result, void *usrdata)
{
  error_t err = E_OK;
  ecu_comm_lwipdrv_ctx_t *lwipdrv_ctx = usrdata;

  do {
    BREAK_IF_ACTION(lwipdrv_ctx == NULL, err = E_PARAM);

    err = ecu_comm_lwipdrv_reset(lwipdrv_ctx->rpc_instance);
    if(err != E_AGAIN) {
      *p_result = err;
      err = E_OK;
    }

  } while(0);

  return err;
}

#endif /* CORE_CM4 */
