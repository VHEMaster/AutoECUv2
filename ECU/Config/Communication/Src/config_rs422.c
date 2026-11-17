/*
 * config_rs422.c
 *
 *  Created on: Oct 19, 2025
 *      Author: VHEMaster
 */

#include "config_rs422.h"
#include "config_extern.h"
#include "common.h"

#ifdef CORE_CM7
#include "config_comm_rpc.h"
#endif /* CORE_CM7 */
#ifdef CORE_CM4
#include "config_ipc_rpc_executor.h"
#include "ipc_rpc_pack.h"
#endif /* CORE_CM4 */

typedef struct ecu_comm_rs422_ctx_tag ecu_comm_rs422_ctx_t;
#ifdef CORE_CM4

#define ECU_COMM_RS422_BUFFER_RX_SIZE   (4096u)
#define ECU_COMM_RS422_BUFFER_TX_SIZE   (8192u)

static uint8_t ecu_comm_rs422_buffer_rx[ECU_COMM_RS422_BUFFER_RX_SIZE];
static uint8_t ecu_comm_rs422_buffer_tx[ECU_COMM_RS422_BUFFER_TX_SIZE];

static error_t ecu_comm_rs422_rpc_executor_configure_unpack(uint32_t arg_index, const void *buffer, uint32_t capacity, uint32_t *p_pos, void *usrdata);
static error_t ecu_comm_rs422_rpc_executor_configure_execute(uint32_t *p_resp_count, error_t *p_result, void *usrdata);
static error_t ecu_comm_rs422_rpc_executor_reset_unpack(uint32_t arg_index, const void *buffer, uint32_t capacity, uint32_t *p_pos, void *usrdata);
static error_t ecu_comm_rs422_rpc_executor_reset_execute(uint32_t *p_resp_count, error_t *p_result, void *usrdata);

#endif /* CORE_CM4 */

typedef struct ecu_comm_rs422_ctx_tag {
#ifdef CORE_CM7
  const rs422_config_t *config_default;
#endif /* CORE_CM7 */
#ifdef CORE_CM4
  rs422_init_ctx_t init;
  rs422_ctx_t *ctx;

  ecu_comm_instance_t rpc_instance;
  void *rpc_pointer;
#endif /* CORE_CM4 */

#ifdef CORE_CM7
  ecu_comm_rpc_ctx_t rpc_configure;
  ecu_comm_rpc_ctx_t rpc_reset;
#endif /* CORE_CM7 */

}ecu_comm_rs422_ctx_t;

#ifdef CORE_CM7
static const rs422_config_t ecu_comm_rs422_default_config[ECU_COMM_RS422_MAX] = {
  {
    .baudrate = 115200,
  }, //ECU_RS422_IF_1
};

static const bool ecu_comm_rs422_enabled_default[ECU_COMM_RS422_MAX] = {
    true,
};
#endif /* CORE_CM7 */

static RAM_SECTION ecu_comm_rs422_ctx_t ecu_comm_rs422_ctx[ECU_COMM_RS422_MAX] = {
    {
#ifdef CORE_CM4
      .init = {
          .handle = &huart1,
          .hdma = &hdma_m2m_stream2,
          .dma_copy_threshold = 256u,
          .dma_tx_threshold = 4u,
          .buffer_rx = ecu_comm_rs422_buffer_rx,
          .buffer_rx_size = ECU_COMM_RS422_BUFFER_RX_SIZE,
          .buffer_tx = ecu_comm_rs422_buffer_tx,
          .buffer_tx_size = ECU_COMM_RS422_BUFFER_TX_SIZE,
      },
#endif /* CORE_CM4 */
#ifdef CORE_CM7
      .config_default = &ecu_comm_rs422_default_config[ECU_COMM_RS422_1],
#endif /* CORE_CM7 */
    },
};

#ifdef CORE_CM4
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1UL)

