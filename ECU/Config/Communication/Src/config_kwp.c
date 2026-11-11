/*
 * config_kwp.c
 *
 *  Created on: Oct 19, 2025
 *      Author: VHEMaster
 */

#include "config_kwp.h"
#ifdef CORE_CM7
#include "config_comm_rpc.h"
#endif /* CORE_CM7 */
#ifdef CORE_CM4
#include "config_ipc_rpc_executor.h"
#include "ipc_rpc_pack.h"
#endif /* CORE_CM4 */
#include "config_extern.h"
#include "common.h"

typedef struct ecu_comm_kwp_ctx_tag ecu_comm_kwp_ctx_t;

#ifdef CORE_CM4
static error_t ecu_comm_kwp_rpc_executor_configure_unpack(uint32_t arg_index, const void *buffer, uint32_t capacity, uint32_t *p_pos, void *usrdata);
static error_t ecu_comm_kwp_rpc_executor_configure_execute(void *usrdata);
static error_t ecu_comm_kwp_rpc_executor_reset_unpack(uint32_t arg_index, const void *buffer, uint32_t capacity, uint32_t *p_pos, void *usrdata);
static error_t ecu_comm_kwp_rpc_executor_reset_execute(void *usrdata);
#endif /* CORE_CM4 */

typedef struct ecu_comm_kwp_ctx_tag {
#ifdef CORE_CM7
    ecu_comm_rpc_ctx_t rpc_configure;
    ecu_comm_rpc_ctx_t rpc_reset;
  const kwp_config_t *config_default;
#endif /* CORE_CM7 */
#ifdef CORE_CM4
  kwp_init_ctx_t init;
  kwp_ctx_t *ctx;

    ecu_comm_instance_t rpc_instance;
    const void *rpc_pointer;
#endif /* CORE_CM4 */
}ecu_comm_kwp_ctx_t;

#ifdef CORE_CM7
static const kwp_config_t ecu_comm_kwp_default_config[ECU_COMM_KWP_MAX] = {
  {

  }, //ECU_KWP_IF_1
};

static const bool ecu_comm_kwp_enabled_default[ECU_COMM_KWP_MAX] = {
    true
};
#endif /* CORE_CM7 */

static RAM_SECTION ecu_comm_kwp_ctx_t ecu_comm_kwp_ctx[ECU_COMM_KWP_MAX] = {
    {
#ifdef CORE_CM4
      .init = {

      },
#endif /* CORE_CM4 */
#ifdef CORE_CM7
      .config_default = &ecu_comm_kwp_default_config[ECU_COMM_KWP_1],
#endif /* CORE_CM7 */
    },
};

#ifdef CORE_CM4
error_t ecu_comm_kwp_init(ecu_comm_kwp_t instance, kwp_ctx_t *ctx)
{
  error_t err = E_OK;
  ecu_comm_kwp_ctx_t *kwp_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_COMM_KWP_MAX || ctx == NULL, err = E_PARAM);

    kwp_ctx = &ecu_comm_kwp_ctx[instance];
    kwp_ctx->ctx = ctx;

    err = kwp_init(kwp_ctx->ctx, &kwp_ctx->init);
    BREAK_IF(err != E_OK);

    err = ecu_comm_set_comm_enabled(ECU_COMM_TYPE_KWP, instance, false);
    BREAK_IF(err != E_OK);

    err = ecu_ipc_rpc_executor_register_func(ECU_IPC_RPC_EXECUTOR_1, ECU_IPC_RPC_FUNC_KWP_CONFIGURE, 2u,
        ecu_comm_kwp_rpc_executor_configure_unpack, ecu_comm_kwp_rpc_executor_configure_execute, kwp_ctx);
    BREAK_IF(err != E_OK);

    err = ecu_ipc_rpc_executor_register_func(ECU_IPC_RPC_EXECUTOR_1, ECU_IPC_RPC_FUNC_KWP_RESET, 1u,
        ecu_comm_kwp_rpc_executor_reset_unpack, ecu_comm_kwp_rpc_executor_reset_execute, kwp_ctx);
    BREAK_IF(err != E_OK);

  } while(0);

  return err;
}
#endif /* CORE_CM4 */

#ifdef CORE_CM7
error_t ecu_comm_kwp_get_default_config(ecu_comm_kwp_t instance, kwp_config_t *config)
{
  error_t err = E_OK;
  ecu_comm_kwp_ctx_t *kwp_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_COMM_KWP_MAX || config == NULL, err = E_PARAM);

    kwp_ctx = &ecu_comm_kwp_ctx[instance];

    memcpy(config, kwp_ctx->config_default, sizeof(kwp_config_t));
    config->enabled = ecu_comm_kwp_enabled_default[instance];

  } while(0);

  return err;
}
#endif /* CORE_CM7 */

