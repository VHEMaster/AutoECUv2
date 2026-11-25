/*
 * config_usb.c
 *
 *  Created on: 17 нояб. 2026 г.
 *      Author: VHEMaster
 */

#include "config_usb.h"
#ifdef CORE_CM7
#include "config_comm_rpc.h"
#endif /* CORE_CM7 */
#ifdef CORE_CM4
#include "config_ipc_rpc_executor.h"
#include "ipc_rpc_pack.h"
#endif /* CORE_CM4 */
#include "config_extern.h"
#include "common.h"

typedef struct ecu_comm_usb_ctx_tag ecu_comm_usb_ctx_t;

typedef struct {
    usb_config_t config;
    uint32_t uid[3];
}usb_rpc_config_t;

#ifdef CORE_CM4
static error_t ecu_comm_usb_rpc_executor_configure_unpack(uint32_t arg_index, const void *buffer, uint32_t capacity, uint32_t *p_pos, void *usrdata);
static error_t ecu_comm_usb_rpc_executor_configure_execute(uint32_t *p_resp_count, error_t *p_result, void *usrdata);
static error_t ecu_comm_usb_rpc_executor_reset_unpack(uint32_t arg_index, const void *buffer, uint32_t capacity, uint32_t *p_pos, void *usrdata);
static error_t ecu_comm_usb_rpc_executor_reset_execute(uint32_t *p_resp_count, error_t *p_result, void *usrdata);
#endif /* CORE_CM4 */

typedef struct ecu_comm_usb_ctx_tag {
#ifdef CORE_CM7
    ecu_comm_rpc_ctx_t rpc_configure;
    ecu_comm_rpc_ctx_t rpc_reset;
    const usb_config_t *config_default;
    usb_rpc_config_t rpc_config;
#endif /* CORE_CM7 */
#ifdef CORE_CM4
    usb_ctx_t *ctx;

    ecu_comm_instance_t rpc_instance;
    const void *rpc_pointer;
#endif /* CORE_CM4 */
}ecu_comm_usb_ctx_t;

#ifdef CORE_CM7
static const usb_config_t ecu_comm_usb_default_config[ECU_COMM_USB_MAX] = {
  {
      /* Existing development IDs; use allocated VID/PID for production. */
      .vid = 0x0483u,
      .pid = 0x5740u,
  }, //ECU_USB_IF_1
};

static const bool ecu_comm_usb_enabled_default[ECU_COMM_USB_MAX] = {
    true
};
#endif /* CORE_CM7 */

static RAM_SECTION ecu_comm_usb_ctx_t ecu_comm_usb_ctx[ECU_COMM_USB_MAX] = {
    {

#ifdef CORE_CM7
      .config_default = &ecu_comm_usb_default_config[ECU_COMM_USB_1],
#endif /* CORE_CM7 */
    },
};

#ifdef CORE_CM4
error_t ecu_comm_usb_init(ecu_comm_usb_t instance, usb_ctx_t *ctx)
{
  error_t err = E_OK;
  ecu_comm_usb_ctx_t *usb_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_COMM_USB_MAX || ctx == NULL, err = E_PARAM);

    usb_ctx = &ecu_comm_usb_ctx[instance];
    usb_ctx->ctx = ctx;

    err = usb_init(usb_ctx->ctx);
    BREAK_IF(err != E_OK);

    err = ecu_comm_set_comm_enabled(ECU_COMM_TYPE_USB, instance, false);
    BREAK_IF(err != E_OK);

    err = ecu_ipc_rpc_executor_register_func(ECU_IPC_RPC_EXECUTOR_1, ECU_IPC_RPC_FUNC_USB_CONFIGURE, 2u,
        ecu_comm_usb_rpc_executor_configure_unpack, ecu_comm_usb_rpc_executor_configure_execute, NULL, usb_ctx);
    BREAK_IF(err != E_OK);

    err = ecu_ipc_rpc_executor_register_func(ECU_IPC_RPC_EXECUTOR_1, ECU_IPC_RPC_FUNC_USB_RESET, 1u,
        ecu_comm_usb_rpc_executor_reset_unpack, ecu_comm_usb_rpc_executor_reset_execute, NULL, usb_ctx);
    BREAK_IF(err != E_OK);

  } while(0);

  return err;
}
#endif /* CORE_CM4 */

#ifdef CORE_CM7
error_t ecu_comm_usb_get_default_config(ecu_comm_usb_t instance, usb_config_t *config)
{
  error_t err = E_OK;
  ecu_comm_usb_ctx_t *usb_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_COMM_USB_MAX || config == NULL, err = E_PARAM);

    usb_ctx = &ecu_comm_usb_ctx[instance];

    memcpy(config, usb_ctx->config_default, sizeof(usb_config_t));
    config->enabled = ecu_comm_usb_enabled_default[instance];

  } while(0);

  return err;
}
#endif /* CORE_CM7 */

#ifdef CORE_CM4
static error_t ecu_comm_usb_configure_local(ecu_comm_usb_t instance, usb_ctx_t *ctx,
    const usb_config_t *config, const uint32_t uid[3])
{
  error_t err = usb_configure(ctx, config, uid);
  error_t enabled_err;

  if(err != E_AGAIN) {
    enabled_err = ecu_comm_set_comm_enabled(ECU_COMM_TYPE_USB, instance, ctx->configured);
    if(err == E_OK) {
      err = enabled_err;
    }
  }

  return err;
}
#endif /* CORE_CM4 */

