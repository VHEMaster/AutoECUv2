/*
 * can.c
 *
 *  Created on: Oct 19, 2025
 *      Author: VHEMaster
 */

#include "can.h"

#define CAN_LBK_NORMAL(ctx) HAL_GPIO_WritePin((ctx)->config.lbk_pin.port, (ctx)->config.lbk_pin.pin, GPIO_PIN_RESET)
#define CAN_LBK_LOOPBACK(ctx) HAL_GPIO_WritePin((ctx)->config.lbk_pin.port, (ctx)->config.lbk_pin.pin, GPIO_PIN_SET)

error_t can_init(can_ctx_t *ctx, const can_cfg_t *config)
{
  error_t err = E_OK;
  HAL_StatusTypeDef status = HAL_OK;


  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(config == NULL, err = E_PARAM);
    BREAK_IF_ACTION(config->handle == NULL, err = E_PARAM);

    memset(ctx, 0, sizeof(can_ctx_t));
    memcpy(&ctx->config, config, sizeof(can_cfg_t));

    CAN_LBK_LOOPBACK(ctx);
    status = HAL_FDCAN_Stop(ctx->config.handle);

#if (USE_HAL_FDCAN_REGISTER_CALLBACKS == 1UL)
    status = HAL_OK;
    if(ctx->config.rx_fifo0_cb != NULL) {
      status |= HAL_FDCAN_RegisterRxFifo0Callback(ctx->config.handle, ctx->config.rx_fifo0_cb);
    } else {
      err = E_PARAM;
      break;
    }
    if(ctx->config.rx_fifo1_cb != NULL) {
      status |= HAL_FDCAN_RegisterRxFifo1Callback(ctx->config.handle, ctx->config.rx_fifo1_cb);
    } else {
      err = E_PARAM;
      break;
    }
    if(ctx->config.rx_buffer_cb != NULL) {
      status |= HAL_FDCAN_RegisterCallback(ctx->config.handle, HAL_FDCAN_RX_BUFFER_NEW_MSG_CB_ID, ctx->config.rx_buffer_cb);
    } else {
      err = E_PARAM;
      break;
    }
    if(ctx->config.error_cb != NULL) {
      status |= HAL_FDCAN_RegisterCallback(ctx->config.handle, HAL_FDCAN_ERROR_CALLBACK_CB_ID, ctx->config.error_cb);
    } else {
      err = E_PARAM;
      break;
    }

    if(status != HAL_OK) {
      err = E_HAL;
      break;
    }
#endif /* USE_HAL_FDCAN_REGISTER_CALLBACKS */

    status = HAL_FDCAN_ConfigGlobalFilter(ctx->config.handle,
        ctx->config.global_filter.reject_non_matching_std ? FDCAN_REJECT : ctx->config.global_filter.reject_non_matching_std,
        ctx->config.global_filter.reject_non_matching_ext ? FDCAN_REJECT : ctx->config.global_filter.reject_non_matching_ext,
        ctx->config.global_filter.reject_remote_std ? FDCAN_REJECT_REMOTE : FDCAN_FILTER_REMOTE,
        ctx->config.global_filter.reject_remote_ext ? FDCAN_REJECT_REMOTE : FDCAN_FILTER_REMOTE);
    BREAK_IF_ACTION(status != HAL_OK, err = E_HAL);

    for(int i = 0; i < CAN_RX_FILTER_MAX; i++) {
      if(ctx->config.filter_config[i].FilterConfig != FDCAN_FILTER_DISABLE) {
        status = HAL_FDCAN_ConfigFilter(ctx->config.handle, &ctx->config.filter_config[i]);
        BREAK_IF_ACTION(status != HAL_OK, err = E_HAL);
      }
    }
    BREAK_IF(err != E_OK);

    status = HAL_FDCAN_ActivateNotification(ctx->config.handle,
        FDCAN_IT_RX_BUFFER_NEW_MESSAGE, 0);
    BREAK_IF_ACTION(status != HAL_OK, err = E_HAL);

    status = HAL_FDCAN_ActivateNotification(ctx->config.handle,
        FDCAN_IT_RX_FIFO0_MESSAGE_LOST |
        FDCAN_IT_RX_FIFO0_FULL |
        FDCAN_IT_RX_FIFO0_NEW_MESSAGE,
        0);
    BREAK_IF_ACTION(status != HAL_OK, err = E_HAL);

    status = HAL_FDCAN_ActivateNotification(ctx->config.handle,
        FDCAN_IT_RX_FIFO1_MESSAGE_LOST |
        FDCAN_IT_RX_FIFO1_FULL |
        FDCAN_IT_RX_FIFO1_NEW_MESSAGE,
        0);
    BREAK_IF_ACTION(status != HAL_OK, err = E_HAL);

    status = HAL_FDCAN_Start(ctx->config.handle);
    BREAK_IF_ACTION(status != HAL_OK, err = E_HAL);

    CAN_LBK_NORMAL(ctx);
    ctx->configured = true;


    ctx->txheader.Identifier = 0x7E8;
    ctx->txheader.IdType = FDCAN_STANDARD_ID;
    ctx->txheader.TxFrameType = FDCAN_DATA_FRAME;
    ctx->txheader.DataLength = FDCAN_DLC_BYTES_8;
    ctx->txheader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
    ctx->txheader.BitRateSwitch = FDCAN_BRS_OFF;
    ctx->txheader.FDFormat = FDCAN_CLASSIC_CAN;
    ctx->txheader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
    ctx->txheader.MessageMarker = 0;
    ctx->message.payload[0] = 0x06;
    ctx->message.payload[1] = 0x41;
    ctx->message.payload[2] = 0x00;

    ctx->message.payload[3] = 0xBE;
    ctx->message.payload[4] = 0x3F;
    ctx->message.payload[5] = 0xA8;
    ctx->message.payload[6] = 0x11;
    status = HAL_FDCAN_AddMessageToTxFifoQ(ctx->config.handle, &ctx->txheader, ctx->message.payload);
    if(status != HAL_OK) {
      printf("TX ERROR\r\n");
    }


  } while(0);

  return err;
}

