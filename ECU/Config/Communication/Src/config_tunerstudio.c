/*
 * config_tunerstudio.c
 *
 *  Created on: Nov 26, 2026
 *      Author: VHEMaster
 */

#include "config_tunerstudio.h"
#ifdef CORE_CM7
#include "config_comm_rpc.h"
#endif /* CORE_CM7 */
#ifdef CORE_CM4
#include "config_ipc_rpc_executor.h"
#include "ipc_rpc_pack.h"
#endif /* CORE_CM4 */
#include "config_extern.h"
#include "common.h"

typedef struct ecu_comm_tunerstudio_ctx_tag ecu_comm_tunerstudio_ctx_t;

#ifdef CORE_CM4
static error_t ecu_comm_tunerstudio_rpc_executor_configure_unpack(uint32_t arg_index, const void *buffer, uint32_t capacity, uint32_t *p_pos, void *usrdata);
static error_t ecu_comm_tunerstudio_rpc_executor_configure_execute(uint32_t *p_resp_count, error_t *p_result, void *usrdata);
static error_t ecu_comm_tunerstudio_rpc_executor_reset_unpack(uint32_t arg_index, const void *buffer, uint32_t capacity, uint32_t *p_pos, void *usrdata);
static error_t ecu_comm_tunerstudio_rpc_executor_reset_execute(uint32_t *p_resp_count, error_t *p_result, void *usrdata);
#endif /* CORE_CM4 */

typedef struct ecu_comm_tunerstudio_ctx_tag {
#ifdef CORE_CM7
    ecu_comm_rpc_ctx_t rpc_configure;
    ecu_comm_rpc_ctx_t rpc_reset;
    const tunerstudio_config_t *config_default;
#endif /* CORE_CM7 */
#ifdef CORE_CM4
    tunerstudio_init_ctx_t init;
    tunerstudio_ctx_t *ctx;

    ecu_comm_instance_t rpc_instance;
    const void *rpc_pointer;
#endif /* CORE_CM4 */
}ecu_comm_tunerstudio_ctx_t;

#ifdef CORE_CM7
static const tunerstudio_config_t ecu_comm_tunerstudio_default_config[ECU_COMM_TUNERSTUDIO_MAX] = {
  {
      .lwip_enabled = true,
      .rs422_enabled = false,
      .port = 2000,
      .rs422_instance = ECU_COMM_RS422_1,
  }, //ECU_COMM_TUNERSTUDIO_1
};

static const bool ecu_comm_tunerstudio_enabled_default[ECU_COMM_TUNERSTUDIO_MAX] = {
    true
};
#endif /* CORE_CM7 */

static RAM_SECTION ecu_comm_tunerstudio_ctx_t ecu_comm_tunerstudio_ctx[ECU_COMM_TUNERSTUDIO_MAX] = {
    {
#ifdef CORE_CM4
      .init = {
          .error_callback = NULL,
      },
#endif /* CORE_CM4 */
#ifdef CORE_CM7
      .config_default = &ecu_comm_tunerstudio_default_config[ECU_COMM_TUNERSTUDIO_1],
#endif /* CORE_CM7 */
    },
};

#ifdef CORE_CM4
error_t ecu_comm_tunerstudio_init(ecu_comm_tunerstudio_t instance, tunerstudio_ctx_t *ctx)
{
  error_t err = E_OK;
  ecu_comm_tunerstudio_ctx_t *tunerstudio_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_COMM_TUNERSTUDIO_MAX || ctx == NULL, err = E_PARAM);

    tunerstudio_ctx = &ecu_comm_tunerstudio_ctx[instance];
    tunerstudio_ctx->ctx = ctx;

    for(int i = 0; i < ECU_COMM_RS422_MAX; i++) {
      err = ecu_comm_get_rs422_ctx(i, &tunerstudio_ctx->init.rs422_ctx[i]);
      BREAK_IF(err != E_OK);
    }
    BREAK_IF(err != E_OK);

    err = tunerstudio_init(tunerstudio_ctx->ctx, &tunerstudio_ctx->init);
    BREAK_IF(err != E_OK);

    err = ecu_comm_set_comm_enabled(ECU_COMM_TYPE_TUNERSTUDIO, instance, false);
    BREAK_IF(err != E_OK);

    err = ecu_ipc_rpc_executor_register_func(ECU_IPC_RPC_EXECUTOR_1, ECU_IPC_RPC_FUNC_TUNERSTUDIO_CONFIGURE, 2u,
        ecu_comm_tunerstudio_rpc_executor_configure_unpack, ecu_comm_tunerstudio_rpc_executor_configure_execute, NULL, tunerstudio_ctx);
    BREAK_IF(err != E_OK);

    err = ecu_ipc_rpc_executor_register_func(ECU_IPC_RPC_EXECUTOR_1, ECU_IPC_RPC_FUNC_TUNERSTUDIO_RESET, 1u,
        ecu_comm_tunerstudio_rpc_executor_reset_unpack, ecu_comm_tunerstudio_rpc_executor_reset_execute, NULL, tunerstudio_ctx);
    BREAK_IF(err != E_OK);

  } while(0);

  return err;
}
#endif /* CORE_CM4 */

#ifdef CORE_CM7
error_t ecu_comm_tunerstudio_get_default_config(ecu_comm_tunerstudio_t instance, tunerstudio_config_t *config)
{
  error_t err = E_OK;
  ecu_comm_tunerstudio_ctx_t *tunerstudio_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_COMM_TUNERSTUDIO_MAX || config == NULL, err = E_PARAM);

    tunerstudio_ctx = &ecu_comm_tunerstudio_ctx[instance];

    memcpy(config, tunerstudio_ctx->config_default, sizeof(tunerstudio_config_t));
    config->enabled = ecu_comm_tunerstudio_enabled_default[instance];

  } while(0);

  return err;
}
#endif /* CORE_CM7 */

