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
  uint32_t reducer;

  while(true) {
    now = time_now_us();

    switch(ctx->fsm_process) {
      case CRC_FSM_CONDITION:
        if(ctx->ready == true && ctx->cmd_ready == true && ctx->cmd_errcode == E_AGAIN) {
          ctx->cmd_len_left = ctx->cmd_length;
          ctx->fsm_process = CRC_FSM_DMA_REQUEST;

          if(ctx->cmd_reset != false) {
            __HAL_CRC_DR_RESET(ctx->hcrc);
          }

          while((ctx->cmd_address & (sizeof(uint32_t) - 1)) != 0 && ctx->cmd_len_left > 0) {
            *(__IO uint8_t *)&ctx->hcrc->Instance->DR = *(uint8_t *)ctx->cmd_address++;

            ctx->cmd_len_left--;
          }
          ctx->cmd_len_cur = ctx->cmd_len_left > CRC_DMA_MAX_LEN ? CRC_DMA_MAX_LEN : ctx->cmd_len_left;
          ctx->cmd_len_left -= ctx->cmd_len_cur;

          continue;
        }

        break;
      case CRC_FSM_DMA_REQUEST:
        reducer = ctx->cmd_len_cur & (sizeof(uint32_t) - 1);
        if(reducer > 0) {
          ctx->cmd_len_cur -= reducer;
          ctx->cmd_len_left += reducer;
        }

        if(ctx->cmd_len_cur > 0) {
          ctx->dma_errcode = E_AGAIN;
          CacheClean((const void *)ctx->cmd_address, ctx->cmd_len_cur);
          status = HAL_MDMA_Start_IT(ctx->hdma, ctx->cmd_address, (uint32_t)&ctx->hcrc->Instance->DR, ctx->cmd_len_cur, 1);
          if(status == HAL_OK) {
            ctx->fsm_process = CRC_FSM_DMA_WAIT;
            ctx->dma_timestamp = now;
            continue;
          } else {
            err = E_HAL;
            ctx->cmd_errcode = err;
            ctx->fsm_process = CRC_FSM_CONDITION;
          }
        } else {
          while(ctx->cmd_len_left > 0) {
            *(__IO uint8_t *)&ctx->hcrc->Instance->DR = *(uint8_t *)ctx->cmd_address++;
            ctx->cmd_len_left--;
          }
          err = E_OK;
          ctx->cmd_errcode = err;
          ctx->fsm_process = CRC_FSM_CONDITION;
        }
        break;
      case CRC_FSM_DMA_WAIT:
        err = ctx->dma_errcode;
        if(err == E_OK) {
          ctx->cmd_address += ctx->cmd_len_cur;
          if(ctx->cmd_len_left == 0) {
            err = E_OK;
            ctx->cmd_errcode = err;
            ctx->fsm_process = CRC_FSM_CONDITION;
          } else {
            ctx->cmd_len_cur = ctx->cmd_len_left > CRC_DMA_MAX_LEN ? CRC_DMA_MAX_LEN : ctx->cmd_len_left;
            ctx->cmd_len_left -= ctx->cmd_len_cur;
            ctx->fsm_process = CRC_FSM_DMA_REQUEST;
            continue;
          }
        } else if(err != E_AGAIN) {
          ctx->cmd_errcode = err;
          ctx->fsm_process = CRC_FSM_CONDITION;
        } else if(time_diff(now, ctx->dma_timestamp) >= CRC_DMA_TIMEOUT_US) {
          ctx->dma_errcode = E_AGAIN;
          status = HAL_MDMA_Abort_IT(ctx->hdma);
          if(status == HAL_OK) {
            ctx->dma_timestamp = now;
            ctx->fsm_process = CRC_FSM_DMA_WAIT_ABORT;
          } else {
            ctx->cmd_errcode = E_TIMEOUT;
            ctx->fsm_process = CRC_FSM_CONDITION;
          }
        }
        break;
      case CRC_FSM_DMA_WAIT_ABORT:
        err = ctx->dma_errcode;
        if(err != E_AGAIN) {
          ctx->cmd_errcode = E_TIMEOUT;
          ctx->fsm_process = CRC_FSM_CONDITION;
        } else if(time_diff(now, ctx->dma_timestamp) >= CRC_DMA_TIMEOUT_US) {
          ctx->cmd_errcode = E_TIMEOUT;
          ctx->fsm_process = CRC_FSM_CONDITION;
        }
        break;
      default:
        break;
    }

    break;
  }

  return err;
}
