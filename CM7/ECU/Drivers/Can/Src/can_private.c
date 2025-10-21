/*
 * can_private.c
 *
 *  Created on: Oct 22, 2025
 *      Author: VHEMaster
 */

#include "can_private.h"

error_t can_rx_get_msg(can_ctx_t *ctx, uint32_t rxindex)
{
  error_t err = E_OK;
  HAL_StatusTypeDef status;
  uint32_t read, write, pos;
  can_rx_cb_t *cb;
  bool handled = false;

  status = HAL_FDCAN_GetRxMessage(ctx->config.handle, rxindex, &ctx->rxheader, ctx->rxmessage.payload);
  if(status == HAL_OK) {
    ctx->rxmessage.id = ctx->rxheader.Identifier;
    ctx->rxmessage.len = ctx->rxheader.DataLength >> 16u;
    if(ctx->rxheader.IdType == FDCAN_EXTENDED_ID) {
      ctx->rxmessage.id |= CAN_MESSAGE_EXTENDED_ID_FLAG;
    }

    for(int i = 0; i < CAN_RX_CB_MAX; i++) {
      cb = &ctx->rx_callbacks[i];
      if(cb->enabled) {
        if(cb->msg_id == ctx->rxmessage.id) {
          if(cb->func != NULL) {
            cb->func(ctx, &ctx->rxmessage, cb->usrdata);
          }
          handled = true;
        }
      } else {
        break;
      }
    }

    if(!handled) {
      read = ctx->rxfifo.read;
      write = ctx->rxfifo.write;
      pos = write;

      if(++write >= CAN_RX_FIFO_SIZE) {
        write = 0;
      }
      if(write != read) {
        memcpy(&ctx->rxfifo.buffer[pos], &ctx->rxmessage, sizeof(can_message_t));
        ctx->rxfifo.write = write;
      } else {
        // CAN RX FIFO FULL
        // TODO: DTC here
      }
    }
  } else {
    err = E_HAL;
    // TODO: DTC here
  }

  return err;
}

error_t can_tx_send_msg(can_ctx_t *ctx, const can_message_t *message)
{
  error_t err = E_OK;
  HAL_StatusTypeDef status;
  uint32_t free;
  uint32_t message_id;

  do {
    free = HAL_FDCAN_GetTxFifoFreeLevel(ctx->config.handle);
    BREAK_IF_ACTION(free == 0, err = E_AGAIN);

    message_id = message->id;

    BREAK_IF_ACTION((message_id & CAN_MESSAGE_EXTENDED_ID_FLAG) != 0 && message_id > (0x1FFFFFFF | CAN_MESSAGE_EXTENDED_ID_FLAG), err = E_PARAM);
    BREAK_IF_ACTION((message_id & CAN_MESSAGE_EXTENDED_ID_FLAG) == 0 && message_id > 0x7FF, err = E_PARAM);

    ctx->txheader.Identifier = message_id & CAN_MESSAGE_EXTENDED_ID_FLAG;
    if((message_id & CAN_MESSAGE_EXTENDED_ID_FLAG)) {
      ctx->txheader.IdType = FDCAN_EXTENDED_ID;
    } else {
      ctx->txheader.IdType = FDCAN_STANDARD_ID;
    }
    ctx->txheader.TxFrameType = FDCAN_DATA_FRAME;
    ctx->txheader.DataLength = message->len << 16u;
    ctx->txheader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
    ctx->txheader.BitRateSwitch = FDCAN_BRS_OFF;
    ctx->txheader.FDFormat = FDCAN_CLASSIC_CAN;
    ctx->txheader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
    ctx->txheader.MessageMarker = 0;

    status = HAL_FDCAN_AddMessageToTxFifoQ(ctx->config.handle, &ctx->txheader, (uint8_t *)message->payload);
    if(status != HAL_OK) {
      err = E_HAL;
      // TODO: DTC here
    }
  } while(0);

  return err;
}