#define ECU_CONFIG_RS422_1ARG_CB(interface, type)  \
static void ecu_config_rs422_##interface##_##type##_cb(UART_HandleTypeDef *huart)   \
{                                                         \
  rs422_ctx_t *ctx = ecu_comm_rs422_ctx[interface].ctx;            \
  if(ctx != NULL) {                                       \
    rs422_##type##_irq(ctx);                                \
  }                                                       \
}

#define ECU_CONFIG_RS422_IF_CB_DEFINE(interface)    \
    ECU_CONFIG_RS422_1ARG_CB(interface, rx_cplt);  \
    ECU_CONFIG_RS422_1ARG_CB(interface, rx_halfcplt);      \
    ECU_CONFIG_RS422_1ARG_CB(interface, tx_cplt);     \
    ECU_CONFIG_RS422_1ARG_CB(interface, error);         \

#define ECU_CONFIG_RS422_IF_CB_ASSIGN(rs422_ctx, interface)               \
    rs422_ctx->init.rx_cplt_cb    = ecu_config_rs422_##interface##_rx_cplt_cb;   \
    rs422_ctx->init.rx_halfcplt_cb    = ecu_config_rs422_##interface##_rx_halfcplt_cb;   \
    rs422_ctx->init.tx_cplt_cb   = ecu_config_rs422_##interface##_tx_cplt_cb;  \
    rs422_ctx->init.error_cb       = ecu_config_rs422_##interface##_error_cb;      \

ECU_CONFIG_RS422_IF_CB_DEFINE(ECU_COMM_RS422_1);

#else /* USE_HAL_UART_REGISTER_CALLBACKS */

#define ECU_CONFIG_RS422_CB(hal_op, rs422_op)  \
INLINE void HAL_UART_##hal_op##Callback(UART_HandleTypeDef *huart)   \
{                                                                 \
  rs422_ctx_t *ctx = NULL;                                              \
  error_t err;                                                    \
  for(int i = 0; i < ECU_COMM_RS422_MAX; i++) {                       \
    ctx = ecu_comm_rs422_ctx[i].ctx;                                     \
    if(err == E_OK) {                                             \
      if(ctx->init.handle == huart) {                                 \
        rs422_##rs422_op##_irq(ctx);                                  \
        break;                                                    \
      }                                                           \
    }                                                             \
  }                                                               \
}                                                                 \

ECU_CONFIG_RS422_CB(RxCplt, rx_cplt);
ECU_CONFIG_RS422_CB(RxHalfCplt, rx_halfcplt);
ECU_CONFIG_RS422_CB(TxCplt, tx_cplt);
ECU_CONFIG_RS422_CB(Error, error);

#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
#endif /* CORE_CM4 */

#ifdef CORE_CM4
error_t ecu_comm_rs422_init(ecu_comm_rs422_t instance, rs422_ctx_t *ctx)
{
  error_t err = E_OK;
  ecu_comm_rs422_ctx_t *rs422_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_COMM_RS422_MAX || ctx == NULL, err = E_PARAM);

    rs422_ctx = &ecu_comm_rs422_ctx[instance];
    rs422_ctx->ctx = ctx;

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1UL)
    switch(instance) {
      case ECU_COMM_RS422_1:
        ECU_CONFIG_RS422_IF_CB_ASSIGN(rs422_ctx, ECU_COMM_RS422_1);
        break;
      default:
        err = E_PARAM;
        break;
    }
