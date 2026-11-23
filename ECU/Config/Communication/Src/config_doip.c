/*
 * config_doip.c
 *
 *  Created on: Nov 18, 2026
 *      Author: VHEMaster
 */

#include "config_doip.h"
#ifdef CORE_CM7
#include "config_comm_rpc.h"
#endif /* CORE_CM7 */
#ifdef CORE_CM4
#include "config_ipc_rpc_executor.h"
#include "ipc_rpc_pack.h"
#endif /* CORE_CM4 */
#include "config_extern.h"
#include "common.h"

typedef struct ecu_comm_doip_ctx_tag ecu_comm_doip_ctx_t;

#ifdef CORE_CM4
static error_t ecu_comm_doip_rpc_executor_configure_unpack(uint32_t arg_index, const void *buffer, uint32_t capacity, uint32_t *p_pos, void *usrdata);
static error_t ecu_comm_doip_rpc_executor_configure_execute(uint32_t *p_resp_count, error_t *p_result, void *usrdata);
static error_t ecu_comm_doip_rpc_executor_reset_unpack(uint32_t arg_index, const void *buffer, uint32_t capacity, uint32_t *p_pos, void *usrdata);
static error_t ecu_comm_doip_rpc_executor_reset_execute(uint32_t *p_resp_count, error_t *p_result, void *usrdata);
#endif /* CORE_CM4 */

typedef struct ecu_comm_doip_ctx_tag {
#ifdef CORE_CM7
    ecu_comm_rpc_ctx_t rpc_configure;
    ecu_comm_rpc_ctx_t rpc_reset;
    const doip_config_t *config_default;
#endif /* CORE_CM7 */
#ifdef CORE_CM4
    doip_init_ctx_t init;
    doip_ctx_t *ctx;

    ecu_comm_instance_t rpc_instance;
    const void *rpc_pointer;
#endif /* CORE_CM4 */
}ecu_comm_doip_ctx_t;

#ifdef CORE_CM7
static const doip_config_t ecu_comm_doip_default_config[ECU_COMM_DOIP_MAX] = {
  {
      .protocol_version = 0x02,
      .port = 13400,
      .logical_address = 0x1000,
      .functional_address = 0xE400,
      .tester_address_start = 0x0E00,
      .tester_address_end = 0x0FFF,
      .max_connections = DOIP_CONNECTIONS_MAX,
      .eid = { 0x02, 0x00, 0x00, 0x00, 0x00, 0x01 },
      .gid = { 0x02, 0x00, 0x00, 0x00, 0x00, 0x01 },
      .anouncement_amount = 0,
      .anouncement_period = 1 * TIME_US_IN_S,
      .initial_timeout = 5 * TIME_US_IN_S,
      .inactivity_timeout = 15 * TIME_US_IN_S,
      .alive_timeout = 500 * TIME_US_IN_MS,
      .receive_timeout = 5 * TIME_US_IN_S,
  }, //ECU_DOIP_IF_1
};

static const bool ecu_comm_doip_enabled_default[ECU_COMM_DOIP_MAX] = {
    true
};
#endif /* CORE_CM7 */

static RAM_SECTION ecu_comm_doip_ctx_t ecu_comm_doip_ctx[ECU_COMM_DOIP_MAX] = {
    {
#ifdef CORE_CM4
      .init = {
          .error_callback = NULL,
      },
#endif /* CORE_CM4 */
#ifdef CORE_CM7
      .config_default = &ecu_comm_doip_default_config[ECU_COMM_DOIP_1],
#endif /* CORE_CM7 */
    },
};

#ifdef CORE_CM4
error_t ecu_comm_doip_init(ecu_comm_doip_t instance, doip_ctx_t *ctx)
{
  error_t err = E_OK;
  ecu_comm_doip_ctx_t *doip_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_COMM_DOIP_MAX || ctx == NULL, err = E_PARAM);

    doip_ctx = &ecu_comm_doip_ctx[instance];
    doip_ctx->ctx = ctx;

    err = doip_init(doip_ctx->ctx, &doip_ctx->init);
    BREAK_IF(err != E_OK);

    err = ecu_comm_set_comm_enabled(ECU_COMM_TYPE_DOIP, instance, false);
    BREAK_IF(err != E_OK);

    err = ecu_ipc_rpc_executor_register_func(ECU_IPC_RPC_EXECUTOR_1, ECU_IPC_RPC_FUNC_DOIP_CONFIGURE, 2u,
        ecu_comm_doip_rpc_executor_configure_unpack, ecu_comm_doip_rpc_executor_configure_execute, NULL, doip_ctx);
    BREAK_IF(err != E_OK);

    err = ecu_ipc_rpc_executor_register_func(ECU_IPC_RPC_EXECUTOR_1, ECU_IPC_RPC_FUNC_DOIP_RESET, 1u,
        ecu_comm_doip_rpc_executor_reset_unpack, ecu_comm_doip_rpc_executor_reset_execute, NULL, doip_ctx);
    BREAK_IF(err != E_OK);

  } while(0);

  return err;
}
#endif /* CORE_CM4 */

