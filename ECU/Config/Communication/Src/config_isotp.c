/*
 * config_isotp.c
 *
 *  Created on: Oct 23, 2025
 *      Author: vhemaster
 */

#include <string.h>
#include "config_isotp.h"
#include "ipc_rpc_pack.h"
#ifdef CORE_CM7
#include "config_comm_rpc.h"
#include "config_ipc.h"
#endif /* CORE_CM7 */
#ifdef CORE_CM4
#include "config_ipc_rpc_executor.h"
#endif /* CORE_CM4 */
#include "compiler.h"

typedef struct ecu_comm_isotp_ctx_tag ecu_comm_isotp_ctx_t;

#ifdef CORE_CM4
static void ecu_comm_isotp_error_callback(isotp_ctx_t *ctx, isotp_error_code_t code, void *userdata);

static error_t ecu_comm_isotp_rpc_executor_configure_unpack(uint32_t arg_index, const void *buffer, uint32_t capacity, uint32_t *p_pos, void *usrdata);
static error_t ecu_comm_isotp_rpc_executor_configure_execute(uint32_t *p_resp_count, void *usrdata);
static error_t ecu_comm_isotp_rpc_executor_reset_unpack(uint32_t arg_index, const void *buffer, uint32_t capacity, uint32_t *p_pos, void *usrdata);
static error_t ecu_comm_isotp_rpc_executor_reset_execute(uint32_t *p_resp_count, void *usrdata);
static error_t ecu_comm_isotp_rpc_executor_frame_write_upstream_unpack(uint32_t arg_index, const void *buffer, uint32_t capacity, uint32_t *p_pos, void *usrdata);
static error_t ecu_comm_isotp_rpc_executor_frame_write_upstream_execute(uint32_t *p_resp_count, void *usrdata);
static error_t ecu_comm_isotp_rpc_executor_data_write_downstream_unpack(uint32_t arg_index, const void *buffer, uint32_t capacity, uint32_t *p_pos, void *usrdata);
static error_t ecu_comm_isotp_rpc_executor_data_write_downstream_execute(uint32_t *p_resp_count, void *usrdata);
static error_t ecu_comm_isotp_rpc_executor_frame_read_downstream_execute(uint32_t *p_resp_count, void *usrdata);
static error_t ecu_comm_isotp_rpc_executor_frame_read_downstream_pack(uint32_t arg_index, void *buffer, uint32_t capacity, uint32_t *p_pos, void *usrdata);
static error_t ecu_comm_isotp_rpc_executor_data_read_upstream_unpack(uint32_t arg_index, const void *buffer, uint32_t capacity, uint32_t *p_pos, void *usrdata);
static error_t ecu_comm_isotp_rpc_executor_data_read_upstream_execute(uint32_t *p_resp_count, void *usrdata);
static error_t ecu_comm_isotp_rpc_executor_data_read_upstream_pack(uint32_t arg_index, void *buffer, uint32_t capacity, uint32_t *p_pos, void *usrdata);

#endif /* CORE_CM4 */

#ifdef CORE_CM7
static error_t ecu_comm_isotp_rpc_caller_data_write_downstream_start(uint32_t *func_id, uint32_t *args_count, void *usrdata);
static error_t ecu_comm_isotp_rpc_caller_data_write_downstream_pack(uint32_t arg_index,
    void *buffer, uint32_t capacity, uint32_t *p_pos, void *usrdata);

#endif /* CORE_CM7 */

typedef struct ecu_comm_isotp_ctx_tag {
#ifdef CORE_CM7
    ecu_comm_rpc_ctx_t rpc_configure;
    ecu_comm_rpc_ctx_t rpc_reset;
    ecu_comm_rpc_ctx_t rpc_frame_write_upstream;
    ecu_comm_rpc_ctx_t rpc_data_write_downstream;
    ecu_comm_rpc_ctx_t rpc_frame_read_downstream;
    ecu_comm_rpc_ctx_t rpc_data_read_upstream;
    const isotp_config_t *config_default;
#endif /* CORE_CM7 */
#ifdef CORE_CM4
    isotp_init_ctx_t init;
    isotp_ctx_t *ctx;

    ecu_comm_instance_t rpc_instance;
    const void *rpc_pointer;
    uint8_t rpc_payload[ISOTP_PAYLOAD_LEN_MAX];
    isotp_frame_t rpc_frame;
    uint16_t rpc_length;
#endif /* CORE_CM4 */

    isotp_error_code_t error_flag;
}ecu_comm_isotp_ctx_t;

