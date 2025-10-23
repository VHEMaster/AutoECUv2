/*
 * can.c
 *
 *  Created on: Oct 19, 2025
 *      Author: VHEMaster
 */

#include "can.h"
#include "can_private.h"

#define CAN_LBK_NORMAL(ctx) HAL_GPIO_WritePin((ctx)->config.lbk_pin.port, (ctx)->config.lbk_pin.pin, GPIO_PIN_RESET)
#define CAN_LBK_LOOPBACK(ctx) HAL_GPIO_WritePin((ctx)->config.lbk_pin.port, (ctx)->config.lbk_pin.pin, GPIO_PIN_SET)

error_t can_init(can_ctx_t *ctx, const can_init_ctx_t *init_ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(init_ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(init_ctx->handle == NULL, err = E_PARAM);

    memset(ctx, 0u, sizeof(can_ctx_t));
    memcpy(&ctx->init, init_ctx, sizeof(can_init_ctx_t));

    if(gpio_valid(&ctx->init.lbk_pin)) {
      gpio_set(&ctx->init.lbk_pin); // Loopback mode
    }

    ctx->initialized = true;

  } while(0);

  return err;
}

error_t can_configure(can_ctx_t *ctx, const can_cfg_t *config)
{
  error_t err = E_OK;
  HAL_StatusTypeDef status = HAL_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(config == NULL, err = E_PARAM);
    BREAK_IF_ACTION(config->baudrate < CAN_BAUDRATE_MAX, err = E_PARAM);

    memcpy(&ctx->config, config, sizeof(can_cfg_t));

    if(gpio_valid(&ctx->init.lbk_pin)) {
      gpio_set(&ctx->init.lbk_pin); // Loopback mode
    }

    status = HAL_FDCAN_Stop(ctx->init.handle);

    switch(ctx->config.baudrate) {
      case CAN_BAUDRATE_1MBPS:
        ctx->init.handle->Init.NominalPrescaler = 8;
        break;
      case CAN_BAUDRATE_500KBPS:
        ctx->init.handle->Init.NominalPrescaler = 16;
        break;
      case CAN_BAUDRATE_250KBPS:
        ctx->init.handle->Init.NominalPrescaler = 32;
        break;
      case CAN_BAUDRATE_125KBPS:
        ctx->init.handle->Init.NominalPrescaler = 64;
        break;
      default:
        err = E_PARAM;
        break;
    }
    ctx->init.handle->Init.NominalTimeSeg1 = 6;
    ctx->init.handle->Init.NominalTimeSeg2 = 3;
    ctx->init.handle->Init.DataTimeSeg1 = 6;
    ctx->init.handle->Init.DataTimeSeg2 = 3;
    status = HAL_FDCAN_Init(ctx->init.handle);
    BREAK_IF_ACTION(status != HAL_OK, err = E_HAL);


#if (USE_HAL_FDCAN_REGISTER_CALLBACKS == 1UL)
    status = HAL_OK;
    if(ctx->init.rx_fifo0_cb != NULL) {
      status |= HAL_FDCAN_RegisterRxFifo0Callback(ctx->init.handle, ctx->init.rx_fifo0_cb);
    } else {
      err = E_PARAM;
      break;
    }
    if(ctx->init.rx_fifo1_cb != NULL) {
      status |= HAL_FDCAN_RegisterRxFifo1Callback(ctx->init.handle, ctx->init.rx_fifo1_cb);
    } else {
      err = E_PARAM;
      break;
    }
    if(ctx->init.rx_buffer_cb != NULL) {
      status |= HAL_FDCAN_RegisterCallback(ctx->init.handle, HAL_FDCAN_RX_BUFFER_NEW_MSG_CB_ID, ctx->init.rx_buffer_cb);
    } else {
      err = E_PARAM;
      break;
    }
    if(ctx->init.error_cb != NULL) {
      status |= HAL_FDCAN_RegisterCallback(ctx->init.handle, HAL_FDCAN_ERROR_CALLBACK_CB_ID, ctx->init.error_cb);
    } else {
      err = E_PARAM;
      break;
    }

    if(status != HAL_OK) {
      err = E_HAL;
      break;
    }
#endif /* USE_HAL_FDCAN_REGISTER_CALLBACKS */

    status = HAL_FDCAN_ConfigGlobalFilter(ctx->init.handle,
        ctx->config.global_filter.reject_non_matching_std ? FDCAN_REJECT : ctx->config.global_filter.reject_non_matching_std,
        ctx->config.global_filter.reject_non_matching_ext ? FDCAN_REJECT : ctx->config.global_filter.reject_non_matching_ext,
        ctx->config.global_filter.reject_remote_std ? FDCAN_REJECT_REMOTE : FDCAN_FILTER_REMOTE,
        ctx->config.global_filter.reject_remote_ext ? FDCAN_REJECT_REMOTE : FDCAN_FILTER_REMOTE);
    BREAK_IF_ACTION(status != HAL_OK, err = E_HAL);

    for(int i = 0; i < CAN_RX_FILTER_MAX; i++) {
      if(ctx->config.filter_config[i].FilterConfig != FDCAN_FILTER_DISABLE) {
        status = HAL_FDCAN_ConfigFilter(ctx->init.handle, &ctx->config.filter_config[i]);
        BREAK_IF_ACTION(status != HAL_OK, err = E_HAL);
      }
    }
    BREAK_IF(err != E_OK);

    status = HAL_FDCAN_ActivateNotification(ctx->init.handle,
        FDCAN_IT_RX_BUFFER_NEW_MESSAGE, 0);
    BREAK_IF_ACTION(status != HAL_OK, err = E_HAL);

    status = HAL_FDCAN_ActivateNotification(ctx->init.handle,
        FDCAN_IT_RX_FIFO0_MESSAGE_LOST |
        FDCAN_IT_RX_FIFO0_FULL |
        FDCAN_IT_RX_FIFO0_NEW_MESSAGE,
        0);
    BREAK_IF_ACTION(status != HAL_OK, err = E_HAL);

    status = HAL_FDCAN_ActivateNotification(ctx->init.handle,
        FDCAN_IT_RX_FIFO1_MESSAGE_LOST |
        FDCAN_IT_RX_FIFO1_FULL |
        FDCAN_IT_RX_FIFO1_NEW_MESSAGE,
        0);
    BREAK_IF_ACTION(status != HAL_OK, err = E_HAL);

    status = HAL_FDCAN_EnableEdgeFiltering(ctx->init.handle);
    BREAK_IF_ACTION(status != HAL_OK, err = E_HAL);

    status = HAL_FDCAN_Start(ctx->init.handle);
    BREAK_IF_ACTION(status != HAL_OK, err = E_HAL);

    if(gpio_valid(&ctx->init.lbk_pin)) {
      gpio_reset(&ctx->init.lbk_pin); // Normal mode
    }
    ctx->configured = true;

  } while(0);

  return err;
}