error_t can_tx(can_ctx_t *ctx, const can_message_t *message)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(message == NULL, err = E_PARAM);

  } while(0);

  return err;
}

error_t can_rx(can_ctx_t *ctx, can_message_t *message)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(message == NULL, err = E_PARAM);

  } while(0);

  return err;
}

error_t can_register_rx_callback(can_ctx_t *ctx, uint32_t msg_id, can_rx_callback_func_t func, void *usrdata)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);

  } while(0);

  return err;
}

error_t can_unregister_rx_callback(can_ctx_t *ctx, uint32_t msg_id)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);

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

void can_loop_fast(can_ctx_t *ctx)
{
  do {
    BREAK_IF(ctx == NULL);

  } while(0);
}

#include <stdio.h>
#include "time.h"

void can_rx_fifo0_irq(can_ctx_t *ctx, uint32_t fifo_its)
{
  do {
    BREAK_IF(ctx == NULL);

    printf("RX FIFO 0 IRQ %lu\r\n", fifo_its);

  } while(0);
}

void can_rx_fifo1_irq(can_ctx_t *ctx, uint32_t fifo_its)
{
  do {
    BREAK_IF(ctx == NULL);

    printf("RX FIFO 1 IRQ %lu\r\n", fifo_its);

  } while(0);
}

void can_rx_buffer_irq(can_ctx_t *ctx)
{
  HAL_StatusTypeDef status;
  do {
    BREAK_IF(ctx == NULL);


    status = HAL_FDCAN_GetRxMessage(ctx->config.handle, FDCAN_RX_BUFFER0, &ctx->rxheader, ctx->message.payload);
    if(status == HAL_OK) {
      ctx->message.id = ctx->rxheader.Identifier;
      ctx->message.len = ctx->rxheader.DataLength >> 16u;
      if(ctx->rxheader.IdType == FDCAN_EXTENDED_ID) {
        ctx->message.id |= CAN_MESSAGE_EXTENDED_ID_FLAG;
      }

      if(ctx->message.payload[1] == 0x01 && ctx->message.payload[2] == 0x00) {
        ctx->txheader.Identifier = 0x7E8;
        ctx->txheader.IdType = FDCAN_STANDARD_ID;
        ctx->txheader.TxFrameType = FDCAN_DATA_FRAME;
        ctx->txheader.DataLength = FDCAN_DLC_BYTES_8;
        ctx->txheader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
        ctx->txheader.BitRateSwitch = FDCAN_BRS_OFF;
        ctx->txheader.FDFormat = FDCAN_CLASSIC_CAN;
        ctx->txheader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
        ctx->txheader.MessageMarker = 0;
        ctx->message.payload[0] = 0x06;
        ctx->message.payload[1] = 0x41;
        ctx->message.payload[2] = 0x00;

        ctx->message.payload[3] = 0xBE;
        ctx->message.payload[4] = 0x3F;
        ctx->message.payload[5] = 0xA8;
        ctx->message.payload[6] = 0x11;
        status = HAL_FDCAN_AddMessageToTxBuffer(ctx->config.handle, &ctx->txheader, ctx->message.payload, FDCAN_TX_BUFFER0);
        if(status != HAL_OK) {
          printf("TX ERROR\r\n");
        }

      }

      for(int i = 0; i < CAN_RX_CB_MAX; i++) {
        if(ctx->rx_callbacks[i].enabled) {
          if(ctx->rx_callbacks[i].msg_id == ctx->message.id) {
            if(ctx->rx_callbacks[i].func != NULL) {
              ctx->rx_callbacks[i].func(ctx, &ctx->message, ctx->rx_callbacks[i].usrdata);
            }
          }
        } else {
          break;
        }
      }
    } else {
      printf("RX BUFFER ERROR");
    }

  } while(0);
}

void can_error_irq(can_ctx_t *ctx)
{
  do {
    BREAK_IF(ctx == NULL);

    printf("ERROR IRQ\r\n");

  } while(0);
}