#ifdef CORE_CM7
error_t ecu_comm_doip_get_default_config(ecu_comm_doip_t instance, doip_config_t *config)
{
  error_t err = E_OK;
  ecu_comm_doip_ctx_t *doip_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_COMM_DOIP_MAX || config == NULL, err = E_PARAM);

    doip_ctx = &ecu_comm_doip_ctx[instance];

    memcpy(config, doip_ctx->config_default, sizeof(doip_config_t));
    config->enabled = ecu_comm_doip_enabled_default[instance];

  } while(0);

  return err;
}
#endif /* CORE_CM7 */

error_t ecu_comm_doip_configure(ecu_comm_doip_t instance, const doip_config_t *config)
{
  error_t err = E_OK;
  ecu_comm_doip_ctx_t *doip_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_COMM_DOIP_MAX || config == NULL, err = E_PARAM);

    doip_ctx = &ecu_comm_doip_ctx[instance];

#ifdef CORE_CM7
    err = ecu_comm_rpc_call(&doip_ctx->rpc_configure, ECU_IPC_RPC_FUNC_DOIP_CONFIGURE,
        instance, config, sizeof(*config), ALIGNOF(doip_config_t));
#endif /* CORE_CM7 */

#ifdef CORE_CM4
    err = doip_configure(doip_ctx->ctx, config);
    BREAK_IF(err != E_OK);

    err = ecu_comm_set_comm_enabled(ECU_COMM_TYPE_DOIP, instance, doip_ctx->ctx->config.enabled);
    BREAK_IF(err != E_OK);
#endif /* CORE_CM4 */

  } while(0);

  return err;
}

error_t ecu_comm_doip_reset(ecu_comm_doip_t instance)
{
  error_t err = E_OK;
  ecu_comm_doip_ctx_t *doip_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_COMM_DOIP_MAX, err = E_PARAM);

    doip_ctx = &ecu_comm_doip_ctx[instance];

#ifdef CORE_CM7
    err = ecu_comm_rpc_call(&doip_ctx->rpc_reset, ECU_IPC_RPC_FUNC_DOIP_RESET,
        instance, NULL, 0u, 1u);
#endif /* CORE_CM7 */

#ifdef CORE_CM4
    err = doip_reset(doip_ctx->ctx);
#endif /* CORE_CM4 */

  } while(0);

  return err;
}

#ifdef CORE_CM4
static error_t ecu_comm_doip_rpc_executor_configure_unpack(uint32_t arg_index, const void *buffer, uint32_t capacity, uint32_t *p_pos, void *usrdata)
{
  error_t err = E_OK;
  ecu_comm_doip_ctx_t *doip_ctx = usrdata;

  do {
    BREAK_IF_ACTION(doip_ctx == NULL, err = E_PARAM);

    switch(arg_index) {
      case 0:
        err = ipc_rpc_unpack(buffer, capacity,
            &doip_ctx->rpc_instance, sizeof(ecu_comm_instance_t),
            p_pos, ALIGNOF(ecu_comm_instance_t));
        break;
      case 1:
        err = ipc_rpc_unpack_ptr(buffer, capacity,
            &doip_ctx->rpc_pointer, sizeof(doip_config_t),
            p_pos, ALIGNOF(doip_config_t));
        break;
      default:
        err = E_PARAM;
        break;
    }

  } while(0);

  return err;
}

static error_t ecu_comm_doip_rpc_executor_configure_execute(uint32_t *p_resp_count, error_t *p_result, void *usrdata)
{
  error_t err = E_OK;
  ecu_comm_doip_ctx_t *doip_ctx = usrdata;

  do {
    BREAK_IF_ACTION(doip_ctx == NULL, err = E_PARAM);

    err = ecu_comm_doip_configure(doip_ctx->rpc_instance, doip_ctx->rpc_pointer);
    if(err != E_AGAIN) {
      *p_result = err;
      err = E_OK;
    }

  } while(0);

  return err;
}

static error_t ecu_comm_doip_rpc_executor_reset_unpack(uint32_t arg_index, const void *buffer, uint32_t capacity, uint32_t *p_pos, void *usrdata)
{
  error_t err = E_OK;
  ecu_comm_doip_ctx_t *doip_ctx = usrdata;

  do {
    BREAK_IF_ACTION(doip_ctx == NULL, err = E_PARAM);

    switch(arg_index) {
      case 0:
        err = ipc_rpc_unpack(buffer, capacity,
            &doip_ctx->rpc_instance, sizeof(ecu_comm_instance_t),
            p_pos, ALIGNOF(ecu_comm_instance_t));
        break;
      default:
        err = E_PARAM;
        break;
    }

  } while(0);

  return err;
}

static error_t ecu_comm_doip_rpc_executor_reset_execute(uint32_t *p_resp_count, error_t *p_result, void *usrdata)
{
  error_t err = E_OK;
  ecu_comm_doip_ctx_t *doip_ctx = usrdata;

  do {
    BREAK_IF_ACTION(doip_ctx == NULL, err = E_PARAM);

    err = ecu_comm_doip_reset(doip_ctx->rpc_instance);
    if(err != E_AGAIN) {
      *p_result = err;
      err = E_OK;
    }

  } while(0);

  return err;
}

#endif /* CORE_CM4 */