#endif /* USE_HAL_RS422_REGISTER_CALLBACKS */

    err = rs422_init(rs422_ctx->ctx, &rs422_ctx->init);
    BREAK_IF(err != E_OK);

    err = ecu_comm_set_comm_enabled(ECU_COMM_TYPE_RS422, instance, false);
    BREAK_IF(err != E_OK);

    err = ecu_ipc_rpc_executor_register_func(ECU_IPC_RPC_EXECUTOR_1, ECU_IPC_RPC_FUNC_RS422_CONFIGURE, 2u,
        ecu_comm_rs422_rpc_executor_configure_unpack, ecu_comm_rs422_rpc_executor_configure_execute, NULL, rs422_ctx);
    BREAK_IF(err != E_OK);

    err = ecu_ipc_rpc_executor_register_func(ECU_IPC_RPC_EXECUTOR_1, ECU_IPC_RPC_FUNC_RS422_RESET, 1u,
        ecu_comm_rs422_rpc_executor_reset_unpack, ecu_comm_rs422_rpc_executor_reset_execute, NULL, rs422_ctx);
    BREAK_IF(err != E_OK);

  } while(0);

  return err;
}
#endif /* CORE_CM4 */

#ifdef CORE_CM7
error_t ecu_comm_rs422_get_default_config(ecu_comm_rs422_t instance, rs422_config_t *config)
{
  error_t err = E_OK;
  ecu_comm_rs422_ctx_t *rs422_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_COMM_RS422_MAX || config == NULL, err = E_PARAM);

    rs422_ctx = &ecu_comm_rs422_ctx[instance];

    memcpy(config, rs422_ctx->config_default, sizeof(rs422_config_t));
    config->enabled = ecu_comm_rs422_enabled_default[instance];

  } while(0);

  return err;
}
#endif /* CORE_CM7 */

error_t ecu_comm_rs422_configure(ecu_comm_rs422_t instance, const rs422_config_t *config)
{
  error_t err = E_OK;
  ecu_comm_rs422_ctx_t *rs422_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_COMM_RS422_MAX || config == NULL, err = E_PARAM);

    rs422_ctx = &ecu_comm_rs422_ctx[instance];

#ifdef CORE_CM7
    err = ecu_comm_rpc_call(&rs422_ctx->rpc_configure, ECU_IPC_RPC_FUNC_RS422_CONFIGURE,
        instance, config, sizeof(*config), ALIGNOF(rs422_config_t));
#endif /* CORE_CM7 */

#ifdef CORE_CM4
    err = rs422_configure(rs422_ctx->ctx, config);
    BREAK_IF(err != E_OK);

    err = ecu_comm_set_comm_enabled(ECU_COMM_TYPE_RS422, instance, rs422_ctx->ctx->config.enabled);
    BREAK_IF(err != E_OK);
#endif /* CORE_CM4 */

  } while(0);

  return err;
}

error_t ecu_comm_rs422_reset(ecu_comm_rs422_t instance)
{
  error_t err = E_OK;
  ecu_comm_rs422_ctx_t *rs422_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_COMM_RS422_MAX, err = E_PARAM);

    rs422_ctx = &ecu_comm_rs422_ctx[instance];

#ifdef CORE_CM7
    err = ecu_comm_rpc_call(&rs422_ctx->rpc_reset, ECU_IPC_RPC_FUNC_RS422_RESET,
        instance, NULL, 0u, 1u);
#endif /* CORE_CM7 */

#ifdef CORE_CM4
    err = rs422_reset(rs422_ctx->ctx);
#endif /* CORE_CM4 */

  } while(0);

  return err;
}

error_t ecu_comm_rs422_transmit(ecu_comm_rs422_t instance, const void *buffer, uint32_t length)
{
  error_t err = E_OK;
  ecu_comm_rs422_ctx_t *rs422_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_COMM_RS422_MAX, err = E_PARAM);
    BREAK_IF_ACTION(buffer == NULL && length != 0, err = E_PARAM);

    rs422_ctx = &ecu_comm_rs422_ctx[instance];

#ifdef CORE_CM7
    (void)rs422_ctx;
    err = E_NOTSUPPORT;
#endif /* CORE_CM7 */

#ifdef CORE_CM4
    err = rs422_tx(rs422_ctx->ctx, buffer, length);
#endif /* CORE_CM4 */

  } while(0);

  return err;
}