#ifdef CORE_CM7
static const isotp_config_t ecu_comm_isotp_config_default = {
    .format = CAN_FRAME_FORMAT_CLASSIC,
    .timeout = 1000 * TIME_US_IN_MS,
    .upstream_block_size = 20,
    .upstream_min_separation_time = 0,
    .upstream_separation_time_gap = 200,
    .downstream_pass_early_ovf = false,
};

static const bool ecu_comm_isotp_enabled_default[ECU_COMM_ISOTP_MAX] = {
    true,
    true
};
#endif /* CORE_CM7 */

static RAM_SECTION ecu_comm_isotp_ctx_t ecu_comm_isotp_ctx[ECU_COMM_ISOTP_MAX] = {
    {
#ifdef CORE_CM4
      .init = {
          .error_callback = ecu_comm_isotp_error_callback,
          .callback_userdata = NULL,
      },
#endif /* CORE_CM4 */
#ifdef CORE_CM7
      .config_default = &ecu_comm_isotp_config_default,
#endif /* CORE_CM7 */
    }, // ECU_COMM_ISOTP_1
    {
#ifdef CORE_CM4
      .init = {
          .error_callback = ecu_comm_isotp_error_callback,
          .callback_userdata = NULL,
      },
#endif /* CORE_CM4 */
#ifdef CORE_CM7
      .config_default = &ecu_comm_isotp_config_default,
#endif /* CORE_CM7 */
    }, //ECU_COMM_ISOTP_2
};

#ifdef CORE_CM4
error_t ecu_comm_isotp_init(ecu_comm_isotp_t instance, isotp_ctx_t *ctx)
{
  error_t err = E_OK;
  ecu_comm_isotp_ctx_t *isotp_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_COMM_ISOTP_MAX || ctx == NULL, err = E_PARAM);

    isotp_ctx = &ecu_comm_isotp_ctx[instance];
    isotp_ctx->ctx = ctx;

    isotp_ctx->init.callback_userdata = (void *)isotp_ctx;

    err = isotp_init(isotp_ctx->ctx, &isotp_ctx->init);
    BREAK_IF(err != E_OK);

    isotp_ctx->error_flag = ISOTP_OK;

    err = ecu_comm_set_comm_enabled(ECU_COMM_TYPE_ISOTP, instance, false);
    BREAK_IF(err != E_OK);

    err = ecu_ipc_rpc_executor_register_func(ECU_IPC_RPC_EXECUTOR_1, ECU_IPC_RPC_FUNC_ISOTP_CONFIGURE, 2u,
        ecu_comm_isotp_rpc_executor_configure_unpack, ecu_comm_isotp_rpc_executor_configure_execute, NULL, isotp_ctx);
    BREAK_IF(err != E_OK);

    err = ecu_ipc_rpc_executor_register_func(ECU_IPC_RPC_EXECUTOR_1, ECU_IPC_RPC_FUNC_ISOTP_RESET, 1u,
        ecu_comm_isotp_rpc_executor_reset_unpack, ecu_comm_isotp_rpc_executor_reset_execute, NULL, isotp_ctx);
    BREAK_IF(err != E_OK);

    err = ecu_ipc_rpc_executor_register_func(ECU_IPC_RPC_EXECUTOR_1, ECU_IPC_RPC_FUNC_ISOTP_FRAME_WRITE_UPSTREAM, 2u,
        ecu_comm_isotp_rpc_executor_frame_write_upstream_unpack, ecu_comm_isotp_rpc_executor_frame_write_upstream_execute, NULL, isotp_ctx);
    BREAK_IF(err != E_OK);

    err = ecu_ipc_rpc_executor_register_func(ECU_IPC_RPC_EXECUTOR_1, ECU_IPC_RPC_FUNC_ISOTP_DATA_WRITE_DOWNSTREAM, 3u,
        ecu_comm_isotp_rpc_executor_data_write_downstream_unpack, ecu_comm_isotp_rpc_executor_data_write_downstream_execute, NULL, isotp_ctx);
    BREAK_IF(err != E_OK);

    err = ecu_ipc_rpc_executor_register_func(ECU_IPC_RPC_EXECUTOR_1, ECU_IPC_RPC_FUNC_ISOTP_FRAME_READ_DOWNSTREAM, 1u,
        ecu_comm_isotp_rpc_executor_reset_unpack, ecu_comm_isotp_rpc_executor_frame_read_downstream_execute, ecu_comm_isotp_rpc_executor_frame_read_downstream_pack, isotp_ctx);
    BREAK_IF(err != E_OK);

    err = ecu_ipc_rpc_executor_register_func(ECU_IPC_RPC_EXECUTOR_1, ECU_IPC_RPC_FUNC_ISOTP_DATA_READ_UPSTREAM, 2u,
        ecu_comm_isotp_rpc_executor_data_read_upstream_unpack, ecu_comm_isotp_rpc_executor_data_read_upstream_execute, ecu_comm_isotp_rpc_executor_data_read_upstream_pack, isotp_ctx);
    BREAK_IF(err != E_OK);

  } while(0);

  return err;
}
#endif /* CORE_CM4 */