error_t can_tx(can_ctx_t *ctx, const can_message_t *message)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(message == NULL, err = E_PARAM);

    err = can_tx_send_msg(ctx, message);

  } while(0);

  return err;
}

error_t can_rx(can_ctx_t *ctx, can_message_t *message)
{
  error_t err = E_OK;
  uint32_t read, write;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(message == NULL, err = E_PARAM);

    read = ctx->rxfifo.read;
    write = ctx->rxfifo.write;
    BREAK_IF_ACTION(read == write, err = E_AGAIN);

    memcpy(message, &ctx->rxfifo.buffer[read], sizeof(can_message_t));

    if(++read >= CAN_RX_FIFO_SIZE) {
      read = 0;
    }
    ctx->rxfifo.read = read;

  } while(0);

  return err;
}

error_t can_register_rx_callback(can_ctx_t *ctx, uint32_t msg_id, can_rx_callback_func_t func, void *usrdata)
{
  error_t err = E_OK;
  can_rx_cb_t *cb;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(func == NULL, err = E_PARAM);
    BREAK_IF_ACTION((msg_id & CAN_MESSAGE_EXTENDED_ID_FLAG) != 0 && msg_id > (0x1FFFFFFF | CAN_MESSAGE_EXTENDED_ID_FLAG), err = E_PARAM);
    BREAK_IF_ACTION((msg_id & CAN_MESSAGE_EXTENDED_ID_FLAG) == 0 && msg_id > 0x7FF, err = E_PARAM);

    err = E_OVERFLOW;
    for(int i = 0; i < CAN_RX_CB_MAX; i++) {
      cb = &ctx->rx_callbacks[i];
      if(!cb->enabled) {
        cb->msg_id = msg_id;
        cb->func = func;
        cb->usrdata = usrdata;
        cb->enabled = true;
        err = E_OK;
        break;
      }
    }

  } while(0);

  return err;
}

