/*
 * can.c
 *
 *  Created on: Oct 19, 2025
 *      Author: VHEMaster
 */

#include "can.h"
#include "can_private.h"

static const struct {
    uint32_t prescaler;
    uint32_t timeseg1;
    uint32_t timeseg2;
    uint32_t tdco;
  } can_baudrate_map[CAN_BAUDRATE_MAX] = {
    [CAN_BAUDRATE_10MBPS] = { .prescaler = 1, .timeseg1 = 4, .timeseg2 = 3, .tdco = 4 },
    [CAN_BAUDRATE_8MBPS]  = { .prescaler = 2, .timeseg1 = 2, .timeseg2 = 2, .tdco = 4 },
    [CAN_BAUDRATE_5MBPS]  = { .prescaler = 2, .timeseg1 = 4, .timeseg2 = 3, .tdco = 8 },
    [CAN_BAUDRATE_4MBPS]  = { .prescaler = 2, .timeseg1 = 6, .timeseg2 = 3, .tdco = 12 },
    [CAN_BAUDRATE_2MBPS]  = { .prescaler = 4, .timeseg1 = 6, .timeseg2 = 3, .tdco = 0 },
    [CAN_BAUDRATE_1MBPS]  = { .prescaler = 8, .timeseg1 = 6, .timeseg2 = 3, .tdco = 0 },
    [CAN_BAUDRATE_500KBPS] = { .prescaler = 16, .timeseg1 = 6, .timeseg2 = 3, .tdco = 0 },
    [CAN_BAUDRATE_250KBPS] = { .prescaler = 32, .timeseg1 = 6, .timeseg2 = 3, .tdco = 0 },
    [CAN_BAUDRATE_125KBPS] = { .prescaler = 32, .timeseg1 = 12, .timeseg2 = 7, .tdco = 0 },
};

#define CAN_LBK_NORMAL(ctx) HAL_GPIO_WritePin((ctx)->config.lbk_pin.port, (ctx)->config.lbk_pin.pin, GPIO_PIN_RESET)
#define CAN_LBK_LOOPBACK(ctx) HAL_GPIO_WritePin((ctx)->config.lbk_pin.port, (ctx)->config.lbk_pin.pin, GPIO_PIN_SET)

error_t can_init(can_ctx_t *ctx, const can_init_ctx_t *init_ctx)
{
  error_t err = E_OK;
  HAL_StatusTypeDef status = HAL_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(init_ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(init_ctx->handle == NULL, err = E_PARAM);

    memset(ctx, 0u, sizeof(can_ctx_t));
    memcpy(&ctx->init, init_ctx, sizeof(can_init_ctx_t));

    if(gpio_valid(&ctx->init.lbk_pin)) {
      gpio_set(&ctx->init.lbk_pin); // Loopback mode
    }

    (void)HAL_FDCAN_Stop(ctx->init.handle);

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

    ctx->init.handle->Init.NominalPrescaler = can_baudrate_map[CAN_BAUDRATE_500KBPS].prescaler;
    ctx->init.handle->Init.NominalTimeSeg1 = can_baudrate_map[CAN_BAUDRATE_500KBPS].timeseg1;
    ctx->init.handle->Init.NominalTimeSeg2 = can_baudrate_map[CAN_BAUDRATE_500KBPS].timeseg2;

    ctx->init.handle->Init.DataPrescaler = can_baudrate_map[CAN_BAUDRATE_500KBPS].prescaler;
    ctx->init.handle->Init.DataTimeSeg1 = can_baudrate_map[CAN_BAUDRATE_500KBPS].timeseg1;
    ctx->init.handle->Init.DataTimeSeg2 = can_baudrate_map[CAN_BAUDRATE_500KBPS].timeseg2;

    if(status != HAL_OK) {
      err = E_HAL;
      break;
    }
#endif /* USE_HAL_FDCAN_REGISTER_CALLBACKS */

    ctx->initialized = true;

  } while(0);

  return err;
}

