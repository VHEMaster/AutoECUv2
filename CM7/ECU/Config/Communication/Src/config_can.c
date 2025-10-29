/*
 * config_can.c
 *
 *  Created on: Oct 19, 2025
 *      Author: VHEMaster
 */

#include "config_can.h"
#include "config_extern.h"
#include "common.h"

typedef struct ecu_comm_can_ctx_tag ecu_comm_can_ctx_t;

static void ecu_comm_can_rx_callback(can_ctx_t *ctx, const can_message_t *message, void *usrdata);
static void ecu_comm_can_err_callback(can_ctx_t *ctx, void *usrdata);

typedef struct {
    uint32_t msg_id;
    ecu_comm_can_t instance;
    ecu_comm_can_ctx_t *ctx;
    void *func;
    void *usrdata;
}ecu_comm_can_callback_usrdata_t;

typedef struct ecu_comm_can_ctx_tag {
  can_config_t config_default;
  can_init_ctx_t init;
  can_ctx_t *ctx;

  uint32_t rx_callbacks_count;
  uint32_t err_callbacks_count;
  ecu_comm_can_callback_usrdata_t rx_callbacks[CAN_RX_CB_MAX];
  ecu_comm_can_callback_usrdata_t err_callbacks[CAN_ERR_CB_MAX];
}ecu_comm_can_ctx_t;

static const can_config_t ecu_comm_can_default_config[ECU_COMM_CAN_MAX] = {
  {
    .baudrate = CAN_BAUDRATE_500KBPS,
    .global_filter = {
        .reject_remote_std = true,
        .reject_remote_ext = true,
        .reject_non_matching_std = true,
        .reject_non_matching_ext = true,
        .non_matching_std_fifo = CAN_FIFO_0,
        .non_matching_ext_fifo = CAN_FIFO_0,
    },
    .filter_config = {
      {
        .id_type = FDCAN_STANDARD_ID,
        .filter_type = FDCAN_FILTER_DUAL,
        .filter_config = FDCAN_FILTER_TO_RXFIFO0,
        .id1_filter = 0x7DF,
        .id2_mask = 0x7E0,
        .rx_buffer_index = 0,
      },
    },
  }, //ECU_CAN_IF_1
};

static ecu_comm_can_ctx_t ecu_comm_can_ctx[ECU_COMM_CAN_MAX] = {
    {
      .init = {
          .handle = &hfdcan1,
          .lbk_pin = { .port = FDCAN1_LBK_GPIO_Port, .pin = FDCAN1_LBK_Pin },
      },
      .config_default = ecu_comm_can_default_config[ECU_COMM_CAN_1],
    },
};

#if (USE_HAL_FDCAN_REGISTER_CALLBACKS == 1UL)

#define ECU_CONFIG_CAN_1ARG_CB(interface, type)  \
static ITCM_FUNC void ecu_config_can_##interface##_##type##_cb(CAN_HANDLE_TYPE *hcan)   \
{                                                         \
  can_ctx_t *can = ecu_comm_can_ctx[interface].ctx;            \
  if(can != NULL) {                                       \
    can_##type##_irq(can);                                \
  }                                                       \
}

#define ECU_CONFIG_CAN_2ARG_CB(interface, type)  \
static ITCM_FUNC void ecu_config_can_##interface##_##type##_cb(CAN_HANDLE_TYPE *hcan, uint32_t arg2)   \
{                                                         \
  can_ctx_t *can = ecu_comm_can_ctx[interface].ctx;            \
  if(can != NULL) {                                       \
    can_##type##_irq(can, arg2);                          \
  }                                                       \
}

#define ECU_CONFIG_CAN_IF_CB_DEFINE(interface)    \
    ECU_CONFIG_CAN_2ARG_CB(interface, rx_fifo0);  \
    ECU_CONFIG_CAN_2ARG_CB(interface, rx_fifo1);      \
    ECU_CONFIG_CAN_1ARG_CB(interface, rx_buffer);     \
    ECU_CONFIG_CAN_1ARG_CB(interface, error);         \