error_t can_register_err_callback(can_ctx_t *ctx, can_err_callback_func_t func, void *usrdata)
{
  error_t err = E_OK;
  can_err_cb_t *cb;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(func == NULL, err = E_PARAM);

    err = E_OVERFLOW;
    for(int i = 0; i < CAN_ERR_CB_MAX; i++) {
      cb = &ctx->err_callbacks[i];
      if(!cb->enabled) {
        cb->func = func;
        cb->usrdata = usrdata;
        cb->enabled = true;
        err = E_OK;
        break;
      }
    }

  } while(0);

  return err;
}

void can_loop_main(can_ctx_t *ctx)
{
  do {
    BREAK_IF(ctx == NULL);

  } while(0);
}

void can_loop_slow(can_ctx_t *ctx)
{
  do {
    BREAK_IF(ctx == NULL);

  } while(0);
}

void can_loop_comm(can_ctx_t *ctx)
{
  do {
    BREAK_IF(ctx == NULL);

  } while(0);
}

void can_rx_fifo0_irq(can_ctx_t *ctx, uint32_t fifo_its)
{
  error_t err;
  uint32_t fill, fifo;

  do {
    BREAK_IF(ctx == NULL);

    fifo = FDCAN_RX_FIFO0;
    fill = HAL_FDCAN_GetRxFifoFillLevel(ctx->init.handle, fifo);
    while(fill--) {
      err = can_rx_get_msg(ctx, fifo);
      BREAK_IF(err != E_OK);
    }

  } while(0);
}

void can_rx_fifo1_irq(can_ctx_t *ctx, uint32_t fifo_its)
{
  error_t err;
  uint32_t fill, fifo;

  do {
    BREAK_IF(ctx == NULL);

    fifo = FDCAN_RX_FIFO1;
    fill = HAL_FDCAN_GetRxFifoFillLevel(ctx->init.handle, fifo);
    while(fill--) {
      err = can_rx_get_msg(ctx, fifo);
      BREAK_IF(err != E_OK);
    }

  } while(0);
}

void can_rx_buffer_irq(can_ctx_t *ctx)
{
  error_t err;
  uint32_t buffers_l, buffers_h;
  uint32_t buffer_index, buffer_index_rel;
  uint32_t bitmask;

  do {
    BREAK_IF(ctx == NULL);

    buffers_l = ctx->init.handle->Instance->NDAT1;
    buffers_h = ctx->init.handle->Instance->NDAT2;

    while(true) {
      if(buffers_l) {
        buffer_index_rel = POSITION_VAL(buffers_l);
        buffer_index = buffer_index_rel;
      } else if(buffers_h) {
        buffer_index_rel = POSITION_VAL(buffers_h);
        buffer_index = buffer_index_rel + FDCAN_RX_BUFFER32;
      } else {
        break;
      }

      err = can_rx_get_msg(ctx, buffer_index);
      BREAK_IF(err != E_OK);

      bitmask = 1 << buffer_index_rel;
      if(buffers_l) {
        buffers_l &= ~bitmask;
        ctx->init.handle->Instance->NDAT1 = bitmask;
      } else {
        buffers_h &= ~bitmask;
        ctx->init.handle->Instance->NDAT2 = bitmask;
      }
    }

  } while(0);
}

void can_error_irq(can_ctx_t *ctx)
{
  can_err_cb_t *cb;

  do {
    BREAK_IF(ctx == NULL);

    for(int i = 0; i < CAN_ERR_CB_MAX; i++) {
      cb = &ctx->err_callbacks[i];
      if(cb->enabled) {
        if(cb->func != NULL) {
          cb->func(ctx, cb->usrdata);
        }
      }
    }
  } while(0);
}