error_t can_configure(can_ctx_t *ctx, const can_config_t *config)
{
  error_t err = E_OK;
  HAL_StatusTypeDef status = HAL_OK;
  uint32_t tdco;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(!ctx->initialized, err = E_INVALACT);
    BREAK_IF_ACTION(config == NULL, err = E_PARAM);
    BREAK_IF_ACTION(config->baudrate >= CAN_BAUDRATE_MAX, err = E_PARAM);
    BREAK_IF_ACTION(config->baudrate_brs >= CAN_BAUDRATE_MAX, err = E_PARAM);
    BREAK_IF_ACTION(config->fdcan_supported == true &&
        config->baudrate > config->baudrate_brs, err = E_PARAM);

    memset(ctx->rx_callbacks, 0, sizeof(ctx->rx_callbacks));
    memset(ctx->err_callbacks, 0, sizeof(ctx->err_callbacks));

    if(&ctx->config != config) {
      memcpy(&ctx->config, config, sizeof(can_config_t));
    }

    if(gpio_valid(&ctx->init.lbk_pin)) {
      gpio_set(&ctx->init.lbk_pin); // Loopback mode
    }

    (void)HAL_FDCAN_Stop(ctx->init.handle);

    ctx->init.handle->Init.NominalPrescaler = can_baudrate_map[ctx->config.baudrate].prescaler;
    ctx->init.handle->Init.NominalTimeSeg1 = can_baudrate_map[ctx->config.baudrate].timeseg1;
    ctx->init.handle->Init.NominalTimeSeg2 = can_baudrate_map[ctx->config.baudrate].timeseg2;

    if(config->fdcan_supported) {
      ctx->init.handle->Init.FrameFormat = FDCAN_FRAME_FD_BRS;

      ctx->init.handle->Init.DataPrescaler = can_baudrate_map[ctx->config.baudrate_brs].prescaler;
      ctx->init.handle->Init.DataTimeSeg1 = can_baudrate_map[ctx->config.baudrate_brs].timeseg1;
      ctx->init.handle->Init.DataTimeSeg2 = can_baudrate_map[ctx->config.baudrate_brs].timeseg2;
      tdco = can_baudrate_map[ctx->config.baudrate_brs].tdco;

      ctx->init.handle->Init.StdFiltersNbr = 16;
      ctx->init.handle->Init.ExtFiltersNbr = 16;
      ctx->init.handle->Init.RxFifo0ElmtsNbr = 56;
      ctx->init.handle->Init.RxFifo0ElmtSize = FDCAN_DATA_BYTES_64;
      ctx->init.handle->Init.RxFifo1ElmtsNbr = 32;
      ctx->init.handle->Init.RxFifo1ElmtSize = FDCAN_DATA_BYTES_64;
      ctx->init.handle->Init.RxBuffersNbr = 16;
      ctx->init.handle->Init.RxBufferSize = FDCAN_DATA_BYTES_64;
      ctx->init.handle->Init.TxFifoQueueElmtsNbr = 32;
      ctx->init.handle->Init.TxElmtSize = FDCAN_DATA_BYTES_64;
      ctx->init.handle->Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
      ctx->init.handle->Init.TxEventsNbr = 0;
      ctx->init.handle->Init.TxBuffersNbr = 0;
    } else {
      ctx->init.handle->Init.FrameFormat = FDCAN_FRAME_CLASSIC;

      ctx->init.handle->Init.DataPrescaler = can_baudrate_map[ctx->config.baudrate].prescaler;
      ctx->init.handle->Init.DataTimeSeg1 = can_baudrate_map[ctx->config.baudrate].timeseg1;
      ctx->init.handle->Init.DataTimeSeg2 = can_baudrate_map[ctx->config.baudrate].timeseg2;
      tdco = can_baudrate_map[ctx->config.baudrate].tdco;

      ctx->init.handle->Init.StdFiltersNbr = 16;
      ctx->init.handle->Init.ExtFiltersNbr = 16;
      ctx->init.handle->Init.RxFifo0ElmtsNbr = 64;
      ctx->init.handle->Init.RxFifo0ElmtSize = FDCAN_DATA_BYTES_8;
      ctx->init.handle->Init.RxFifo1ElmtsNbr = 64;
      ctx->init.handle->Init.RxFifo1ElmtSize = FDCAN_DATA_BYTES_8;
      ctx->init.handle->Init.RxBuffersNbr = 16;
      ctx->init.handle->Init.RxBufferSize = FDCAN_DATA_BYTES_8;
      ctx->init.handle->Init.TxFifoQueueElmtsNbr = 32;
      ctx->init.handle->Init.TxElmtSize = FDCAN_DATA_BYTES_8;
      ctx->init.handle->Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
      ctx->init.handle->Init.TxEventsNbr = 0;
      ctx->init.handle->Init.TxBuffersNbr = 0;
    }

    status = HAL_FDCAN_Init(ctx->init.handle);
    BREAK_IF_ACTION(status != HAL_OK, err = E_HAL);

    if(tdco > 0u) {
      status = HAL_FDCAN_ConfigTxDelayCompensation(ctx->init.handle, tdco, 0u);
      BREAK_IF_ACTION(status != HAL_OK, err = E_HAL);

      status = HAL_FDCAN_EnableTxDelayCompensation(ctx->init.handle);
      BREAK_IF_ACTION(status != HAL_OK, err = E_HAL);
    } else {
      status = HAL_FDCAN_DisableTxDelayCompensation(ctx->init.handle);
      BREAK_IF_ACTION(status != HAL_OK, err = E_HAL);
    }

    status = HAL_FDCAN_ConfigGlobalFilter(ctx->init.handle,
        ctx->config.global_filter.reject_non_matching_std ? FDCAN_REJECT : ctx->config.global_filter.reject_non_matching_std,
        ctx->config.global_filter.reject_non_matching_ext ? FDCAN_REJECT : ctx->config.global_filter.reject_non_matching_ext,
        ctx->config.global_filter.reject_remote_std ? FDCAN_REJECT_REMOTE : FDCAN_FILTER_REMOTE,
        ctx->config.global_filter.reject_remote_ext ? FDCAN_REJECT_REMOTE : FDCAN_FILTER_REMOTE);
    BREAK_IF_ACTION(status != HAL_OK, err = E_HAL);

    for(int i = 0; i < CAN_RX_FILTER_MAX; i++) {
      if(ctx->config.filter_config[i].filter_config != CAN_FILTER_CONFIG_DISABLE) {

        switch(ctx->config.filter_config[i].id_type) {
          case CAN_FILTER_IDTYPE_STD:
            ctx->filter.IdType = FDCAN_STANDARD_ID;
            break;
          case CAN_FILTER_IDTYPE_EXT:
            ctx->filter.IdType = FDCAN_EXTENDED_ID;
            break;
          default:
            err = E_PARAM;
            break;
        }
        BREAK_IF(err != E_OK);

        switch(ctx->config.filter_config[i].filter_type) {
          case CAN_FILTER_TYPE_RANGE:
            ctx->filter.FilterType = FDCAN_FILTER_RANGE;
            break;
          case CAN_FILTER_TYPE_DUAL:
            ctx->filter.FilterType = FDCAN_FILTER_DUAL;
            break;
          case CAN_FILTER_TYPE_MASK:
            ctx->filter.FilterType = FDCAN_FILTER_MASK;
            break;
          default:
            err = E_PARAM;
            break;
        }
        BREAK_IF(err != E_OK);

        switch(ctx->config.filter_config[i].filter_config) {
          case CAN_FILTER_CONFIG_TO_RXFIFO0:
            ctx->filter.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
            break;
          case CAN_FILTER_CONFIG_TO_RXFIFO1:
            ctx->filter.FilterConfig = FDCAN_FILTER_TO_RXFIFO1;
            break;
          case CAN_FILTER_CONFIG_REJECT:
            ctx->filter.FilterConfig = FDCAN_FILTER_REJECT;
            break;
          case CAN_FILTER_CONFIG_HP:
            ctx->filter.FilterConfig = FDCAN_FILTER_HP;
            break;
          case CAN_FILTER_CONFIG_TO_RXFIFO0_HP:
            ctx->filter.FilterConfig = FDCAN_FILTER_TO_RXFIFO0_HP;
            break;
          case CAN_FILTER_CONFIG_TO_RXFIFO1_HP:
            ctx->filter.FilterConfig = FDCAN_FILTER_TO_RXFIFO1_HP;
            break;
          case CAN_FILTER_CONFIG_TO_RXBUFFER:
            ctx->filter.FilterConfig = FDCAN_FILTER_TO_RXBUFFER;
            break;
          default:
            err = E_PARAM;
            break;
        }
        BREAK_IF(err != E_OK);

        ctx->filter.FilterIndex = i;
        ctx->filter.RxBufferIndex = ctx->config.filter_config[i].rx_buffer_index;
        ctx->filter.FilterID1 = ctx->config.filter_config[i].id1_filter;
        ctx->filter.FilterID2 = ctx->config.filter_config[i].id2_mask;

        status = HAL_FDCAN_ConfigFilter(ctx->init.handle, &ctx->filter);
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
    BREAK_IF_ACTION(ctx->configured == false, err = E_INVALACT);
    BREAK_IF_ACTION(message == NULL, err = E_PARAM);

    err = can_private_tx_send_msg(ctx, message);

  } while(0);

  return err;
}

error_t can_rx(can_ctx_t *ctx, can_message_t *message)
{
  error_t err = E_OK;
  uint32_t read, write;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->configured == false, err = E_INVALACT);
    BREAK_IF_ACTION(message == NULL, err = E_PARAM);

    read = ctx->rxfifo.read;
    write = ctx->rxfifo.write;
    BREAK_IF_ACTION(read == write, err = E_AGAIN);

    *message = ctx->rxfifo.buffer[read];

    if(++read >= CAN_RX_FIFO_SIZE) {
      read = 0;
    }
    ctx->rxfifo.read = read;

  } while(0);

  return err;
}

