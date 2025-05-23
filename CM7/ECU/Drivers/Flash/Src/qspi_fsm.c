/*
 * qspi_fsm.c
 *
 *  Created on: Apr 25, 2024
 *      Author: VHEMaster
 */

#include "qspi_fsm.h"
#include "compiler.h"
#include <string.h>

ITCM_FUNC static error_t qspi_fsm_io(qspi_ctx_t *ctx)
{
  HAL_StatusTypeDef status;
  error_t err, cmd_async_errcode;
  time_us_t now;

  while(true) {
    err = E_AGAIN;
    now = time_now_us();
    cmd_async_errcode = ctx->cmd_async_errcode;

    switch(ctx->fsm_io) {
      case QSPI_FSM_IO_INITIAL:
        if(ctx->cmd_wren_needed == true) {
          ctx->fsm_io = QSPI_FSM_IO_WREN_REQ;
          continue;
        } else {
          ctx->fsm_io = QSPI_FSM_IO_CMD_REQ;
          continue;
        }
        break;
      case QSPI_FSM_IO_WREN_REQ:
        ctx->cmd_async_errcode = E_AGAIN;
        status = HAL_QSPI_Command_IT(ctx->init.hqspi, &ctx->init.cmd_wren);
        if(status == HAL_OK) {
          ctx->fsm_io = QSPI_FSM_IO_WREN_WAIT;
          ctx->cmd_timestamp = now;
        } else {
          err = E_HAL;
        }
        break;
      case QSPI_FSM_IO_WREN_WAIT:
        if(cmd_async_errcode == E_OK) {
          ctx->fsm_io = QSPI_FSM_IO_CMD_REQ;
          continue;
        } else if(cmd_async_errcode == E_AGAIN) {
          if(time_diff(now, ctx->cmd_timestamp) >= QSPI_CMD_TIMEOUT_US) {
            ctx->fsm_io = QSPI_FSM_IO_ABORT_REQ;
            continue;
          }
        } else {
          err = cmd_async_errcode;
        }
        break;
      case QSPI_FSM_IO_CMD_REQ:
        ctx->cmd_async_errcode = E_AGAIN;
        status = HAL_QSPI_Command_IT(ctx->init.hqspi, ctx->cmd_ptr);
        if(status == HAL_OK) {
          if(ctx->cmd_ptr->DataMode == QSPI_DATA_NONE) {
            ctx->fsm_io = QSPI_FSM_IO_CMD_WAIT;
            ctx->cmd_timestamp = now;
          } else {
            ctx->fsm_io = QSPI_FSM_IO_PAYLOAD_REQ;
            continue;
          }
        } else {
          err = E_HAL;
        }
        break;
      case QSPI_FSM_IO_CMD_WAIT:
        if(cmd_async_errcode == E_OK) {
          if(ctx->cmd_ptr->DataMode != QSPI_DATA_NONE) {
            ctx->fsm_io = QSPI_FSM_IO_PAYLOAD_REQ;
            continue;
          } else if(ctx->cmd_status_poll_needed == true) {
            if(ctx->cmd_poll_delay > 0) {
              ctx->fsm_io = QSPI_FSM_IO_STATUS_DELAY;
              ctx->cmd_timestamp = now;
              break;
            } else {
              ctx->fsm_io = QSPI_FSM_IO_STATUS_REQ;
              continue;
            }
          } else {
            ctx->fsm_io = QSPI_FSM_IO_INITIAL;
            err = cmd_async_errcode;
          }
        } else if(cmd_async_errcode == E_AGAIN) {
          if(time_diff(now, ctx->cmd_timestamp) >= QSPI_CMD_TIMEOUT_US) {
            ctx->fsm_io = QSPI_FSM_IO_ABORT_REQ;
            continue;
          }
        } else {
          err = cmd_async_errcode;
        }
        break;
      case QSPI_FSM_IO_PAYLOAD_REQ:
        ctx->cmd_async_errcode = E_AGAIN;
        status = HAL_ERROR;
        if(ctx->init.hqspi->hmdma != NULL && ctx->cmd_ptr->NbData >= 4) {
          ctx->cmd_uses_dma = true;
          if(ctx->cmd_payload_tx != NULL && ctx->cmd_payload_rx == NULL) {
            CacheClean(ctx->cmd_payload_tx, ctx->cmd_ptr->NbData);
            status = HAL_QSPI_Transmit_DMA(ctx->init.hqspi, (uint8_t *)ctx->cmd_payload_tx);
          } else if(ctx->cmd_payload_tx == NULL && ctx->cmd_payload_rx != NULL) {
            CacheClean(ctx->cmd_payload_rx, ctx->cmd_ptr->NbData);
            status = HAL_QSPI_Receive_DMA(ctx->init.hqspi, (uint8_t *)ctx->cmd_payload_rx);
          }
        } else {
          ctx->cmd_uses_dma = false;
          if(ctx->cmd_payload_tx != NULL && ctx->cmd_payload_rx == NULL) {
            status = HAL_QSPI_Transmit_IT(ctx->init.hqspi, (uint8_t *)ctx->cmd_payload_tx);
          } else if(ctx->cmd_payload_tx == NULL && ctx->cmd_payload_rx != NULL) {
            status = HAL_QSPI_Receive_IT(ctx->init.hqspi, (uint8_t *)ctx->cmd_payload_rx);
          }
        }
        if(status == HAL_OK) {
          ctx->fsm_io = QSPI_FSM_IO_PAYLOAD_WAIT;
          ctx->cmd_timestamp = now;
        } else {
          err = E_HAL;
        }
        break;
      case QSPI_FSM_IO_PAYLOAD_WAIT:
        if(cmd_async_errcode == E_OK) {
          if(ctx->cmd_uses_dma == true) {
            if(ctx->cmd_payload_rx != NULL) {
              CacheInvalidate(ctx->cmd_payload_rx, ctx->cmd_ptr->NbData);
            }
          }
          if(ctx->cmd_status_poll_needed == true) {
            if(ctx->cmd_poll_delay > 0) {
              ctx->fsm_io = QSPI_FSM_IO_STATUS_DELAY;
              ctx->cmd_timestamp = now;
              break;
            } else {
              ctx->fsm_io = QSPI_FSM_IO_STATUS_REQ;
              continue;
            }
          } else {
            ctx->fsm_io = QSPI_FSM_IO_INITIAL;
            err = cmd_async_errcode;
          }
        } else if(cmd_async_errcode == E_AGAIN) {
          if(time_diff(now, ctx->cmd_timestamp) >= QSPI_PAYLOAD_TIMEOUT_US) {
            ctx->fsm_io = QSPI_FSM_IO_ABORT_REQ;
            continue;
          }
        } else {
          err = cmd_async_errcode;
        }
        break;
      case QSPI_FSM_IO_STATUS_DELAY:
        if(time_diff(now, ctx->cmd_timestamp) >= ctx->cmd_poll_delay) {
          ctx->cmd_async_errcode = E_AGAIN;
          ctx->fsm_io = QSPI_FSM_IO_STATUS_REQ;
          ctx->cmd_timestamp = now;
          continue;
        }
        break;
      case QSPI_FSM_IO_STATUS_REQ:
        ctx->cmd_async_errcode = E_AGAIN;
        status = HAL_QSPI_AutoPolling_IT(ctx->init.hqspi, &ctx->init.cmd_rdsr, &ctx->cmd_poll);
        if(status == HAL_OK) {
          ctx->fsm_io = QSPI_FSM_IO_STATUS_WAIT;
          ctx->cmd_timestamp = now;
        } else {
          err = E_HAL;
        }
        break;
      case QSPI_FSM_IO_STATUS_WAIT:
        if(cmd_async_errcode == E_OK) {
          ctx->fsm_io = QSPI_FSM_IO_INITIAL;
          err = cmd_async_errcode;
        } else if(cmd_async_errcode == E_AGAIN) {
          if(time_diff(now, ctx->cmd_timestamp) >= ctx->cmd_poll_timeout) {
            ctx->fsm_io = QSPI_FSM_IO_ABORT_REQ;
            continue;
          }
        } else {
          err = cmd_async_errcode;
        }
        break;
      case QSPI_FSM_IO_ABORT_REQ:
        ctx->cmd_async_errcode = E_AGAIN;
        status = HAL_QSPI_Abort_IT(ctx->init.hqspi);
        if(status == HAL_OK) {
          ctx->fsm_io = QSPI_FSM_IO_ABORT_WAIT;
          ctx->cmd_timestamp = now;
        } else {
          err = E_HAL;
        }
        break;
      case QSPI_FSM_IO_ABORT_WAIT:
        if(cmd_async_errcode != E_AGAIN) {
          err = cmd_async_errcode;
        } else if(time_diff(now, ctx->cmd_timestamp) >= QSPI_ABORT_TIMEOUT_US) {
          err = E_TIMEOUT;
        }
        break;
      default:
        break;
    }
    break;
  }

  if(err != E_OK && err != E_AGAIN) {
    ctx->fsm_io = QSPI_FSM_IO_INITIAL;
  }

  return err;
}