#ifdef CORE_CM7
error_t ecu_comm_isotp_get_default_config(ecu_comm_isotp_t instance, isotp_config_t *config)
{
  error_t err = E_OK;
  ecu_comm_isotp_ctx_t *isotp_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_COMM_ISOTP_MAX || config == NULL, err = E_PARAM);

    isotp_ctx = &ecu_comm_isotp_ctx[instance];

    memcpy(config, isotp_ctx->config_default, sizeof(isotp_config_t));
    config->enabled = ecu_comm_isotp_enabled_default[instance];

  } while(0);

  return err;
}
#endif /* CORE_CM7 */

error_t ecu_comm_isotp_configure(ecu_comm_isotp_t instance, const isotp_config_t *config)
{
  error_t err = E_OK;
  ecu_comm_isotp_ctx_t *isotp_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_COMM_ISOTP_MAX || config == NULL, err = E_PARAM);

    isotp_ctx = &ecu_comm_isotp_ctx[instance];

#ifdef CORE_CM7
    err = ecu_comm_rpc_call(&isotp_ctx->rpc_configure, ECU_IPC_RPC_FUNC_ISOTP_CONFIGURE,
        instance, config, sizeof(*config), ALIGNOF(isotp_config_t));
#endif /* CORE_CM7 */

#ifdef CORE_CM4
    err = isotp_configure(isotp_ctx->ctx, config);
    BREAK_IF(err != E_OK);

    err = ecu_comm_set_comm_enabled(ECU_COMM_TYPE_ISOTP, instance, isotp_ctx->ctx->config.enabled);
    BREAK_IF(err != E_OK);
#endif /* CORE_CM4 */

  } while(0);

  return err;
}

error_t ecu_comm_isotp_reset(ecu_comm_isotp_t instance)
{
  error_t err = E_OK;
  ecu_comm_isotp_ctx_t *isotp_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_COMM_ISOTP_MAX, err = E_PARAM);

    isotp_ctx = &ecu_comm_isotp_ctx[instance];

#ifdef CORE_CM7
    err = ecu_comm_rpc_call(&isotp_ctx->rpc_reset, ECU_IPC_RPC_FUNC_ISOTP_RESET,
        instance, NULL, 0u, 1u);
#endif /* CORE_CM7 */

#ifdef CORE_CM4
    err = isotp_reset(isotp_ctx->ctx);
#endif /* CORE_CM4 */

  } while(0);

  return err;
}

#ifdef CORE_CM4
static void ecu_comm_isotp_error_callback(isotp_ctx_t *ctx, isotp_error_code_t code, void *userdata)
{
  ecu_comm_isotp_ctx_t *isotp_ctx;

  do {
    BREAK_IF(ctx == NULL);
    BREAK_IF(userdata == NULL);

    isotp_ctx = (ecu_comm_isotp_ctx_t *)userdata;
    BREAK_IF(isotp_ctx == NULL);
    BREAK_IF(isotp_ctx->ctx != ctx);

    isotp_ctx->error_flag = code;

  } while(0);
}
#endif /* CORE_CM4 */