#define ECU_CONFIG_CAN_IF_CB_ASSIGN(can_ctx, interface)               \
    can_ctx->init.rx_fifo0_cb    = ecu_config_can_##interface##_rx_fifo0_cb;   \
    can_ctx->init.rx_fifo1_cb    = ecu_config_can_##interface##_rx_fifo1_cb;   \
    can_ctx->init.rx_buffer_cb   = ecu_config_can_##interface##_rx_buffer_cb;  \
    can_ctx->init.error_cb       = ecu_config_can_##interface##_error_cb;      \

ECU_CONFIG_CAN_IF_CB_DEFINE(ECU_COMM_CAN_1);

#else /* USE_HAL_FDCAN_REGISTER_CALLBACKS */

#define ECU_CONFIG_CAN_CB(hal_op, can_op)  \
INLINE ITCM_FUNC void HAL_FDCAN_##hal_op##Callback(CAN_HANDLE_TYPE *hcan)   \
{                                                                 \
  can_t *can = NULL;                                              \
  error_t err;                                                    \
  for(int i = 0; i < ECU_COMM_CAN_MAX; i++) {                       \
    can = ecu_comm_can_ctx[i].ctx;                                     \
    if(err == E_OK) {                                             \
      if(can->cfg.hcan == hcan) {                                 \
        can_##can_op##_irq(can);                                  \
        break;                                                    \
      }                                                           \
    }                                                             \
  }                                                               \
}                                                                 \

ECU_CONFIG_CAN_CB(RxFifo0, rx_fifo0);
ECU_CONFIG_CAN_CB(RxFifo1, rx_fifo1);
ECU_CONFIG_CAN_CB(RxBufferNewMessage, rx_buffer);
ECU_CONFIG_CAN_CB(Error, error);

#endif /* USE_HAL_FDCAN_REGISTER_CALLBACKS */

error_t ecu_comm_can_init(ecu_comm_can_t instance, can_ctx_t *ctx)
{
  error_t err = E_OK;
  ecu_comm_can_ctx_t *can_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_COMM_CAN_MAX || ctx == NULL, err = E_PARAM);

    can_ctx = &ecu_comm_can_ctx[instance];
    can_ctx->ctx = ctx;

#if (USE_HAL_FDCAN_REGISTER_CALLBACKS == 1UL)
    switch(instance) {
      case ECU_COMM_CAN_1:
        ECU_CONFIG_CAN_IF_CB_ASSIGN(can_ctx, ECU_COMM_CAN_1);
        break;
      default:
        err = E_PARAM;
        break;
    }
#endif /* USE_HAL_CAN_REGISTER_CALLBACKS */

    err = can_init(can_ctx->ctx, &can_ctx->init);
    BREAK_IF(err != E_OK);

  } while(0);

  return err;
}

error_t ecu_comm_can_get_default_config(ecu_comm_can_t instance, can_config_t *config)
{
  error_t err = E_OK;
  ecu_comm_can_ctx_t *can_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_COMM_CAN_MAX || config == NULL, err = E_PARAM);

    can_ctx = &ecu_comm_can_ctx[instance];

    memcpy(config, &can_ctx->config_default, sizeof(can_config_t));

  } while(0);

  return err;
}

error_t ecu_comm_can_configure(ecu_comm_can_t instance, const can_config_t *config)
{
  error_t err = E_OK;
  ecu_comm_can_ctx_t *can_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_COMM_CAN_MAX || config == NULL, err = E_PARAM);

    can_ctx = &ecu_comm_can_ctx[instance];

    memset(can_ctx->rx_callbacks, 0, sizeof(can_ctx->rx_callbacks));
    memset(can_ctx->err_callbacks, 0, sizeof(can_ctx->err_callbacks));
    can_ctx->rx_callbacks_count = 0;
    can_ctx->err_callbacks_count = 0;

    err = can_configure(can_ctx->ctx, config);

  } while(0);

  return err;
}

error_t ecu_comm_can_reset(ecu_comm_can_t instance)
{
  error_t err = E_OK;
  ecu_comm_can_ctx_t *can_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_COMM_CAN_MAX, err = E_PARAM);

    can_ctx = &ecu_comm_can_ctx[instance];

    err = can_reset(can_ctx->ctx);

  } while(0);

  return err;
}

error_t ecu_comm_can_transmit(ecu_comm_can_t instance, const can_message_t *message)
{
  error_t err = E_OK;
  ecu_comm_can_ctx_t *can_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_COMM_CAN_MAX, err = E_PARAM);

    can_ctx = &ecu_comm_can_ctx[instance];

    err = can_tx(can_ctx->ctx, message);

  } while(0);

  return err;
}