ITCM_FUNC static error_t qspi_fsm_init(qspi_ctx_t *ctx)
{
  error_t err = E_OK;
  time_us_t now;

  while(true) {
    err = E_OK;
    now = time_now_us();

    switch(ctx->fsm_init) {
      case QSPI_FSM_INIT_CONDITION:
        if(ctx->init_request == true && ctx->init_errcode == E_AGAIN) {
          ctx->fsm_init = QSPI_FSM_INIT_QUAD_RST1;
          ctx->cmd_ptr = &ctx->init.cmd_rstqioq;
          ctx->cmd_status_poll_needed = false;
          ctx->cmd_wren_needed = false;
          err = E_AGAIN;
          continue;
        }
        break;
      case QSPI_FSM_INIT_QUAD_RST1:
        err = qspi_fsm_io(ctx);
        if(err == E_OK) {
          ctx->fsm_init = QSPI_FSM_INIT_QUAD_RST2;
          ctx->cmd_ptr = &ctx->init.cmd_rstqios;
          err = E_AGAIN;
          continue;
        } else if(err != E_AGAIN) {
          ctx->fsm_init = QSPI_FSM_INIT_CONDITION;
          ctx->init_errcode = err;
        }
        break;
      case QSPI_FSM_INIT_QUAD_RST2:
        err = qspi_fsm_io(ctx);
        if(err == E_OK) {
          ctx->fsm_init = QSPI_FSM_INIT_RESET_EN;
          ctx->cmd_ptr = &ctx->init.cmd_rsten;
          err = E_AGAIN;
          continue;
        } else if(err != E_AGAIN) {
          ctx->fsm_init = QSPI_FSM_INIT_CONDITION;
          ctx->init_errcode = err;
        }
        break;
      case QSPI_FSM_INIT_RESET_EN:
        err = qspi_fsm_io(ctx);
        if(err == E_OK) {
          ctx->fsm_init = QSPI_FSM_INIT_RESET;
          ctx->cmd_ptr = &ctx->init.cmd_rst;
          err = E_AGAIN;
          continue;
        } else if(err != E_AGAIN) {
          ctx->fsm_init = QSPI_FSM_INIT_CONDITION;
          ctx->init_errcode = err;
        }
        break;
      case QSPI_FSM_INIT_RESET:
        err = qspi_fsm_io(ctx);
        if(err == E_OK) {
          ctx->fsm_init = QSPI_FSM_INIT_JEDEC;
          ctx->cmd_ptr = &ctx->init.cmd_jid;
          ctx->cmd_payload_tx = NULL;
          ctx->cmd_payload_rx = &ctx->init.dma_ctx->jedec_quad;
          err = E_AGAIN;
          continue;
        } else if(err != E_AGAIN) {
          ctx->fsm_init = QSPI_FSM_INIT_CONDITION;
          ctx->init_errcode = err;
        }
        break;
      case QSPI_FSM_INIT_JEDEC:
        err = qspi_fsm_io(ctx);
        if(err == E_OK) {
          for(int i = 0; i < ITEMSOF(ctx->init.dma_ctx->jedec_quad.mfg_id); i++) {
            if(ctx->init.dma_ctx->jedec_quad.mfg_id[i] != ctx->init.expected_jedec.mfg_id) {
              err = E_BADRESP;
            }
          }
          for(int i = 0; i < ITEMSOF(ctx->init.dma_ctx->jedec_quad.device_type); i++) {
            if(ctx->init.dma_ctx->jedec_quad.device_type[i] != ctx->init.expected_jedec.device_type) {
              err = E_BADRESP;
            }
          }
          for(int i = 0; i < ITEMSOF(ctx->init.dma_ctx->jedec_quad.device_id); i++) {
            if(ctx->init.dma_ctx->jedec_quad.device_id[i] != ctx->init.expected_jedec.device_id) {
              err = E_BADRESP;
            }
          }

          ctx->cmd_payload_rx = NULL;
          ctx->cmd_payload_tx = NULL;

          if(err == E_OK) {
            ctx->fsm_init = QSPI_FSM_INIT_QUAD;
            ctx->cmd_ptr = &ctx->init.cmd_eqio;
            ctx->cmd_status_poll_needed = true;
            ctx->cmd_poll_delay = 0;
            ctx->cmd_poll_timeout = QSPI_CMD_TIMEOUT_US;
            err = E_AGAIN;
            continue;
          } else {
            ctx->fsm_init = QSPI_FSM_INIT_CONDITION;
            ctx->init_errcode = err;
          }
        } else if(err != E_AGAIN) {
          ctx->fsm_init = QSPI_FSM_INIT_CONDITION;
          ctx->init_errcode = err;
        }
        break;
      case QSPI_FSM_INIT_QUAD:
        err = qspi_fsm_io(ctx);
        if(err == E_OK) {
          ctx->fsm_init = QSPI_FSM_INIT_QUAD_WAIT;
          ctx->cmd_timestamp = now;
          err = E_AGAIN;
        } else if(err != E_AGAIN) {
          ctx->fsm_init = QSPI_FSM_INIT_CONDITION;
          ctx->init_errcode = err;
        }
        break;
      case QSPI_FSM_INIT_QUAD_WAIT:
        if(time_diff(now, ctx->cmd_timestamp) >= QSPI_QUAD_INIT_DELAY_US) {
          ctx->fsm_init = QSPI_FSM_INIT_QJEDEC;
          ctx->cmd_ptr = &ctx->init.cmd_qjid;
          ctx->cmd_payload_tx = NULL;
          ctx->cmd_payload_rx = &ctx->init.dma_ctx->jedec_quad;
          memset(&ctx->init.dma_ctx->jedec_quad, 0, sizeof(ctx->init.dma_ctx->jedec_quad));
          err = E_AGAIN;
          continue;
        }
        break;
      case QSPI_FSM_INIT_QJEDEC:
        err = qspi_fsm_io(ctx);
        if(err == E_OK) {
          for(int i = 0; i < ITEMSOF(ctx->init.dma_ctx->jedec_quad.mfg_id); i++) {
            if(ctx->init.dma_ctx->jedec_quad.mfg_id[i] != ctx->init.expected_jedec.mfg_id) {
              err = E_BADRESP;
            }
          }
          for(int i = 0; i < ITEMSOF(ctx->init.dma_ctx->jedec_quad.device_type); i++) {
            if(ctx->init.dma_ctx->jedec_quad.device_type[i] != ctx->init.expected_jedec.device_type) {
              err = E_BADRESP;
            }
          }
          for(int i = 0; i < ITEMSOF(ctx->init.dma_ctx->jedec_quad.device_id); i++) {
            if(ctx->init.dma_ctx->jedec_quad.device_id[i] != ctx->init.expected_jedec.device_id) {
              err = E_BADRESP;
            }
          }

          ctx->cmd_payload_rx = NULL;
          ctx->cmd_payload_tx = NULL;

          ctx->jedec.mfg_id = ctx->init.dma_ctx->jedec_quad.mfg_id[0];
          ctx->jedec.device_type = ctx->init.dma_ctx->jedec_quad.device_type[0];
          ctx->jedec.device_id = ctx->init.dma_ctx->jedec_quad.device_id[0];
          ctx->jedec_ready = true;

          if(err == E_OK) {
            ctx->fsm_init = QSPI_FSM_INIT_CFG;
            ctx->cmd_ptr = &ctx->init.cmd_wrsr;
            ctx->cmd_payload_tx = ctx->init.dma_ctx->payload_dummy;
            ctx->cmd_payload_rx = NULL;
            ctx->cmd_wren_needed = false;
            ctx->cmd_status_poll_needed = true;
            ctx->cmd_poll_delay = 0;
            ctx->cmd_poll_timeout = QSPI_CMD_TIMEOUT_US;
            err = E_AGAIN;
            continue;
          } else {
            ctx->fsm_init = QSPI_FSM_INIT_CONDITION;
            ctx->init_errcode = err;
          }
        } else if(err != E_AGAIN) {
          ctx->fsm_init = QSPI_FSM_INIT_CONDITION;
          ctx->init_errcode = err;
        }
        break;
      case QSPI_FSM_INIT_CFG:
        err = qspi_fsm_io(ctx);
        if(err == E_OK) {
          ctx->fsm_init = QSPI_FSM_INIT_BPR;
          ctx->cmd_ptr = &ctx->init.cmd_wbpr;

          for(int i = 0; i < QSPI_BPR_SIZE; i++) {
            ctx->bpr.bytes[i] = ctx->init.bpr.bytes[i];
            ctx->init.dma_ctx->payload_bpr[i * 2] = ctx->bpr.bytes[i];
            ctx->init.dma_ctx->payload_bpr[i * 2 + 1] = ctx->bpr.bytes[i];
          }

          ctx->cmd_payload_tx = ctx->init.dma_ctx->payload_bpr;
          ctx->cmd_payload_rx = NULL;
          ctx->cmd_wren_needed = true;
          ctx->cmd_status_poll_needed = true;
          ctx->cmd_poll_delay = 0;
          ctx->cmd_poll_timeout = QSPI_BPR_TIMEOUT_US;
        } else if(err != E_AGAIN) {
          ctx->fsm_init = QSPI_FSM_INIT_CONDITION;
          ctx->init_errcode = err;
        }
        break;
      case QSPI_FSM_INIT_BPR:
        err = qspi_fsm_io(ctx);
        if(err == E_OK) {
          ctx->cmd_payload_tx = NULL;
          ctx->cmd_payload_rx = NULL;
          ctx->cmd_wren_needed = false;
          ctx->cmd_status_poll_needed = false;
          ctx->fsm_init = QSPI_FSM_INIT_CONDITION;
          ctx->init_errcode = err;
          ctx->initialized = true;
        } else if(err != E_AGAIN) {
          ctx->fsm_init = QSPI_FSM_INIT_CONDITION;
          ctx->init_errcode = err;
        }
        break;
      default:
        break;
    }
    break;
  }

  return err;
}