error_t ecu_comm_tunerstudio_configure(ecu_comm_tunerstudio_t instance, const tunerstudio_config_t *config)
{
  error_t err = E_OK;
  ecu_comm_tunerstudio_ctx_t *tunerstudio_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_COMM_TUNERSTUDIO_MAX || config == NULL, err = E_PARAM);

    tunerstudio_ctx = &ecu_comm_tunerstudio_ctx[instance];

#ifdef CORE_CM7
    err = ecu_comm_rpc_call(&tunerstudio_ctx->rpc_configure, ECU_IPC_RPC_FUNC_TUNERSTUDIO_CONFIGURE,
        instance, config, sizeof(*config), ALIGNOF(tunerstudio_config_t));
#endif /* CORE_CM7 */

#ifdef CORE_CM4
    err = tunerstudio_configure(tunerstudio_ctx->ctx, config);
    BREAK_IF(err != E_OK);

    err = ecu_comm_set_comm_enabled(ECU_COMM_TYPE_TUNERSTUDIO, instance, tunerstudio_ctx->ctx->config.enabled);
    BREAK_IF(err != E_OK);
#endif /* CORE_CM4 */

  } while(0);

  return err;
}

error_t ecu_comm_tunerstudio_reset(ecu_comm_tunerstudio_t instance)
{
  error_t err = E_OK;
  ecu_comm_tunerstudio_ctx_t *tunerstudio_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_COMM_TUNERSTUDIO_MAX, err = E_PARAM);

    tunerstudio_ctx = &ecu_comm_tunerstudio_ctx[instance];

#ifdef CORE_CM7
    err = ecu_comm_rpc_call(&tunerstudio_ctx->rpc_reset, ECU_IPC_RPC_FUNC_TUNERSTUDIO_RESET,
        instance, NULL, 0u, 1u);
#endif /* CORE_CM7 */

#ifdef CORE_CM4
    err = tunerstudio_reset(tunerstudio_ctx->ctx);
#endif /* CORE_CM4 */

  } while(0);

  return err;
}

#ifdef CORE_CM4
static error_t ecu_comm_tunerstudio_rpc_executor_configure_unpack(uint32_t arg_index, const void *buffer, uint32_t capacity, uint32_t *p_pos, void *usrdata)
{
  error_t err = E_OK;
  ecu_comm_tunerstudio_ctx_t *tunerstudio_ctx = usrdata;

  do {
    BREAK_IF_ACTION(tunerstudio_ctx == NULL, err = E_PARAM);

    switch(arg_index) {
      case 0:
        err = ipc_rpc_unpack(buffer, capacity,
            &tunerstudio_ctx->rpc_instance, sizeof(ecu_comm_instance_t),
            p_pos, ALIGNOF(ecu_comm_instance_t));
        break;
      case 1:
        err = ipc_rpc_unpack_ptr(buffer, capacity,
            &tunerstudio_ctx->rpc_pointer, sizeof(tunerstudio_config_t),
            p_pos, ALIGNOF(tunerstudio_config_t));
        break;
      default:
        err = E_PARAM;
        break;
    }

  } while(0);

  return err;
}

static error_t ecu_comm_tunerstudio_rpc_executor_configure_execute(uint32_t *p_resp_count, error_t *p_result, void *usrdata)
{
  error_t err = E_OK;
  ecu_comm_tunerstudio_ctx_t *tunerstudio_ctx = usrdata;

  do {
    BREAK_IF_ACTION(tunerstudio_ctx == NULL, err = E_PARAM);

    err = ecu_comm_tunerstudio_configure(tunerstudio_ctx->rpc_instance, tunerstudio_ctx->rpc_pointer);
    if(err != E_AGAIN) {
      *p_result = err;
      err = E_OK;
    }

  } while(0);

  return err;
}

static error_t ecu_comm_tunerstudio_rpc_executor_reset_unpack(uint32_t arg_index, const void *buffer, uint32_t capacity, uint32_t *p_pos, void *usrdata)
{
  error_t err = E_OK;
  ecu_comm_tunerstudio_ctx_t *tunerstudio_ctx = usrdata;

  do {
    BREAK_IF_ACTION(tunerstudio_ctx == NULL, err = E_PARAM);

    switch(arg_index) {
      case 0:
        err = ipc_rpc_unpack(buffer, capacity,
            &tunerstudio_ctx->rpc_instance, sizeof(ecu_comm_instance_t),
            p_pos, ALIGNOF(ecu_comm_instance_t));
        break;
      default:
        err = E_PARAM;
        break;
    }

  } while(0);

  return err;
}

static error_t ecu_comm_tunerstudio_rpc_executor_reset_execute(uint32_t *p_resp_count, error_t *p_result, void *usrdata)
{
  error_t err = E_OK;
  ecu_comm_tunerstudio_ctx_t *tunerstudio_ctx = usrdata;

  do {
    BREAK_IF_ACTION(tunerstudio_ctx == NULL, err = E_PARAM);

    err = ecu_comm_tunerstudio_reset(tunerstudio_ctx->rpc_instance);
    if(err != E_AGAIN) {
      *p_result = err;
      err = E_OK;
    }

  } while(0);

  return err;
}

#endif /* CORE_CM4 */