error_t ecu_comm_isotp_frame_write_upstream(ecu_comm_isotp_t instance, const can_message_t *message)
{
  error_t err = E_OK;
  ecu_comm_isotp_ctx_t *isotp_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_COMM_ISOTP_MAX, err = E_PARAM);
    BREAK_IF_ACTION(message == NULL, err = E_PARAM);

    isotp_ctx = &ecu_comm_isotp_ctx[instance];

#ifdef CORE_CM7
    err = ecu_comm_rpc_call(&isotp_ctx->rpc_frame_write_upstream, ECU_IPC_RPC_FUNC_ISOTP_FRAME_WRITE_UPSTREAM,
        instance, message, sizeof(*message), ALIGNOF(can_message_t));
#endif /* CORE_CM7 */

#ifdef CORE_CM4
    err = isotp_frame_write_upstream(isotp_ctx->ctx, message);
#endif /* CORE_CM4 */

  } while(0);

  return err;
}

error_t ecu_comm_isotp_frame_read_downstream(ecu_comm_isotp_t instance, isotp_frame_t *frame)
{
  error_t err = E_OK;
  ecu_comm_isotp_ctx_t *isotp_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_COMM_ISOTP_MAX, err = E_PARAM);
    BREAK_IF_ACTION(frame == NULL, err = E_PARAM);

    isotp_ctx = &ecu_comm_isotp_ctx[instance];

#ifdef CORE_CM7
    err = ecu_comm_rpc_read(&isotp_ctx->rpc_frame_read_downstream, ECU_IPC_RPC_FUNC_ISOTP_FRAME_READ_DOWNSTREAM,
        instance, frame, sizeof(isotp_frame_t), ALIGNOF(isotp_frame_t), NULL);
#endif /* CORE_CM7 */

#ifdef CORE_CM4
    err = isotp_frame_read_downstream(isotp_ctx->ctx, frame);
#endif /* CORE_CM4 */

  } while(0);

  return err;
}

error_t ecu_comm_isotp_data_write_downstream(ecu_comm_isotp_t instance, const uint8_t *payload, uint16_t length)
{
  error_t err = E_OK;
  ecu_comm_isotp_ctx_t *isotp_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_COMM_ISOTP_MAX, err = E_PARAM);
    BREAK_IF_ACTION(payload == NULL, err = E_PARAM);

    isotp_ctx = &ecu_comm_isotp_ctx[instance];

#ifdef CORE_CM7
    BREAK_IF_ACTION(length == 0u || length > ISOTP_PAYLOAD_LEN_MAX, err = E_PARAM);
    ecu_comm_rpc_ctx_t *rpc_ctx = &isotp_ctx->rpc_data_write_downstream;

    if(rpc_ctx->fsm.state == IPC_RPC_CALLER_FSM_IDLE) {
      err = ecu_ipc_get_rpc_caller_ctx(ECU_IPC_RPC_CALLER_1, &rpc_ctx->caller);
      BREAK_IF(err != E_OK);
      BREAK_IF_ACTION(rpc_ctx->caller == NULL, err = E_FAULT);

      rpc_ctx->instance = instance;
      rpc_ctx->argument = payload;
      rpc_ctx->bytes = length;
    } else {
      BREAK_IF_ACTION(rpc_ctx->instance != instance || rpc_ctx->argument != payload ||
          rpc_ctx->bytes != length, err = E_BUSY);
    }

    err = ipc_rpc_caller_fsm(rpc_ctx->caller, &rpc_ctx->fsm,
        ecu_comm_isotp_rpc_caller_data_write_downstream_start,
        ecu_comm_isotp_rpc_caller_data_write_downstream_pack, NULL, NULL, rpc_ctx);
#endif /* CORE_CM7 */

#ifdef CORE_CM4
    err = isotp_data_write_downstream(isotp_ctx->ctx, payload, length);
#endif /* CORE_CM4 */

  } while(0);

  return err;
}

error_t ecu_comm_isotp_data_read_upstream(ecu_comm_isotp_t instance, uint8_t *payload, uint16_t *length)
{
  error_t err = E_OK;
  ecu_comm_isotp_ctx_t *isotp_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_COMM_ISOTP_MAX, err = E_PARAM);
    BREAK_IF_ACTION(payload == NULL, err = E_PARAM);
    BREAK_IF_ACTION(length == NULL, err = E_PARAM);

    isotp_ctx = &ecu_comm_isotp_ctx[instance];

