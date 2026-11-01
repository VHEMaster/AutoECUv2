/*
 * can_private.c
 *
 *  Created on: Oct 22, 2025
 *      Author: VHEMaster
 */

#include "can_private.h"

#define FDCAN_DLC_INVALID   (UINT_MAX)

static const uint32_t can_const_message_length_to_frame_length[CAN_MESSAGE_PAYLOAD_LEN_MAX + 1] = {
    [0]  = FDCAN_DLC_BYTES_0,
    [1]  = FDCAN_DLC_BYTES_1,
    [2]  = FDCAN_DLC_BYTES_2,
    [3]  = FDCAN_DLC_BYTES_3,
    [4]  = FDCAN_DLC_BYTES_4,
    [5]  = FDCAN_DLC_BYTES_5,
    [6]  = FDCAN_DLC_BYTES_6,
    [7]  = FDCAN_DLC_BYTES_7,
    [8]  = FDCAN_DLC_BYTES_8,
    [9]  = FDCAN_DLC_INVALID,
    [10] = FDCAN_DLC_INVALID,
    [11] = FDCAN_DLC_INVALID,
    [12] = FDCAN_DLC_BYTES_12,
    [13] = FDCAN_DLC_INVALID,
    [14] = FDCAN_DLC_INVALID,
    [15] = FDCAN_DLC_INVALID,
    [16] = FDCAN_DLC_BYTES_16,
    [17] = FDCAN_DLC_INVALID,
    [18] = FDCAN_DLC_INVALID,
    [19] = FDCAN_DLC_INVALID,
    [20] = FDCAN_DLC_BYTES_20,
    [21] = FDCAN_DLC_INVALID,
    [22] = FDCAN_DLC_INVALID,
    [23] = FDCAN_DLC_INVALID,
    [24] = FDCAN_DLC_BYTES_24,
    [25] = FDCAN_DLC_INVALID,
    [26] = FDCAN_DLC_INVALID,
    [27] = FDCAN_DLC_INVALID,
    [28] = FDCAN_DLC_INVALID,
    [29] = FDCAN_DLC_INVALID,
    [30] = FDCAN_DLC_INVALID,
    [31] = FDCAN_DLC_INVALID,
    [32] = FDCAN_DLC_BYTES_32,
    [33] = FDCAN_DLC_INVALID,
    [34] = FDCAN_DLC_INVALID,
    [35] = FDCAN_DLC_INVALID,
    [36] = FDCAN_DLC_INVALID,
    [37] = FDCAN_DLC_INVALID,
    [38] = FDCAN_DLC_INVALID,
    [39] = FDCAN_DLC_INVALID,
    [40] = FDCAN_DLC_INVALID,
    [41] = FDCAN_DLC_INVALID,
    [42] = FDCAN_DLC_INVALID,
    [43] = FDCAN_DLC_INVALID,
    [44] = FDCAN_DLC_INVALID,
    [45] = FDCAN_DLC_INVALID,
    [46] = FDCAN_DLC_INVALID,
    [47] = FDCAN_DLC_INVALID,
    [48] = FDCAN_DLC_BYTES_48,
    [49] = FDCAN_DLC_INVALID,
    [50] = FDCAN_DLC_INVALID,
    [51] = FDCAN_DLC_INVALID,
    [52] = FDCAN_DLC_INVALID,
    [53] = FDCAN_DLC_INVALID,
    [54] = FDCAN_DLC_INVALID,
    [55] = FDCAN_DLC_INVALID,
    [56] = FDCAN_DLC_INVALID,
    [57] = FDCAN_DLC_INVALID,
    [58] = FDCAN_DLC_INVALID,
    [59] = FDCAN_DLC_INVALID,
    [60] = FDCAN_DLC_INVALID,
    [61] = FDCAN_DLC_INVALID,
    [62] = FDCAN_DLC_INVALID,
    [63] = FDCAN_DLC_INVALID,
    [64] = FDCAN_DLC_BYTES_64,
};

