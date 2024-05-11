/*
 * crc_fsm.c
 *
 *  Created on: May 5, 2024
 *      Author: VHEMaster
 */

#include "crc_fsm.h"
#include "compiler.h"

ITCM_FUNC error_t crc_fsm(crc_ctx_t *ctx)
{
  error_t err = E_OK;
  HAL_StatusTypeDef status;
  time_us_t now;

  while(true) {
    now = time_get_current_us();

    switch(ctx->fsm_process) {
      case CRC_FSM_CONDITION:
        if(ctx->ready == true && ctx->cmd_ready == true && ctx->cmd_errcode == E_AGAIN) {
          ctx->cmd_len_left = ctx->cmd_length;
          ctx->cmd_len_cur = ctx->cmd_len_left > CRC_DMA_MAX_LEN ? CRC_DMA_MAX_LEN : ctx->cmd_len_left;
          ctx->dma_errcode = E_AGAIN;
          ctx->fsm_process = CRC_FSM_DMA_REQUEST;

          CacheClean((const void *)ctx->cmd_address, ctx->cmd_len_cur);
          if(ctx->cmd_reset != false) {
            __HAL_CRC_DR_RESET(ctx->hcrc);
          }
          continue;
        }
        break;
      case CRC_FSM_DMA_REQUEST:
        status = HAL_DMA_Start_IT(ctx->hdma, ctx->cmd_address, (uint32_t)&ctx->hcrc->Instance->DR, ctx->cmd_len_cur);
        if(status == HAL_OK) {
          ctx->fsm_process = CRC_FSM_DMA_WAIT;
          ctx->dma_timestamp = now;
          continue;
        } else {
          err = E_HAL;
          ctx->cmd_errcode = err;
          ctx->fsm_process = CRC_FSM_CONDITION;
        }
        break;
      case CRC_FSM_DMA_WAIT:
        err = ctx->dma_errcode;
        if(err == E_OK) {
          if(ctx->cmd_len_left > ctx->cmd_len_cur) {
            ctx->cmd_len_left -= ctx->cmd_len_cur;
          } else {
            ctx->cmd_len_left = 0;
          }
          if(ctx->cmd_len_left == 0) {
            err = E_OK;
            ctx->cmd_errcode = err;
            ctx->fsm_process = CRC_FSM_CONDITION;
          } else {
            ctx->cmd_len_cur = ctx->cmd_len_left > CRC_DMA_MAX_LEN ? CRC_DMA_MAX_LEN : ctx->cmd_len_left;
            ctx->fsm_process = CRC_FSM_DMA_REQUEST;
            continue;
          }
        } else if(err != E_AGAIN) {
          ctx->cmd_errcode = ctx->dma_errcode;
          ctx->fsm_process = CRC_FSM_CONDITION;
        } else if(time_diff(now, ctx->dma_timestamp) >= CRC_DMA_TIMEOUT_US) {
          ctx->dma_errcode = E_TIMEOUT;
          continue;
        }
        break;
      default:
        break;
    }

    break;
  }

  return err;
}