#ifdef CORE_CM7
    err = ecu_comm_rpc_read(&isotp_ctx->rpc_data_read_upstream, ECU_IPC_RPC_FUNC_ISOTP_DATA_READ_UPSTREAM,
        instance, payload, 0u, ALIGNOF(uint8_t), length);
#endif /* CORE_CM7 */

#ifdef CORE_CM4
    err = isotp_data_read_upstream(isotp_ctx->ctx, payload, length);
#endif /* CORE_CM4 */

  } while(0);

  return err;
}

error_t ecu_comm_isotp_get_error_flag(ecu_comm_isotp_t instance, isotp_error_code_t *code)
{
  error_t err = E_OK;
  ecu_comm_isotp_ctx_t *isotp_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_COMM_ISOTP_MAX, err = E_PARAM);
    BREAK_IF_ACTION(code == NULL, err = E_PARAM);

    isotp_ctx = &ecu_comm_isotp_ctx[instance];

#ifdef CORE_CM7
    (void)isotp_ctx;
    err = E_NOTSUPPORT;
#endif /* CORE_CM7 */

#ifdef CORE_CM4
    *code = isotp_ctx->error_flag;
    isotp_ctx->error_flag = ISOTP_OK;
#endif /* CORE_CM4 */

  } while(0);

  return err;
}

#ifdef CORE_CM4
static error_t ecu_comm_isotp_rpc_executor_configure_unpack(uint32_t arg_index, const void *buffer, uint32_t capacity, uint32_t *p_pos, void *usrdata)
{
  error_t err = E_OK;
  ecu_comm_isotp_ctx_t *isotp_ctx = usrdata;

  do {
    BREAK_IF_ACTION(isotp_ctx == NULL, err = E_PARAM);

    switch(arg_index) {
      case 0:
        err = ipc_rpc_unpack(buffer, capacity,
            &isotp_ctx->rpc_instance, sizeof(ecu_comm_instance_t),
            p_pos, ALIGNOF(ecu_comm_instance_t));
        break;
      case 1:
        err = ipc_rpc_unpack_ptr(buffer, capacity,
            &isotp_ctx->rpc_pointer, sizeof(isotp_config_t),
            p_pos, ALIGNOF(isotp_config_t));
        break;
      default:
        err = E_PARAM;
        break;
    }

  } while(0);

  return err;
}

static error_t ecu_comm_isotp_rpc_executor_configure_execute(uint32_t *p_resp_count, void *usrdata)
{
  error_t err = E_OK;
  ecu_comm_isotp_ctx_t *isotp_ctx = usrdata;

  do {
    BREAK_IF_ACTION(isotp_ctx == NULL, err = E_PARAM);

    err = ecu_comm_isotp_configure(isotp_ctx->rpc_instance, isotp_ctx->rpc_pointer);

  } while(0);

  return err;
}

static error_t ecu_comm_isotp_rpc_executor_reset_unpack(uint32_t arg_index, const void *buffer, uint32_t capacity, uint32_t *p_pos, void *usrdata)
{
  error_t err = E_OK;
  ecu_comm_isotp_ctx_t *isotp_ctx = usrdata;

  do {
    BREAK_IF_ACTION(isotp_ctx == NULL, err = E_PARAM);

    switch(arg_index) {
      case 0:
        err = ipc_rpc_unpack(buffer, capacity,
            &isotp_ctx->rpc_instance, sizeof(ecu_comm_instance_t),
            p_pos, ALIGNOF(ecu_comm_instance_t));
        break;
      default:
        err = E_PARAM;
        break;
    }

  } while(0);

  return err;
}

static error_t ecu_comm_isotp_rpc_executor_reset_execute(uint32_t *p_resp_count, void *usrdata)
{
  error_t err = E_OK;
  ecu_comm_isotp_ctx_t *isotp_ctx = usrdata;

  do {
    BREAK_IF_ACTION(isotp_ctx == NULL, err = E_PARAM);

    err = ecu_comm_isotp_reset(isotp_ctx->rpc_instance);

  } while(0);

  return err;
}