error_t ecu_comm_rs422_receive(ecu_comm_rs422_t instance, void *buffer, uint32_t *p_length)
{
  error_t err = E_OK;
  ecu_comm_rs422_ctx_t *rs422_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_COMM_RS422_MAX, err = E_PARAM);
    BREAK_IF_ACTION(buffer == NULL, err = E_PARAM);
    BREAK_IF_ACTION(p_length == NULL, err = E_PARAM);

    rs422_ctx = &ecu_comm_rs422_ctx[instance];

#ifdef CORE_CM7
    (void)rs422_ctx;
    err = E_NOTSUPPORT;
#endif /* CORE_CM7 */

#ifdef CORE_CM4
    err = rs422_rx(rs422_ctx->ctx, buffer, p_length);
#endif /* CORE_CM4 */

  } while(0);

  return err;
}

#ifdef CORE_CM4

static error_t ecu_comm_rs422_rpc_executor_configure_unpack(uint32_t arg_index, const void *buffer, uint32_t capacity, uint32_t *p_pos, void *usrdata)
{
  error_t err = E_OK;
  ecu_comm_rs422_ctx_t *rs422_ctx = usrdata;

  do {
    BREAK_IF_ACTION(rs422_ctx == NULL, err = E_PARAM);

    switch(arg_index) {
      case 0:
        err = ipc_rpc_unpack(buffer, capacity,
            &rs422_ctx->rpc_instance, sizeof(ecu_comm_instance_t),
            p_pos, ALIGNOF(ecu_comm_instance_t));
        break;
      case 1:
        err = ipc_rpc_unpack_ptr(buffer, capacity,
            (const void **)&rs422_ctx->rpc_pointer, sizeof(rs422_config_t),
            p_pos, ALIGNOF(rs422_config_t));
        break;
      default:
        err = E_PARAM;
        break;
    }

  } while(0);

  return err;
}

static error_t ecu_comm_rs422_rpc_executor_configure_execute(uint32_t *p_resp_count, error_t *p_result, void *usrdata)
{
  error_t err = E_OK;
  ecu_comm_rs422_ctx_t *rs422_ctx = usrdata;

  do {
    BREAK_IF_ACTION(rs422_ctx == NULL, err = E_PARAM);

    err = ecu_comm_rs422_configure(rs422_ctx->rpc_instance, rs422_ctx->rpc_pointer);
    if(err != E_AGAIN) {
      *p_result = err;
      err = E_OK;
    }

  } while(0);

  return err;
}

static error_t ecu_comm_rs422_rpc_executor_reset_unpack(uint32_t arg_index, const void *buffer, uint32_t capacity, uint32_t *p_pos, void *usrdata)
{
  error_t err = E_OK;
  ecu_comm_rs422_ctx_t *rs422_ctx = usrdata;

  do {
    BREAK_IF_ACTION(rs422_ctx == NULL, err = E_PARAM);

    switch(arg_index) {
      case 0:
        err = ipc_rpc_unpack(buffer, capacity,
            &rs422_ctx->rpc_instance, sizeof(ecu_comm_instance_t),
            p_pos, ALIGNOF(ecu_comm_instance_t));
        break;
      default:
        err = E_PARAM;
        break;
    }

  } while(0);

  return err;
}

static error_t ecu_comm_rs422_rpc_executor_reset_execute(uint32_t *p_resp_count, error_t *p_result, void *usrdata)
{
  error_t err = E_OK;
  ecu_comm_rs422_ctx_t *rs422_ctx = usrdata;

  do {
    BREAK_IF_ACTION(rs422_ctx == NULL, err = E_PARAM);

    err = ecu_comm_rs422_reset(rs422_ctx->rpc_instance);
    if(err != E_AGAIN) {
      *p_result = err;
      err = E_OK;
    }

  } while(0);

  return err;
}

#endif /* CORE_CM4 */