error_t ecu_comm_kwp_configure(ecu_comm_kwp_t instance, const kwp_config_t *config)
{
  error_t err = E_OK;
  ecu_comm_kwp_ctx_t *kwp_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_COMM_KWP_MAX || config == NULL, err = E_PARAM);

    kwp_ctx = &ecu_comm_kwp_ctx[instance];

#ifdef CORE_CM7
    err = ecu_comm_rpc_call(&kwp_ctx->rpc_configure, ECU_IPC_RPC_FUNC_KWP_CONFIGURE,
        instance, config, sizeof(*config), ALIGNOF(kwp_config_t));
#endif /* CORE_CM7 */

#ifdef CORE_CM4
    err = kwp_configure(kwp_ctx->ctx, config);
    BREAK_IF(err != E_OK);

    err = ecu_comm_set_comm_enabled(ECU_COMM_TYPE_KWP, instance, kwp_ctx->ctx->config.enabled);
    BREAK_IF(err != E_OK);
#endif /* CORE_CM4 */

  } while(0);

  return err;
}

error_t ecu_comm_kwp_reset(ecu_comm_kwp_t instance)
{
  error_t err = E_OK;
  ecu_comm_kwp_ctx_t *kwp_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_COMM_KWP_MAX, err = E_PARAM);

    kwp_ctx = &ecu_comm_kwp_ctx[instance];

#ifdef CORE_CM7
    err = ecu_comm_rpc_call(&kwp_ctx->rpc_reset, ECU_IPC_RPC_FUNC_KWP_RESET,
        instance, NULL, 0u, 1u);
#endif /* CORE_CM7 */

#ifdef CORE_CM4
    err = kwp_reset(kwp_ctx->ctx);
#endif /* CORE_CM4 */

  } while(0);

  return err;
}

#ifdef CORE_CM4
static error_t ecu_comm_kwp_rpc_executor_configure_unpack(uint32_t arg_index, const void *buffer, uint32_t capacity, uint32_t *p_pos, void *usrdata)
{
  error_t err = E_OK;
  ecu_comm_kwp_ctx_t *kwp_ctx = usrdata;

  do {
    BREAK_IF(kwp_ctx == NULL);

    switch(arg_index) {
      case 0:
        err = ipc_rpc_unpack(buffer, capacity,
            &kwp_ctx->rpc_instance, sizeof(ecu_comm_instance_t),
            p_pos, ALIGNOF(ecu_comm_instance_t));
        break;
      case 1:
        err = ipc_rpc_unpack_ptr(buffer, capacity,
            &kwp_ctx->rpc_pointer, sizeof(kwp_config_t),
            p_pos, ALIGNOF(kwp_config_t));
        break;
      default:
        err = E_PARAM;
        break;
    }

  } while(0);

  return err;
}

static error_t ecu_comm_kwp_rpc_executor_configure_execute(void *usrdata)
{
  error_t err = E_OK;
  ecu_comm_kwp_ctx_t *kwp_ctx = usrdata;

  do {
    BREAK_IF(kwp_ctx == NULL);

    err = ecu_comm_kwp_configure(kwp_ctx->rpc_instance, kwp_ctx->rpc_pointer);

  } while(0);

  return err;
}

static error_t ecu_comm_kwp_rpc_executor_reset_unpack(uint32_t arg_index, const void *buffer, uint32_t capacity, uint32_t *p_pos, void *usrdata)
{
  error_t err = E_OK;
  ecu_comm_kwp_ctx_t *kwp_ctx = usrdata;

  do {
    BREAK_IF(kwp_ctx == NULL);

    switch(arg_index) {
      case 0:
        err = ipc_rpc_unpack(buffer, capacity,
            &kwp_ctx->rpc_instance, sizeof(ecu_comm_instance_t),
            p_pos, ALIGNOF(ecu_comm_instance_t));
        break;
      default:
        err = E_PARAM;
        break;
    }

  } while(0);

  return err;
}

static error_t ecu_comm_kwp_rpc_executor_reset_execute(void *usrdata)
{
  error_t err = E_OK;
  ecu_comm_kwp_ctx_t *kwp_ctx = usrdata;

  do {
    BREAK_IF(kwp_ctx == NULL);

    err = ecu_comm_kwp_reset(kwp_ctx->rpc_instance);

  } while(0);

  return err;
}

#endif /* CORE_CM4 */