static error_t ecu_comm_isotp_rpc_executor_frame_write_upstream_unpack(uint32_t arg_index, const void *buffer, uint32_t capacity, uint32_t *p_pos, void *usrdata)
{
  error_t err = E_OK;
  ecu_comm_isotp_ctx_t *isotp_ctx = usrdata;

  do {
    BREAK_IF_ACTION(isotp_ctx == NULL, err = E_PARAM);

    switch(arg_index) {
      case 0:
        err = ipc_rpc_unpack(buffer, capacity,
            &isotp_ctx->rpc_instance, sizeof(ecu_comm_instance_t),
            p_pos, ALIGNOF(ecu_comm_instance_t));
        break;
      case 1:
        err = ipc_rpc_unpack_ptr(buffer, capacity,
            &isotp_ctx->rpc_pointer, sizeof(can_message_t),
            p_pos, ALIGNOF(can_message_t));
        break;
      default:
        err = E_PARAM;
        break;
    }

  } while(0);

  return err;
}

static error_t ecu_comm_isotp_rpc_executor_frame_write_upstream_execute(uint32_t *p_resp_count, void *usrdata)
{
  error_t err = E_OK;
  ecu_comm_isotp_ctx_t *isotp_ctx = usrdata;

  do {
    BREAK_IF_ACTION(isotp_ctx == NULL, err = E_PARAM);

    err = ecu_comm_isotp_frame_write_upstream(isotp_ctx->rpc_instance, isotp_ctx->rpc_pointer);

  } while(0);

  return err;
}

static error_t ecu_comm_isotp_rpc_executor_data_write_downstream_unpack(uint32_t arg_index, const void *buffer, uint32_t capacity, uint32_t *p_pos, void *usrdata)
{
  error_t err = E_OK;
  ecu_comm_isotp_ctx_t *isotp_ctx = usrdata;

  do {
    BREAK_IF_ACTION(isotp_ctx == NULL, err = E_PARAM);

    switch(arg_index) {
      case 0:
        err = ipc_rpc_unpack(buffer, capacity,
            &isotp_ctx->rpc_instance, sizeof(ecu_comm_instance_t),
            p_pos, ALIGNOF(ecu_comm_instance_t));
        break;
      case 1:
        err = ipc_rpc_unpack(buffer, capacity,
            &isotp_ctx->rpc_length, sizeof(isotp_ctx->rpc_length),
            p_pos, ALIGNOF(uint16_t));
        break;
      case 2:
        err = ipc_rpc_unpack_ptr(buffer, capacity, &isotp_ctx->rpc_pointer,
            isotp_ctx->rpc_length, p_pos, ALIGNOF(uint8_t));
        break;
      default:
        err = E_PARAM;
        break;
    }

  } while(0);

  return err;
}

static error_t ecu_comm_isotp_rpc_executor_data_write_downstream_execute(uint32_t *p_resp_count, void *usrdata)
{
  error_t err = E_OK;
  ecu_comm_isotp_ctx_t *isotp_ctx = usrdata;

  do {
    BREAK_IF_ACTION(isotp_ctx == NULL, err = E_PARAM);

    err = ecu_comm_isotp_data_write_downstream(isotp_ctx->rpc_instance, isotp_ctx->rpc_pointer, isotp_ctx->rpc_length);

  } while(0);

  return err;
}

static error_t ecu_comm_isotp_rpc_executor_frame_read_downstream_execute(uint32_t *p_resp_count, void *usrdata)
{
  error_t err = E_OK;
  ecu_comm_isotp_ctx_t *isotp_ctx = usrdata;

  do {
    BREAK_IF_ACTION(isotp_ctx == NULL, err = E_PARAM);

    err = ecu_comm_isotp_frame_read_downstream(isotp_ctx->rpc_instance, &isotp_ctx->rpc_frame);
    BREAK_IF_ACTION(err == E_AGAIN, err = E_NOTRDY);

    if(err == E_OK) {
      *p_resp_count = 1u;
    }

  } while(0);

  return err;
}

