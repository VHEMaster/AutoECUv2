/*
 * config_can.c
 *
 *  Created on: Oct 19, 2025
 *      Author: VHEMaster
 */

#include <string.h>
#include "config_can.h"
#include "config_extern.h"
#include "middlelayer_can.h"
#include "compiler.h"
#include "errors.h"
#include "can.h"

typedef struct {
  can_cfg_t can_cfg;
}ecu_config_can_if_t;

static const ecu_config_can_if_t ecu_config_can[ECU_CAN_IF_MAX] = {
  {
    .can_cfg = {
      .handle = &hfdcan1,
      .lbk_pin = { .port = FDCAN1_LBK_GPIO_Port, .pin = FDCAN1_LBK_Pin },
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
          .IdType = FDCAN_STANDARD_ID,
          .FilterIndex = 0,
          .FilterType = FDCAN_FILTER_DUAL,
          .FilterConfig = FDCAN_FILTER_TO_RXFIFO0,
          .FilterID1 = 0x7DF,
          .FilterID2 = 0x7E0,
        },
      },
    },
  }, //ECU_CAN_IF_MAIN
};

#if (USE_HAL_FDCAN_REGISTER_CALLBACKS == 1UL)

#define ECU_CONFIG_CAN_1ARG_CB(interface, type)  \
static ITCM_FUNC void ecu_config_can_##interface##_##type##_cb(CAN_HANDLE_TYPE *hcan)   \
{                                                         \
  can_ctx_t *can = NULL;                                  \
  error_t err;                                            \
  err = middlelayer_can_get_if(&can, interface);          \
  if(err == E_OK && can != NULL) {                        \
    can_##type##_irq(can);                                \
  }                                                       \
}

#define ECU_CONFIG_CAN_2ARG_CB(interface, type)  \
static ITCM_FUNC void ecu_config_can_##interface##_##type##_cb(CAN_HANDLE_TYPE *hcan, uint32_t arg2)   \
{                                                         \
  can_ctx_t *can = NULL;                                  \
  error_t err;                                            \
  err = middlelayer_can_get_if(&can, interface);          \
  if(err == E_OK && can != NULL) {                        \
    can_##type##_irq(can, arg2);                          \
  }                                                       \
}

#define ECU_CONFIG_CAN_IF_CB_DEFINE(interface)    \
    ECU_CONFIG_CAN_2ARG_CB(interface, rx_fifo0);  \
ECU_CONFIG_CAN_2ARG_CB(interface, rx_fifo1);      \
ECU_CONFIG_CAN_1ARG_CB(interface, rx_buffer);     \
ECU_CONFIG_CAN_1ARG_CB(interface, error);         \

#define ECU_CONFIG_CAN_IF_CB_ASSIGN(can_cfg, interface)               \
can_cfg->rx_fifo0_cb    = ecu_config_can_##interface##_rx_fifo0_cb;   \
can_cfg->rx_fifo1_cb    = ecu_config_can_##interface##_rx_fifo1_cb;   \
can_cfg->rx_buffer_cb   = ecu_config_can_##interface##_rx_buffer_cb;  \
can_cfg->error_cb       = ecu_config_can_##interface##_error_cb;      \

ECU_CONFIG_CAN_IF_CB_DEFINE(ECU_CAN_IF_MAIN);

#else /* USE_HAL_FDCAN_REGISTER_CALLBACKS */

#define ECU_CONFIG_CAN_CB(hal_op, can_op)  \
INLINE ITCM_FUNC void HAL_FDCAN_##hal_op##Callback(CAN_HANDLE_TYPE *hcan)   \
{                                                                 \
  can_t *can = NULL;                                              \
  error_t err;                                                    \
  for(int i = 0; i < ECU_CAN_IF_MAX; i++) {                       \
    err = middlelayer_can_get_if(&can, i);                        \
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

error_t ecu_config_can_init(void)
{
  error_t err = E_OK;

  for(int i = 0; i < ECU_CAN_IF_MAX; i++) {

    BREAK_IF(err != E_OK);
  }

  return err;
}

error_t ecu_config_can_register_callbacks(can_cfg_t *can_cfg, ecu_can_if_enum_t interface)
{
  error_t err = E_OK;

  if(can_cfg != NULL) {
#if (USE_HAL_FDCAN_REGISTER_CALLBACKS == 1UL)
    switch(interface) {
      case ECU_CAN_IF_MAIN:
        ECU_CONFIG_CAN_IF_CB_ASSIGN(can_cfg, ECU_CAN_IF_MAIN);
        break;
      default:
        err = E_PARAM;
        break;
    }
#endif /* USE_HAL_CAN_REGISTER_CALLBACKS */
  } else {
    err = E_PARAM;
  }

  return err;
}

error_t ecu_config_can_get_if_cfg(can_cfg_t *can_cfg, ecu_can_if_enum_t interface)
{
  error_t err = E_OK;

  if(can_cfg != NULL && interface < ECU_CAN_IF_MAX) {
    memcpy(can_cfg, &ecu_config_can[interface], sizeof(can_cfg_t));
  } else {
    err = E_PARAM;
  }

  return err;
}