error_t ecu_comm_can_receive(ecu_comm_can_t instance, can_message_t *message)
{
  error_t err = E_OK;
  ecu_comm_can_ctx_t *can_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_COMM_CAN_MAX, err = E_PARAM);

    can_ctx = &ecu_comm_can_ctx[instance];

    err = can_rx(can_ctx->ctx, message);

  } while(0);

  return err;
}

error_t ecu_comm_can_register_rx_callback(ecu_comm_can_t instance, uint32_t msg_id, can_rx_callback_func_t func, void *usrdata)
{
  error_t err = E_OK;
  ecu_comm_can_ctx_t *can_ctx;
  ecu_comm_can_callback_usrdata_t *cb;
  uint32_t cb_count;

  do {
    BREAK_IF_ACTION(instance >= ECU_COMM_CAN_MAX, err = E_PARAM);

    can_ctx = &ecu_comm_can_ctx[instance];
    cb_count = can_ctx->rx_callbacks_count;

    BREAK_IF_ACTION(cb_count >= CAN_RX_CB_MAX, err = E_OVERFLOW);

    cb = &can_ctx->rx_callbacks[cb_count];
    cb->msg_id = msg_id;
    cb->instance = instance;
    cb->ctx = can_ctx;
    cb->func = func;
    cb->usrdata = usrdata;

    err = can_register_rx_callback(can_ctx->ctx, msg_id, ecu_comm_can_rx_callback, cb);
    if(err == E_OK) {
      can_ctx->rx_callbacks_count = ++cb_count;
    }

  } while(0);

  return err;
}

error_t ecu_comm_can_register_err_callback(ecu_comm_can_t instance, can_err_callback_func_t func, void *usrdata)
{
  error_t err = E_OK;
  ecu_comm_can_ctx_t *can_ctx;
  ecu_comm_can_callback_usrdata_t *cb;
  uint32_t cb_count;

  do {
    BREAK_IF_ACTION(instance >= ECU_COMM_CAN_MAX, err = E_PARAM);

    can_ctx = &ecu_comm_can_ctx[instance];
    cb_count = can_ctx->err_callbacks_count;

    BREAK_IF_ACTION(cb_count >= CAN_ERR_CB_MAX, err = E_OVERFLOW);

    cb = &can_ctx->err_callbacks[cb_count];
    cb->instance = instance;
    cb->ctx = can_ctx;
    cb->func = func;
    cb->usrdata = usrdata;

    err = can_register_err_callback(can_ctx->ctx, ecu_comm_can_err_callback, cb);
    if(err == E_OK) {
      can_ctx->rx_callbacks_count = ++cb_count;
    }

  } while(0);

  return err;
}

static void ecu_comm_can_rx_callback(can_ctx_t *ctx, const can_message_t *message, void *usrdata)
{
  ecu_comm_can_callback_usrdata_t *cb = (ecu_comm_can_callback_usrdata_t *)usrdata;
  ecu_comm_can_ctx_t *can_ctx;
  ecu_comm_can_rx_callback_t func;

  do {
    BREAK_IF(cb == NULL);
    can_ctx = cb->ctx;
    BREAK_IF(can_ctx == NULL);
    BREAK_IF(can_ctx->ctx != ctx);
    if(cb->func != NULL) {
      func = (ecu_comm_can_rx_callback_t)cb->func;
      func(cb->instance, message, cb->usrdata);
    }
  } while(0);
}

static void ecu_comm_can_err_callback(can_ctx_t *ctx, void *usrdata)
{
  ecu_comm_can_callback_usrdata_t *cb = (ecu_comm_can_callback_usrdata_t *)usrdata;
  ecu_comm_can_ctx_t *can_ctx;
  ecu_comm_can_err_callback_t func;

  do {
    BREAK_IF(cb == NULL);
    can_ctx = cb->ctx;
    BREAK_IF(can_ctx == NULL);
    BREAK_IF(can_ctx->ctx != ctx);
    if(cb->func != NULL) {
      func = (ecu_comm_can_err_callback_t)cb->func;
      func(cb->instance, cb->usrdata);
    }
  } while(0);
}