static error_t ecu_comm_isotp_rpc_executor_frame_read_downstream_pack(uint32_t arg_index, void *buffer, uint32_t capacity, uint32_t *p_pos, void *usrdata)
{
  error_t err = E_OK;
  ecu_comm_isotp_ctx_t *isotp_ctx = usrdata;

  do {
    BREAK_IF_ACTION(isotp_ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(arg_index != 0u, err = E_PARAM);

    err = ipc_rpc_pack(buffer, capacity, &isotp_ctx->rpc_frame, sizeof(isotp_frame_t),
        p_pos, ALIGNOF(isotp_frame_t));

  } while(0);

  return err;
}

static error_t ecu_comm_isotp_rpc_executor_data_read_upstream_unpack(uint32_t arg_index, const void *buffer, uint32_t capacity, uint32_t *p_pos, void *usrdata)
{
  error_t err = E_OK;
  ecu_comm_isotp_ctx_t *isotp_ctx = usrdata;

  do {
    BREAK_IF_ACTION(isotp_ctx == NULL, err = E_PARAM);

    switch(arg_index) {
      case 0:
        err = ipc_rpc_unpack(buffer, capacity, &isotp_ctx->rpc_instance, sizeof(ecu_comm_instance_t),
            p_pos, ALIGNOF(ecu_comm_instance_t));
        break;
      case 1:
        err = ipc_rpc_unpack(buffer, capacity, &isotp_ctx->rpc_length, sizeof(isotp_ctx->rpc_length),
            p_pos, ALIGNOF(uint16_t));
        break;
      default:
        err = E_PARAM;
        break;
    }

  } while(0);

  return err;
}

static error_t ecu_comm_isotp_rpc_executor_data_read_upstream_execute(uint32_t *p_resp_count, void *usrdata)
{
  error_t err = E_OK;
  ecu_comm_isotp_ctx_t *isotp_ctx = usrdata;

  do {
    BREAK_IF_ACTION(isotp_ctx == NULL, err = E_PARAM);

    if(isotp_ctx->rpc_length > sizeof(isotp_ctx->rpc_payload)) {
      isotp_ctx->rpc_length = sizeof(isotp_ctx->rpc_payload);
    }

    err = ecu_comm_isotp_data_read_upstream(isotp_ctx->rpc_instance,
        isotp_ctx->rpc_payload, &isotp_ctx->rpc_length);
    BREAK_IF_ACTION(err == E_AGAIN, err = E_NOTRDY);

    if(err == E_OK) {
      *p_resp_count = 2u;
    }

  } while(0);

  return err;
}

static error_t ecu_comm_isotp_rpc_executor_data_read_upstream_pack(uint32_t arg_index, void *buffer, uint32_t capacity, uint32_t *p_pos, void *usrdata)
{
  error_t err = E_OK;
  ecu_comm_isotp_ctx_t *isotp_ctx = usrdata;

  do {
    BREAK_IF_ACTION(isotp_ctx == NULL, err = E_PARAM);

    switch(arg_index) {
      case 0:
        err = ipc_rpc_pack(buffer, capacity, &isotp_ctx->rpc_length, sizeof(isotp_ctx->rpc_length),
            p_pos, ALIGNOF(uint16_t));
        break;
      case 1:
        err = ipc_rpc_pack(buffer, capacity, isotp_ctx->rpc_payload, isotp_ctx->rpc_length,
            p_pos, ALIGNOF(uint8_t));
        break;
      default:
        err = E_PARAM;
        break;
    }

  } while(0);

  return err;
}

#endif /* CORE_CM4 */

#ifdef CORE_CM7
static error_t ecu_comm_isotp_rpc_caller_data_write_downstream_start(uint32_t *func_id, uint32_t *args_count, void *usrdata)
{
  (void)usrdata;
  *func_id = ECU_IPC_RPC_FUNC_ISOTP_DATA_WRITE_DOWNSTREAM;
  *args_count = 3u;

  return E_OK;
}

static error_t ecu_comm_isotp_rpc_caller_data_write_downstream_pack(uint32_t arg_index,
    void *buffer, uint32_t capacity, uint32_t *p_pos, void *usrdata)
{
  ecu_comm_rpc_ctx_t *rpc_ctx = usrdata;
  uint16_t length = (uint16_t)rpc_ctx->bytes;

  switch(arg_index) {
    case 0:
      return ipc_rpc_pack(buffer, capacity, &rpc_ctx->instance, sizeof(rpc_ctx->instance),
          p_pos, ALIGNOF(ecu_comm_instance_t));
    case 1:
      return ipc_rpc_pack(buffer, capacity, &length, sizeof(length), p_pos, ALIGNOF(uint16_t));
    case 2:
      return ipc_rpc_pack(buffer, capacity, rpc_ctx->argument, length, p_pos, ALIGNOF(uint8_t));
    default:
      return E_PARAM;
  }
}
#endif /* CORE_CM7 */