error_t ecu_comm_usb_configure(ecu_comm_usb_t instance, const usb_config_t *config)
{
  error_t err = E_OK;
  ecu_comm_usb_ctx_t *usb_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_COMM_USB_MAX || config == NULL, err = E_PARAM);

    usb_ctx = &ecu_comm_usb_ctx[instance];

#ifdef CORE_CM7
    if(usb_ctx->rpc_configure.fsm.state == IPC_RPC_CALLER_FSM_IDLE) {
      memcpy(&usb_ctx->rpc_config.config, config, sizeof(*config));
      usb_ctx->rpc_config.uid[0] = HAL_GetUIDw0();
      usb_ctx->rpc_config.uid[1] = HAL_GetUIDw1();
      usb_ctx->rpc_config.uid[2] = HAL_GetUIDw2();
    }
    err = ecu_comm_rpc_call(&usb_ctx->rpc_configure, ECU_IPC_RPC_FUNC_USB_CONFIGURE,
        instance, &usb_ctx->rpc_config, sizeof(usb_rpc_config_t), ALIGNOF(usb_rpc_config_t));
#endif /* CORE_CM7 */

#ifdef CORE_CM4
    BREAK_IF_ACTION(usb_ctx->ctx == NULL, err = E_INVALACT);
    err = ecu_comm_usb_configure_local(instance, usb_ctx->ctx, config, usb_ctx->ctx->uid);
#endif /* CORE_CM4 */

  } while(0);

  return err;
}

error_t ecu_comm_usb_reset(ecu_comm_usb_t instance)
{
  error_t err = E_OK;
  ecu_comm_usb_ctx_t *usb_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_COMM_USB_MAX, err = E_PARAM);

    usb_ctx = &ecu_comm_usb_ctx[instance];

#ifdef CORE_CM7
    err = ecu_comm_rpc_call(&usb_ctx->rpc_reset, ECU_IPC_RPC_FUNC_USB_RESET,
        instance, NULL, 0u, 1u);
#endif /* CORE_CM7 */

#ifdef CORE_CM4
    err = usb_reset(usb_ctx->ctx);
    if(err == E_OK) err = ecu_comm_set_comm_enabled(ECU_COMM_TYPE_USB, instance, false);
#endif /* CORE_CM4 */

  } while(0);

  return err;
}

#ifdef CORE_CM4
static error_t ecu_comm_usb_rpc_executor_configure_unpack(uint32_t arg_index, const void *buffer, uint32_t capacity, uint32_t *p_pos, void *usrdata)
{
  error_t err = E_OK;
  ecu_comm_usb_ctx_t *usb_ctx = usrdata;

  do {
    BREAK_IF_ACTION(usb_ctx == NULL, err = E_PARAM);

    switch(arg_index) {
      case 0:
        err = ipc_rpc_unpack(buffer, capacity,
            &usb_ctx->rpc_instance, sizeof(ecu_comm_instance_t),
            p_pos, ALIGNOF(ecu_comm_instance_t));
        break;
      case 1:
        err = ipc_rpc_unpack_ptr(buffer, capacity,
            &usb_ctx->rpc_pointer, sizeof(usb_rpc_config_t),
            p_pos, ALIGNOF(usb_rpc_config_t));
        break;
      default:
        err = E_PARAM;
        break;
    }

  } while(0);

  return err;
}

static error_t ecu_comm_usb_rpc_executor_configure_execute(uint32_t *p_resp_count, error_t *p_result, void *usrdata)
{
  error_t err = E_OK;
  ecu_comm_usb_ctx_t *usb_ctx = usrdata;
  const usb_rpc_config_t *rpc;

  do {
    BREAK_IF_ACTION(usb_ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION((rpc = usb_ctx->rpc_pointer) == NULL || usb_ctx->rpc_instance >= ECU_COMM_USB_MAX, err = E_PARAM);
    BREAK_IF_ACTION(usb_ctx != &ecu_comm_usb_ctx[usb_ctx->rpc_instance], err = E_PARAM);

    BREAK_IF_ACTION(usb_ctx->ctx == NULL, err = E_INVALACT);
    err = ecu_comm_usb_configure_local(usb_ctx->rpc_instance, usb_ctx->ctx, &rpc->config, rpc->uid);
    if(err != E_AGAIN) {
      *p_result = err;
      err = E_OK;
    }

  } while(0);

  return err;
}

static error_t ecu_comm_usb_rpc_executor_reset_unpack(uint32_t arg_index, const void *buffer, uint32_t capacity, uint32_t *p_pos, void *usrdata)
{
  error_t err = E_OK;
  ecu_comm_usb_ctx_t *usb_ctx = usrdata;

  do {
    BREAK_IF_ACTION(usb_ctx == NULL, err = E_PARAM);

    switch(arg_index) {
      case 0:
        err = ipc_rpc_unpack(buffer, capacity,
            &usb_ctx->rpc_instance, sizeof(ecu_comm_instance_t),
            p_pos, ALIGNOF(ecu_comm_instance_t));
        break;
      default:
        err = E_PARAM;
        break;
    }

  } while(0);

  return err;
}

static error_t ecu_comm_usb_rpc_executor_reset_execute(uint32_t *p_resp_count, error_t *p_result, void *usrdata)
{
  error_t err = E_OK;
  ecu_comm_usb_ctx_t *usb_ctx = usrdata;

  do {
    BREAK_IF_ACTION(usb_ctx == NULL, err = E_PARAM);

    err = ecu_comm_usb_reset(usb_ctx->rpc_instance);
    if(err != E_AGAIN) {
      *p_result = err;
      err = E_OK;
    }

  } while(0);

  return err;
}

#endif /* CORE_CM4 */
