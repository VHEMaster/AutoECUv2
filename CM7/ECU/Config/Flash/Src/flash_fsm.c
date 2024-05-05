/*
 * flash_fsm.c
 *
 *  Created on: May 4, 2024
 *      Author: VHEMaster
 */

#include "flash_fsm.h"
#include "flash_memory_layout.h"
#include "crc.h"
#include "compiler.h"

error_t flash_fsm(flash_runtime_ctx_t *ctx)
{
  error_t err = E_OK;
  uint32_t insector_addr_mask;
  uint32_t address;
  const flash_mem_layout_section_info_t *cmd_section;

  while(true) {
    switch(ctx->fsm_process) {
      case FLASH_FSM_CONDITION:
        if(ctx->ready == true && ctx->cmd_errcode == E_AGAIN && ctx->cmd_request != FLASH_CMD_NONE) {
          if(ctx->cmd_request < FLASH_CMD_MAX) {
            ctx->fsm_process = FLASH_FSM_FLASH_LOCK;
            ctx->cmd_errcode_internal = E_AGAIN;
          } else {
            err = E_PARAM;
            ctx->cmd_errcode = err;
            ctx->fsm_process = FLASH_FSM_CONDITION;
          }
          if(ctx->cmd_errcode == E_AGAIN) {
            continue;
          }
        }
        break;
      case FLASH_FSM_FLASH_LOCK:
        err = qspi_lock(ctx->qspi_ctx);
        if(err == E_OK) {
          ctx->fsm_process = FLASH_FSM_SECTION_INFO;
          continue;
        } else if(err != E_AGAIN) {
          ctx->cmd_errcode = err;
          ctx->fsm_process = FLASH_FSM_CONDITION;
        }
        break;
      case FLASH_FSM_SECTION_INFO:
        err = flash_mem_layout_get_section_info(&cmd_section, ctx->cmd_section_type, ctx->cmd_section_index);
        if(err == E_OK) {
          ctx->cmd_erase_type = cmd_section->uses_full_block ? FLASH_ERASE_TYPE_BLOCK : FLASH_ERASE_TYPE_SECTOR;
          ctx->fsm_process = FLASH_FSM_SECTION_ADDR;
          ctx->cmd_dupl_index = 0;
          continue;
        } else if(err != E_AGAIN) {
          ctx->cmd_errcode_internal = err;
          ctx->fsm_process = FLASH_FSM_FLASH_UNLOCK;
        }

        break;
      case FLASH_FSM_SECTION_ADDR:
        err = flash_mem_layout_get_section_address(&address, ctx->cmd_section_type, ctx->cmd_section_index);
        if(err == E_OK) {
          ctx->cmd_address = address;

          if(ctx->cmd_dupl_index >= 2) {
            if(ctx->cmd_request == FLASH_CMD_READ) {
              err = E_BADVALUE;
            } else if(ctx->cmd_request == FLASH_CMD_WRITE) {
              err = E_OK;
            }
            ctx->cmd_errcode_internal = err;
            ctx->fsm_process = FLASH_FSM_FLASH_UNLOCK;
            continue;
          } else if(ctx->cmd_dupl_index > 0) {
            insector_addr_mask = ctx->qspi_ctx->init.erase_sector_size - 1;
            address = ctx->cmd_address;
            address &= ~insector_addr_mask;
            address ^= ctx->qspi_ctx->init.flash_size >> 1;
            address |= ctx->cmd_address & insector_addr_mask;
            ctx->cmd_address = address;
          }
          ctx->cmd_dupl_index++;
          switch(ctx->cmd_request) {
            case FLASH_CMD_READ:
              ctx->fsm_process = FLASH_FSM_READ_HEADER;
              break;
            case FLASH_CMD_WRITE:
              ctx->fsm_process = FLASH_FSM_ERASE;
              break;
            default:
              break;
          }
          continue;
        } else if(err != E_AGAIN) {
          ctx->cmd_errcode_internal = err;
          ctx->fsm_process = FLASH_FSM_FLASH_UNLOCK;
        }
        break;
      case FLASH_FSM_READ_HEADER:
        err = qspi_fast_read(ctx->qspi_ctx, ctx->cmd_address, &ctx->section_header, ECU_FLASH_SECTION_HEADER_LENGTH);
        if(err == E_OK) {
          ctx->fsm_process = FLASH_FSM_READ_HEADER_SYNC;
          continue;
        } else if(err != E_AGAIN) {
          ctx->cmd_errcode_internal = err;
          ctx->fsm_process = FLASH_FSM_FLASH_UNLOCK;
        }
        break;
      case FLASH_FSM_READ_HEADER_SYNC:
        err = qspi_sync(ctx->qspi_ctx);
        if(err == E_OK) {
          if(ctx->section_header.section_type != ctx->cmd_section_type || ctx->section_header.section_index != ctx->cmd_section_index) {
            ctx->fsm_process = FLASH_FSM_SECTION_ADDR;
            continue;
          }
          ctx->cmd_address += ECU_FLASH_SECTION_HEADER_LENGTH;
          ctx->fsm_process = FLASH_FSM_READ_PAYLOAD;
          continue;
        } else if(err != E_AGAIN) {
          ctx->cmd_errcode_internal = err;
          ctx->fsm_process = FLASH_FSM_FLASH_UNLOCK;
        }
        break;
      case FLASH_FSM_READ_PAYLOAD:
        err = qspi_fast_read(ctx->qspi_ctx, ctx->cmd_address, ctx->cmd_payload_rx, ctx->cmd_length);
        if(err == E_OK) {
          ctx->fsm_process = FLASH_FSM_READ_PAYLOAD_SYNC;
          continue;
        } else if(err != E_AGAIN) {
          ctx->cmd_errcode_internal = err;
          ctx->fsm_process = FLASH_FSM_FLASH_UNLOCK;
        }
        break;
      case FLASH_FSM_READ_PAYLOAD_SYNC:
        err = qspi_sync(ctx->qspi_ctx);
        if(err == E_OK) {
          ctx->fsm_process = FLASH_FSM_READ_CHECKSUM_LOCK;
          continue;
        } else if(err != E_AGAIN) {
          ctx->cmd_errcode_internal = err;
          ctx->fsm_process = FLASH_FSM_FLASH_UNLOCK;
        }
        break;
      case FLASH_FSM_READ_CHECKSUM_LOCK:
        err = crc_lock();
        if(err == E_OK) {
          ctx->fsm_process = FLASH_FSM_READ_CHECKSUM_HEADER;
          continue;
        } else if(err != E_AGAIN) {
          ctx->cmd_errcode_internal = err;
          ctx->fsm_process = FLASH_FSM_FLASH_UNLOCK;
        }
        break;
      case FLASH_FSM_READ_CHECKSUM_HEADER:
        err = crc_calculate(&ctx->checksum, &ctx->section_header, OFFSETOF(flash_section_header_t, crc), true);
        if(err == E_OK) {
          ctx->fsm_process = FLASH_FSM_READ_CHECKSUM_PAYLOAD;
          continue;
        } else if(err != E_AGAIN) {
          ctx->cmd_errcode_internal = err;
          ctx->fsm_process = FLASH_FSM_READ_CHECKSUM_UNLOCK;
        }
        break;
      case FLASH_FSM_READ_CHECKSUM_PAYLOAD:
        err = crc_calculate(&ctx->checksum, &ctx->cmd_payload_rx, ctx->cmd_length, false);
        if(err == E_OK) {
          ctx->fsm_process = FLASH_FSM_READ_CHECKSUM_UNLOCK;
          continue;
        } else if(err != E_AGAIN) {
          ctx->cmd_errcode_internal = err;
          ctx->fsm_process = FLASH_FSM_READ_CHECKSUM_UNLOCK;
        }
        break;
      case FLASH_FSM_READ_CHECKSUM_UNLOCK:
        err = crc_unlock();
        if(err == E_OK) {
          if(ctx->cmd_errcode_internal == E_AGAIN) {
            ctx->fsm_process = FLASH_FSM_READ_VERIFY;
          } else {
            ctx->fsm_process = FLASH_FSM_FLASH_UNLOCK;
          }
          continue;
        } else if(err != E_AGAIN) {
          ctx->cmd_errcode_internal = err;
          ctx->fsm_process = FLASH_FSM_FLASH_UNLOCK;
        }
        break;
      case FLASH_FSM_READ_VERIFY:
        if(ctx->checksum != ctx->section_header.crc) {
          ctx->fsm_process = FLASH_FSM_SECTION_ADDR;
          continue;
        } else if(ctx->section_header.section_type != ctx->cmd_section_type || ctx->section_header.section_index != ctx->cmd_section_index) {
          ctx->fsm_process = FLASH_FSM_SECTION_ADDR;
          continue;
        }
        if(err != E_AGAIN) {
          ctx->cmd_errcode_internal = err;
          ctx->fsm_process = FLASH_FSM_FLASH_UNLOCK;
        }
        break;
      case FLASH_FSM_FLASH_UNLOCK:
        err = qspi_unlock(ctx->qspi_ctx);
        if(err != E_AGAIN) {
          if(ctx->cmd_errcode_internal != E_AGAIN) {
            err = ctx->cmd_errcode_internal;
          }
          ctx->cmd_errcode = err;
          ctx->fsm_process = FLASH_FSM_CONDITION;
        }
        break;
      default:
        break;
    }
    break;
  }

  return err;
}