error_t can_unregister_rx_callback(can_ctx_t *ctx, can_rx_callback_func_t func, void *usrdata)
{
  error_t err = E_OK;
  can_rx_cb_t *cb;
  can_rx_cb_t *cb_l;
  can_rx_cb_t *cb_r;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->configured == false, err = E_INVALACT);
    BREAK_IF_ACTION(func == NULL, err = E_PARAM);

    for(uint32_t i = 0; i < CAN_RX_CB_MAX;) {
      cb = &ctx->rx_callbacks[i];
      if(cb->enabled) {
        if(cb->func == func && cb->usrdata == usrdata) {
          if(i + 1 < CAN_RX_CB_MAX) {
            for(uint32_t j = i; j < CAN_RX_CB_MAX - 1; j++) {
              cb_l = &ctx->rx_callbacks[j];
              cb_r = &ctx->rx_callbacks[j + 1];

              if(cb_r->enabled) {
                cb_l->enabled = false;
                cb_r->enabled = false;
                *cb_l = *cb_r;
                cb_l->enabled = true;
              } else {
                cb_l->enabled = false;
                break;
              }
            }
          } else {
            cb->enabled = false;
          }
        } else {
          i++;
        }
      } else {
        break;
      }
    }

  } while(0);

  return err;
}

error_t can_unregister_err_callback(can_ctx_t *ctx, can_err_callback_func_t func, void *usrdata)
{
  error_t err = E_OK;
  can_err_cb_t *cb;
  can_err_cb_t *cb_l;
  can_err_cb_t *cb_r;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->configured == false, err = E_INVALACT);
    BREAK_IF_ACTION(func == NULL, err = E_PARAM);

    for(uint32_t i = 0; i < CAN_ERR_CB_MAX;) {
      cb = &ctx->err_callbacks[i];
      if(cb->enabled) {
        if(cb->func == func && cb->usrdata == usrdata) {
          if(i + 1 < CAN_ERR_CB_MAX) {
            for(uint32_t j = i; j < CAN_ERR_CB_MAX - 1; j++) {
              cb_l = &ctx->err_callbacks[j];
              cb_r = &ctx->err_callbacks[j + 1];

              if(cb_r->enabled) {
                cb_l->enabled = false;
                cb_r->enabled = false;
                *cb_l = *cb_r;
                cb_l->enabled = true;
              } else {
                cb_l->enabled = false;
                break;
              }
            }
          } else {
            cb->enabled = false;
          }
        } else {
          i++;
        }
      } else {
        break;
      }
    }

  } while(0);

  return err;
}