static const uint32_t can_const_minimum_message_length[CAN_MESSAGE_PAYLOAD_LEN_MAX + 1] = {
    [0] = 0u, [1] = 1u, [2] = 2u, [3] = 3u, [4] = 4u, [5] = 5u, [6] = 6u, [7] = 7u, [8] = 8u,
    [9] = 12u, [10] = 12u, [11] = 12u, [12] = 12u, [13] = 16u, [14] = 16u, [15] = 16u, [16] = 16u,
    [17] = 20u, [18] = 20u, [19] = 20u, [20] = 20u, [21] = 24u, [22] = 24u, [23] = 24u, [24] = 24u,
    [25] = 32u, [26] = 32u, [27] = 32u, [28] = 32u, [29] = 32u, [30] = 32u, [31] = 32u, [32] = 32u,
    [33] = 48u, [34] = 48u, [35] = 48u, [36] = 48u, [37] = 48u, [38] = 48u, [39] = 48u, [40] = 48u,
    [41] = 48u, [42] = 48u, [43] = 48u, [44] = 48u, [45] = 48u, [46] = 48u, [47] = 48u, [48] = 48u,
    [49] = 64u, [50] = 64u, [51] = 64u, [52] = 64u, [53] = 64u, [54] = 64u, [55] = 64u, [56] = 64u,
    [57] = 64u, [58] = 64u, [59] = 64u, [60] = 64u, [61] = 64u, [62] = 64u, [63] = 64u, [64] = 64u,


};

static const uint32_t can_const_frame_length_to_message_length[16] = {
    [FDCAN_DLC_BYTES_0 >> 16u]  = 0u,
    [FDCAN_DLC_BYTES_1 >> 16u]  = 1u,
    [FDCAN_DLC_BYTES_2 >> 16u]  = 2u,
    [FDCAN_DLC_BYTES_3 >> 16u]  = 3u,
    [FDCAN_DLC_BYTES_4 >> 16u]  = 4u,
    [FDCAN_DLC_BYTES_5 >> 16u]  = 5u,
    [FDCAN_DLC_BYTES_6 >> 16u]  = 6u,
    [FDCAN_DLC_BYTES_7 >> 16u]  = 7u,
    [FDCAN_DLC_BYTES_8 >> 16u]  = 8u,
    [FDCAN_DLC_BYTES_12 >> 16u] = 12u,
    [FDCAN_DLC_BYTES_16 >> 16u] = 16u,
    [FDCAN_DLC_BYTES_20 >> 16u] = 20u,
    [FDCAN_DLC_BYTES_24 >> 16u] = 24u,
    [FDCAN_DLC_BYTES_32 >> 16u] = 32u,
    [FDCAN_DLC_BYTES_48 >> 16u] = 48u,
    [FDCAN_DLC_BYTES_64 >> 16u] = 64u,
};

