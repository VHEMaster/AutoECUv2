/*
 * qspi_fsm.c
 *
 *  Created on: Apr 25, 2024
 *      Author: VHEMaster
 */

#include "qspi_fsm.h"
#include "compiler.h"

static error_t qspi_fsm_io(qspi_ctx_t *ctx)
{
  HAL_StatusTypeDef status;
  error_t err, cmd_async_errcode;
  time_us_t now;

  while(true) {
    err = E_AGAIN;
    now = time_get_current_us();
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
        if(ctx->cmd_ptr->DataMode == QSPI_DATA_NONE) {
          status = HAL_QSPI_Command_IT(ctx->init.hqspi, ctx->cmd_ptr);
          if(status == HAL_OK) {
            ctx->fsm_io = QSPI_FSM_IO_CMD_WAIT;
            ctx->cmd_timestamp = now;
          } else {
            err = E_HAL;
          }
        } else {
          status = HAL_QSPI_Command(ctx->init.hqspi, ctx->cmd_ptr, 0);
          if(status == HAL_OK) {
            ctx->fsm_io = QSPI_FSM_IO_PAYLOAD_REQ;
            continue;
          } else {
            err = E_HAL;
          }
        }
        break;
      case QSPI_FSM_IO_CMD_WAIT:
        if(cmd_async_errcode == E_OK) {
          if(ctx->cmd_ptr->DataMode != QSPI_DATA_NONE) {
            ctx->fsm_io = QSPI_FSM_IO_PAYLOAD_REQ;
            continue;
          }
          else if(ctx->cmd_status_poll_needed == true) {
            ctx->fsm_io = QSPI_FSM_IO_STATUS_REQ;
            continue;
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
        //if(ctx->init.hqspi->hmdma != NULL && ctx->cmd_ptr->NbData >= 4) {
        if(0) {
          if(ctx->cmd_payload_tx != NULL && ctx->cmd_payload_rx == NULL) {
            status = HAL_QSPI_Transmit_DMA(ctx->init.hqspi, (uint8_t *)ctx->cmd_payload_tx);
          } else if(ctx->cmd_payload_tx == NULL && ctx->cmd_payload_rx != NULL) {
            status = HAL_QSPI_Receive_DMA(ctx->init.hqspi, (uint8_t *)ctx->cmd_payload_rx);
          }
        } else {
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
          if(ctx->cmd_status_poll_needed == true) {
            ctx->fsm_io = QSPI_FSM_IO_STATUS_REQ;
            continue;
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

static error_t qspi_fsm_init(qspi_ctx_t *ctx)
{
  error_t err = E_OK;

  while(true) {
    err = E_OK;

    switch(ctx->fsm_init) {
      case QSPI_FSM_INIT_CONDITION:
        if(ctx->initialized == false && ctx->init_errcode == E_AGAIN) {
          ctx->fsm_init = QSPI_FSM_INIT_QUAD_RST;
          ctx->cmd_ptr = &ctx->init.cmd_rstqio;
          ctx->cmd_status_poll_needed = false;
          ctx->cmd_wren_needed = false;
          err = E_AGAIN;
          continue;
        }
        break;
      case QSPI_FSM_INIT_QUAD_RST:
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
          ctx->cmd_payload_rx = &ctx->jedec_quad;
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
          ctx->cmd_payload_rx = NULL;

          for(int i = 0; i < ITEMSOF(ctx->jedec_quad.mfg_id); i++) {
            if(ctx->jedec_quad.mfg_id[i] != ctx->init.expected_jedec.mfg_id) {
              err = E_BADRESP;
            }
          }
          for(int i = 0; i < ITEMSOF(ctx->jedec_quad.device_type); i++) {
            if(ctx->jedec_quad.device_type[i] != ctx->init.expected_jedec.device_type) {
              err = E_BADRESP;
            }
          }
          for(int i = 0; i < ITEMSOF(ctx->jedec_quad.device_id); i++) {
            if(ctx->jedec_quad.device_id[i] != ctx->init.expected_jedec.device_id) {
              err = E_BADRESP;
            }
          }
          ctx->jedec.mfg_id = ctx->jedec_quad.mfg_id[0];
          ctx->jedec.device_type = ctx->jedec_quad.device_type[0];
          ctx->jedec.device_id = ctx->jedec_quad.device_id[0];
          ctx->jedec_ready = true;

          if(err == E_OK) {
            ctx->fsm_init = QSPI_FSM_INIT_QUAD;
            ctx->cmd_ptr = &ctx->init.cmd_eqio;
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

  while(true) {
    err = E_OK;

    switch(ctx->fsm_process) {
      case QSPI_FSM_PROCESS_CONDITION:
        if(ctx->initialized == true && ctx->cmd_ready == true && ctx->cmd_errcode == E_AGAIN) {
          ctx->fsm_process = QSPI_FSM_PROCESS_IO;
          continue;
        }
        break;
      case QSPI_FSM_PROCESS_IO:
        err = qspi_fsm_io(ctx);
        if(err != E_AGAIN) {
          ctx->fsm_init = QSPI_FSM_INIT_CONDITION;
          ctx->cmd_errcode = err;
        }
        break;
      default:
        break;
    }
    break;
  }

  return err;
}

error_t qspi_fsm(qspi_ctx_t *ctx)
{
  error_t err = E_OK;

  if(ctx->initialized == false && ctx->init_errcode == E_AGAIN) {
    err = qspi_fsm_init(ctx);
  } else if(ctx->initialized == true) {
    err = qspi_fsm_process(ctx);
  }

  return err;
}