error_t can_register_rx_callback(can_ctx_t *ctx, uint32_t msg_id_start, uint32_t msg_id_end, can_rx_callback_func_t func, void *usrdata)
{
  error_t err = E_OK;
  can_rx_cb_t *cb;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->configured == false, err = E_INVALACT);
    BREAK_IF_ACTION(func == NULL, err = E_PARAM);
    BREAK_IF_ACTION((msg_id_start & CAN_MESSAGE_EXTENDED_ID_FLAG) != (msg_id_end & CAN_MESSAGE_EXTENDED_ID_FLAG), err = E_PARAM);
    BREAK_IF_ACTION((msg_id_start & CAN_MESSAGE_EXTENDED_ID_FLAG) != 0 && msg_id_start > (0x1FFFFFFF | CAN_MESSAGE_EXTENDED_ID_FLAG), err = E_PARAM);
    BREAK_IF_ACTION((msg_id_start & CAN_MESSAGE_EXTENDED_ID_FLAG) == 0 && msg_id_start > 0x7FF, err = E_PARAM);
    BREAK_IF_ACTION((msg_id_end & CAN_MESSAGE_EXTENDED_ID_FLAG) != 0 && msg_id_end > (0x1FFFFFFF | CAN_MESSAGE_EXTENDED_ID_FLAG), err = E_PARAM);
    BREAK_IF_ACTION((msg_id_end & CAN_MESSAGE_EXTENDED_ID_FLAG) == 0 && msg_id_end > 0x7FF, err = E_PARAM);


    err = E_OVERFLOW;
    for(int i = 0; i < CAN_RX_CB_MAX; i++) {
      cb = &ctx->rx_callbacks[i];
      if(!cb->enabled) {
        cb->msg_id_start = msg_id_start;
        cb->msg_id_end = msg_id_end;
        cb->func = func;
        cb->usrdata = usrdata;
        cb->enabled = true;
        err = E_OK;
        break;
      } else {
        if(cb->msg_id_start == msg_id_start && cb->msg_id_end == msg_id_end && cb->func == func && cb->usrdata == usrdata) {
          err = E_EXISTS;
          break;
        }
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
    BREAK_IF_ACTION(ctx->configured == false, err = E_INVALACT);
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
      } else {
        if(cb->func == func && cb->usrdata == usrdata) {
          err = E_EXISTS;
          break;
        }
      }
    }

  } while(0);

  return err;
}