static error_t qspi_fsm_process(qspi_ctx_t *ctx)
{
  error_t err = E_OK;
  HAL_StatusTypeDef status;

  while(true) {
    err = E_OK;

    switch(ctx->fsm_process) {
      case QSPI_FSM_PROCESS_CONDITION:
        if(ctx->cmd_errcode == E_AGAIN) {
          if(ctx->initialized == true && ctx->cmd_ready == true &&
              ctx->memory_mapping == false && ctx->memory_mapping_accept == false) {
            ctx->fsm_process = QSPI_FSM_PROCESS_IO;
            continue;
          } else if(ctx->memory_mapping != ctx->memory_mapping_accept) {
            ctx->fsm_process = QSPI_FSM_MEMORYMAPPING_HAL;
            continue;
          }
        }
        break;
      case QSPI_FSM_PROCESS_IO:
        err = qspi_fsm_io(ctx);
        if(err != E_AGAIN) {
          ctx->fsm_process = QSPI_FSM_PROCESS_CONDITION;
          ctx->cmd_errcode = err;
        }
        break;

      case QSPI_FSM_MEMORYMAPPING_HAL:
        if(ctx->memory_mapping == false) {
          status = HAL_QSPI_Abort(ctx->init.hqspi);
        } else {
          status = HAL_QSPI_MemoryMapped(ctx->init.hqspi, &ctx->init.cmd_hsread, &ctx->init.mem_map);
        }
        if(status != HAL_OK) {
          err = E_HAL;
          ctx->fsm_process = QSPI_FSM_PROCESS_CONDITION;
          ctx->cmd_errcode = err;
        } else {
          if(ctx->memory_mapping == false) {
            err = E_OK;
            ctx->cmd_errcode = E_OK;
            ctx->fsm_process = QSPI_FSM_PROCESS_CONDITION;
            ctx->memory_mapping_accept = ctx->memory_mapping;
          } else {
            err = E_OK;
            ctx->cmd_errcode = E_OK;
            ctx->fsm_process = QSPI_FSM_MEMORYMAPPING_WAIT;
            ctx->memory_mapping_accept = ctx->memory_mapping;
          }
        }
        break;
      case QSPI_FSM_MEMORYMAPPING_WAIT:
        if(ctx->memory_mapping == false || ctx->memory_mapping_accept == false) {
          ctx->fsm_process = QSPI_FSM_MEMORYMAPPING_HAL;
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

ITCM_FUNC error_t qspi_fsm(qspi_ctx_t *ctx)
{
  error_t err = E_OK;

  if(ctx->initialized == false && ctx->init_errcode == E_AGAIN) {
    err = qspi_fsm_init(ctx);
  } else if(ctx->initialized == true) {
    err = qspi_fsm_process(ctx);
  }

  return err;
}