error_t can_private_rx_get_msg(can_ctx_t *ctx, uint32_t rxindex)
{
  error_t err = E_OK;
  HAL_StatusTypeDef status;
  uint32_t read, write, pos;
  can_rx_cb_t *cb;
  bool handled = false;
  uint32_t data_length;

  do {
    status = HAL_FDCAN_GetRxMessage(ctx->init.handle, rxindex, &ctx->rxheader, ctx->rxmessage.payload);
    if(status == HAL_OK) {
      data_length = ctx->rxheader.DataLength >> 16u;
      BREAK_IF(data_length >= ITEMSOF(can_const_frame_length_to_message_length));

      ctx->rxmessage.id = ctx->rxheader.Identifier;
      if(ctx->rxheader.IdType == FDCAN_EXTENDED_ID) {
        ctx->rxmessage.id |= CAN_MESSAGE_EXTENDED_ID_FLAG;
      }

      if(ctx->rxheader.FDFormat == FDCAN_CLASSIC_CAN) {
        ctx->rxmessage.format = CAN_FRAME_FORMAT_CLASSIC;
        ctx->rxmessage.len = MIN(data_length, CAN_MESSAGE_CLASSIC_PAYLOAD_LEN_MAX);
      } else if(ctx->rxheader.FDFormat == FDCAN_FD_CAN) {
        ctx->rxmessage.format = CAN_FRAME_FORMAT_FDCAN;
        ctx->rxmessage.len = can_const_frame_length_to_message_length[data_length];
      } else {
        break;
      }

      for(int i = 0; i < CAN_RX_CB_MAX; i++) {
        cb = &ctx->rx_callbacks[i];
        BREAK_IF(cb->enabled == false);
        if(ctx->rxmessage.id >= cb->msg_id_start && ctx->rxmessage.id <= cb->msg_id_end) {
          if(cb->func != NULL) {
            cb->func(ctx, &ctx->rxmessage, cb->usrdata);
          }
          handled = true;
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
  } while(0);

  return err;
}

error_t can_private_tx_send_msg(can_ctx_t *ctx, const can_message_t *message)
{
  error_t err = E_OK;
  HAL_StatusTypeDef status;
  uint32_t free;
  uint32_t message_id;
  uint32_t data_length_field;

  do {
    free = HAL_FDCAN_GetTxFifoFreeLevel(ctx->init.handle);
    BREAK_IF_ACTION(free == 0, err = E_AGAIN);

    message_id = message->id;

    BREAK_IF_ACTION((message_id & CAN_MESSAGE_EXTENDED_ID_FLAG) != 0 && message_id > (0x1FFFFFFF | CAN_MESSAGE_EXTENDED_ID_FLAG), err = E_PARAM);
    BREAK_IF_ACTION((message_id & CAN_MESSAGE_EXTENDED_ID_FLAG) == 0 && message_id > 0x7FF, err = E_PARAM);
    BREAK_IF_ACTION(message->len > CAN_MESSAGE_PAYLOAD_LEN_MAX, err = E_PARAM);

    BREAK_IF_ACTION(message->format >= CAN_FRAME_FORMAT_MAX, err = E_PARAM);
    BREAK_IF_ACTION(message->format == CAN_FRAME_FORMAT_FDCAN && ctx->config.fdcan_supported == false, err = E_PARAM);
    BREAK_IF_ACTION(message->format == CAN_FRAME_FORMAT_CLASSIC && message->len > CAN_MESSAGE_CLASSIC_PAYLOAD_LEN_MAX, err = E_PARAM);
    BREAK_IF_ACTION(message->format == CAN_FRAME_FORMAT_FDCAN && message->len > CAN_MESSAGE_FD_PAYLOAD_LEN_MAX, err = E_PARAM);
    data_length_field = can_const_message_length_to_frame_length[message->len];
    BREAK_IF_ACTION(data_length_field == FDCAN_DLC_INVALID, err = E_PARAM);

    ctx->txheader.Identifier = message_id & ~CAN_MESSAGE_EXTENDED_ID_FLAG;
    if((message_id & CAN_MESSAGE_EXTENDED_ID_FLAG)) {
      ctx->txheader.IdType = FDCAN_EXTENDED_ID;
    } else {
      ctx->txheader.IdType = FDCAN_STANDARD_ID;
    }
    ctx->txheader.TxFrameType = FDCAN_DATA_FRAME;
    ctx->txheader.DataLength = data_length_field;
    ctx->txheader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
    switch(message->format) {
      case CAN_FRAME_FORMAT_CLASSIC:
        ctx->txheader.FDFormat = FDCAN_CLASSIC_CAN;
        ctx->txheader.BitRateSwitch = FDCAN_BRS_OFF;
        break;
      case CAN_FRAME_FORMAT_FDCAN:
        ctx->txheader.FDFormat = FDCAN_FD_CAN;
        if(message->len >= ctx->config.brs_threshold) {
          ctx->txheader.BitRateSwitch = FDCAN_BRS_ON;
        } else {
          ctx->txheader.BitRateSwitch = FDCAN_BRS_OFF;
        }
        break;
      default:
        err = E_PARAM;
        break;
    }
    ctx->txheader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
    ctx->txheader.MessageMarker = 0;

    status = HAL_FDCAN_AddMessageToTxFifoQ(ctx->init.handle, &ctx->txheader, (uint8_t *)message->payload);
    if(status != HAL_OK) {
      err = E_HAL;
      // TODO: DTC here
    }
  } while(0);

  return err;
}

error_t can_private_validate_message_length(can_frame_format_t format, uint32_t length)
{
  error_t err = E_OK;
  uint32_t data_length_field;

  do {
    BREAK_IF_ACTION(format >= CAN_FRAME_FORMAT_MAX, err = E_PARAM);
    BREAK_IF_ACTION(format == CAN_FRAME_FORMAT_CLASSIC && length > CAN_MESSAGE_CLASSIC_PAYLOAD_LEN_MAX, err = E_PARAM);
    BREAK_IF_ACTION(format == CAN_FRAME_FORMAT_FDCAN && length > CAN_MESSAGE_FD_PAYLOAD_LEN_MAX, err = E_PARAM);

    data_length_field = can_const_message_length_to_frame_length[length];
    BREAK_IF_ACTION(data_length_field == FDCAN_DLC_INVALID, err = E_PARAM);
  } while(0);

  return err;
}

error_t can_private_minimum_message_length(can_frame_format_t format, uint32_t req_length, uint32_t *length)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(format >= CAN_FRAME_FORMAT_MAX, err = E_PARAM);
    BREAK_IF_ACTION(format == CAN_FRAME_FORMAT_CLASSIC && req_length > CAN_MESSAGE_CLASSIC_PAYLOAD_LEN_MAX, err = E_PARAM);
    BREAK_IF_ACTION(format == CAN_FRAME_FORMAT_FDCAN && req_length > CAN_MESSAGE_FD_PAYLOAD_LEN_MAX, err = E_PARAM);
    BREAK_IF_ACTION(length == NULL, err = E_PARAM);

    *length = can_const_minimum_message_length[req_length];

  } while(0);

  return err;
}