void can_loop_main(can_ctx_t *ctx)
{
  do {
    BREAK_IF(ctx == NULL);
    BREAK_IF(ctx->configured == false);

  } while(0);
}

void can_loop_slow(can_ctx_t *ctx)
{
  do {
    BREAK_IF(ctx == NULL);
    BREAK_IF(ctx->configured == false);

  } while(0);
}

void can_loop_comm(can_ctx_t *ctx)
{
  do {
    BREAK_IF(ctx == NULL);
    BREAK_IF(ctx->configured == false);

  } while(0);
}

void can_rx_fifo0_irq(can_ctx_t *ctx, uint32_t fifo_its)
{
  error_t err;
  uint32_t fill, fifo;

  do {
    BREAK_IF(ctx == NULL);
    BREAK_IF(ctx->configured == false);

    fifo = FDCAN_RX_FIFO0;
    fill = HAL_FDCAN_GetRxFifoFillLevel(ctx->init.handle, fifo);
    while(fill--) {
      err = can_private_rx_get_msg(ctx, fifo);
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
    BREAK_IF(ctx->configured == false);

    fifo = FDCAN_RX_FIFO1;
    fill = HAL_FDCAN_GetRxFifoFillLevel(ctx->init.handle, fifo);
    while(fill--) {
      err = can_private_rx_get_msg(ctx, fifo);
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
    BREAK_IF(ctx->configured == false);

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

      err = can_private_rx_get_msg(ctx, buffer_index);
      BREAK_IF(err != E_OK);

      bitmask = 1u << buffer_index_rel;
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
    BREAK_IF(ctx->configured == false);

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

error_t can_reset(can_ctx_t *ctx)
{
  error_t err = E_OK;
  HAL_StatusTypeDef status = HAL_OK;

  do {
    BREAK_IF(ctx == NULL);
    BREAK_IF_ACTION(ctx->configured == false, err = E_INVALACT);

    if(gpio_valid(&ctx->init.lbk_pin)) {
      gpio_set(&ctx->init.lbk_pin); // Loopback mode
    }

    (void)HAL_FDCAN_Stop(ctx->init.handle);

    __HAL_FDCAN_CLEAR_FLAG(ctx->init.handle, FDCAN_FLAG_BUS_OFF);
    __HAL_FDCAN_CLEAR_FLAG(ctx->init.handle, FDCAN_FLAG_ERROR_PASSIVE | FDCAN_FLAG_ERROR_WARNING);
    __HAL_FDCAN_CLEAR_FLAG(ctx->init.handle, FDCAN_FLAG_RX_FIFO0_NEW_MESSAGE | FDCAN_FLAG_RX_FIFO1_NEW_MESSAGE);

    ctx->init.handle->Instance->TXBCR = ~0;
    ctx->init.handle->Instance->NDAT1 = ~0;
    ctx->init.handle->Instance->NDAT2 = ~0;

    status = HAL_FDCAN_Start(ctx->init.handle);
    if(status != HAL_OK) {
      err = can_configure(ctx, &ctx->config);
      ctx->configured = false;
      break;
    }

    if(gpio_valid(&ctx->init.lbk_pin)) {
      gpio_reset(&ctx->init.lbk_pin); // Normal mode
    }

  } while(0);

  return err;
}

error_t can_validate_message_length(can_frame_format_t format, uint32_t length)
{
  return can_private_validate_message_length(format, length);
}

