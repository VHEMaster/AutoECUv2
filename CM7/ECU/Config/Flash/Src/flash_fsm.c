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
#include <string.h>

error_t flash_fsm(flash_runtime_ctx_t *ctx)
{
  error_t err = E_OK;
  uint32_t insector_addr_mask;
  uint32_t address;
  bool compare_success;

  while(true) {
    switch(ctx->fsm_process) {
      case FLASH_FSM_CONDITION:
        if(ctx->cmd_errcode == E_AGAIN) {
          if(ctx->ready == true  && ctx->cmd_request != FLASH_CMD_NONE) {
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
        err = flash_mem_layout_get_section_info(&ctx->cmd_section, ctx->cmd_section_type, ctx->cmd_section_index);
        if(err == E_OK) {
          ctx->cmd_erase_type = ctx->cmd_section->uses_full_block ? FLASH_ERASE_TYPE_BLOCK : FLASH_ERASE_TYPE_SECTOR;
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
          ctx->cmd_address_base = address;

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
            ctx->cmd_address_base = address;
            ctx->cmd_payload_tx_temp = ctx->cmd_payload_tx;
          }
          switch(ctx->cmd_request) {
            case FLASH_CMD_READ:
              ctx->fsm_process = FLASH_FSM_READ_HEADER;
              break;
            case FLASH_CMD_WRITE:
              ctx->cmd_left_len = ctx->cmd_length + ECU_FLASH_SECTION_HEADER_LENGTH;

              if(ctx->cmd_dupl_index == 0) {
                ctx->fsm_process = FLASH_FSM_WRITE_CHECKSUM_LOCK;
              } else {
                ctx->fsm_process = FLASH_FSM_WRITE_ERASE;
              }
              break;
            default:
              break;
          }
          ctx->cmd_dupl_index++;
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
        err = crc_calculate(&ctx->checksum, ctx->cmd_payload_rx, ctx->cmd_length, false);
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
          if(ctx->cmd_payload_version_p != NULL) {
            *ctx->cmd_payload_version_p = ctx->section_header.payload_version;
          }
          ctx->cmd_errcode_internal = err;
          ctx->fsm_process = FLASH_FSM_FLASH_UNLOCK;
        }
        break;

      case FLASH_FSM_WRITE_CHECKSUM_LOCK:
        err = crc_lock();
        if(err == E_OK) {
          memset(&ctx->section_header, 0u, sizeof(ctx->section_header));
          ctx->section_header.section_type = ctx->cmd_section_type;
          ctx->section_header.section_index = ctx->cmd_section_index;
          ctx->section_header.payload_version = ctx->cmd_payload_version;
          ctx->section_header.pages = ctx->cmd_section->section_length / ctx->qspi_ctx->init.prog_page_size;
          ctx->section_header.crc = 0;

          ctx->fsm_process = FLASH_FSM_WRITE_CHECKSUM_HEADER;
          continue;
        } else if(err != E_AGAIN) {
          ctx->cmd_errcode_internal = err;
          ctx->fsm_process = FLASH_FSM_FLASH_UNLOCK;
        }
        break;
      case FLASH_FSM_WRITE_CHECKSUM_HEADER:
        err = crc_calculate(&ctx->checksum, &ctx->section_header, OFFSETOF(flash_section_header_t, crc), true);
        if(err == E_OK) {
          ctx->fsm_process = FLASH_FSM_WRITE_CHECKSUM_PAYLOAD;
          continue;
        } else if(err != E_AGAIN) {
          ctx->cmd_errcode_internal = err;
          ctx->fsm_process = FLASH_FSM_WRITE_CHECKSUM_UNLOCK;
        }
        break;
      case FLASH_FSM_WRITE_CHECKSUM_PAYLOAD:
        err = crc_calculate(&ctx->checksum, ctx->cmd_payload_tx, ctx->cmd_length, false);
        if(err == E_OK) {
          ctx->section_header.crc = ctx->checksum;
          ctx->fsm_process = FLASH_FSM_WRITE_CHECKSUM_UNLOCK;
          continue;
        } else if(err != E_AGAIN) {
          ctx->cmd_errcode_internal = err;
          ctx->fsm_process = FLASH_FSM_WRITE_CHECKSUM_UNLOCK;
        }
        break;
      case FLASH_FSM_WRITE_CHECKSUM_UNLOCK:
        err = crc_unlock();
        if(err == E_OK) {
          if(ctx->cmd_errcode_internal == E_AGAIN) {
            ctx->fsm_process = FLASH_FSM_WRITE_ERASE;
          } else {
            ctx->fsm_process = FLASH_FSM_FLASH_UNLOCK;
          }
          continue;
        } else if(err != E_AGAIN) {
          ctx->cmd_errcode_internal = err;
          ctx->fsm_process = FLASH_FSM_FLASH_UNLOCK;
        }
        break;

      case FLASH_FSM_WRITE_ERASE:
        if(ctx->cmd_erase_type == FLASH_ERASE_TYPE_NONE) {
          ctx->fsm_process = FLASH_FSM_WRITE_HEADER;
          continue;
        } else if(ctx->cmd_erase_type == FLASH_ERASE_TYPE_SECTOR) {
          err = qspi_sector_erase(ctx->qspi_ctx, ctx->cmd_address);
          ctx->cmd_cur_len = MIN(ctx->cmd_left_len, ctx->qspi_ctx->init.erase_sector_size);
        } else if(ctx->cmd_erase_type == FLASH_ERASE_TYPE_BLOCK) {
          err = qspi_block_erase(ctx->qspi_ctx, ctx->cmd_address);
        } else {
          err = E_INVALACT;
        }

        if(err == E_OK) {
          ctx->fsm_process = FLASH_FSM_WRITE_ERASE_SYNC;
          continue;
        } else if(err != E_AGAIN) {
          ctx->cmd_errcode_internal = err;
          ctx->fsm_process = FLASH_FSM_FLASH_UNLOCK;
        }
        break;
      case FLASH_FSM_WRITE_ERASE_SYNC:
        err = qspi_sync(ctx->qspi_ctx);
        if(err == E_OK) {
          if(ctx->cmd_erase_type == FLASH_ERASE_TYPE_SECTOR) {
            ctx->cmd_address += ctx->cmd_cur_len;
            ctx->cmd_left_len -= ctx->cmd_cur_len;
            ctx->cmd_cur_len = MIN(ctx->cmd_left_len, ctx->qspi_ctx->init.erase_sector_size);
            if(ctx->cmd_cur_len > 0) {
              ctx->fsm_process = FLASH_FSM_WRITE_ERASE;
              continue;
            } else {
              ctx->fsm_process = FLASH_FSM_WRITE_HEADER;
            }
          } else {
            ctx->fsm_process = FLASH_FSM_WRITE_HEADER;
          }
          ctx->cmd_address = ctx->cmd_address_base;
          ctx->cmd_payload_tx_temp = ctx->cmd_payload_tx;
          ctx->cmd_left_len = ctx->cmd_length + ECU_FLASH_SECTION_HEADER_LENGTH;
          ctx->cmd_cur_len = MIN(ctx->cmd_left_len, ctx->qspi_ctx->init.prog_page_size);

          continue;
        } else if(err != E_AGAIN) {
          ctx->cmd_errcode_internal = err;
          ctx->fsm_process = FLASH_FSM_FLASH_UNLOCK;
        }
        break;
      case FLASH_FSM_WRITE_HEADER:
        err = qspi_page_write(ctx->qspi_ctx, ctx->cmd_address, &ctx->section_header, ECU_FLASH_SECTION_HEADER_LENGTH);
        if(err == E_OK) {
          ctx->fsm_process = FLASH_FSM_WRITE_HEADER_SYNC;
          continue;
        } else if(err != E_AGAIN) {
          ctx->cmd_errcode_internal = err;
          ctx->fsm_process = FLASH_FSM_FLASH_UNLOCK;
        }
        break;
      case FLASH_FSM_WRITE_HEADER_SYNC:
        err = qspi_sync(ctx->qspi_ctx);
        if(err == E_OK) {
          ctx->fsm_process = FLASH_FSM_WRITE_HEADER_VERIFY;
          continue;
        } else if(err != E_AGAIN) {
          ctx->cmd_errcode_internal = err;
          ctx->fsm_process = FLASH_FSM_FLASH_UNLOCK;
        }
        break;
      case FLASH_FSM_WRITE_HEADER_VERIFY:
        err = qspi_fast_read(ctx->qspi_ctx, ctx->cmd_address, ctx->cmd_write_verify, ECU_FLASH_SECTION_HEADER_LENGTH);
        if(err == E_OK) {
          ctx->fsm_process = FLASH_FSM_WRITE_HEADER_VERIFY_SYNC;
          continue;
        } else if(err != E_AGAIN) {
          ctx->cmd_errcode_internal = err;
          ctx->fsm_process = FLASH_FSM_FLASH_UNLOCK;
        }
        break;
      case FLASH_FSM_WRITE_HEADER_VERIFY_SYNC:
        err = qspi_sync(ctx->qspi_ctx);
        if(err == E_OK) {
          compare_success = memcmp(&ctx->section_header, ctx->cmd_write_verify, ECU_FLASH_SECTION_HEADER_LENGTH) == 0;
          if(compare_success == true) {
            ctx->cmd_left_len -= ECU_FLASH_SECTION_HEADER_LENGTH;
            ctx->cmd_cur_len -= ECU_FLASH_SECTION_HEADER_LENGTH;
            ctx->cmd_address += ECU_FLASH_SECTION_HEADER_LENGTH;
            ctx->fsm_process = FLASH_FSM_WRITE_PAYLOAD;
          } else {
            err = E_BADRESP;
            ctx->cmd_errcode_internal = err;
            ctx->fsm_process = FLASH_FSM_FLASH_UNLOCK;
          }
          continue;
        } else if(err != E_AGAIN) {
          ctx->cmd_errcode_internal = err;
          ctx->fsm_process = FLASH_FSM_FLASH_UNLOCK;
        }
        break;
      case FLASH_FSM_WRITE_PAYLOAD:
        err = qspi_page_write(ctx->qspi_ctx, ctx->cmd_address, ctx->cmd_payload_tx_temp, ctx->cmd_cur_len);
        if(err == E_OK) {
          ctx->fsm_process = FLASH_FSM_WRITE_PAYLOAD_SYNC;
          continue;
        } else if(err != E_AGAIN) {
          ctx->cmd_errcode_internal = err;
          ctx->fsm_process = FLASH_FSM_FLASH_UNLOCK;
        }
        break;
      case FLASH_FSM_WRITE_PAYLOAD_SYNC:
        err = qspi_sync(ctx->qspi_ctx);
        if(err == E_OK) {
          ctx->fsm_process = FLASH_FSM_WRITE_PAYLOAD_VERIFY;
          continue;
        } else if(err != E_AGAIN) {
          ctx->cmd_errcode_internal = err;
          ctx->fsm_process = FLASH_FSM_FLASH_UNLOCK;
        }
        break;
      case FLASH_FSM_WRITE_PAYLOAD_VERIFY:
        err = qspi_fast_read(ctx->qspi_ctx, ctx->cmd_address, ctx->cmd_write_verify, ctx->cmd_cur_len);
        if(err == E_OK) {
          ctx->fsm_process = FLASH_FSM_WRITE_PAYLOAD_VERIFY_SYNC;
          continue;
        } else if(err != E_AGAIN) {
          ctx->cmd_errcode_internal = err;
          ctx->fsm_process = FLASH_FSM_FLASH_UNLOCK;
        }
        break;
      case FLASH_FSM_WRITE_PAYLOAD_VERIFY_SYNC:
        err = qspi_sync(ctx->qspi_ctx);
        if(err == E_OK) {
          compare_success = memcmp(ctx->cmd_payload_tx_temp, ctx->cmd_write_verify, ctx->cmd_cur_len) == 0;
          if(compare_success == true) {
            ctx->cmd_payload_tx_temp += ctx->cmd_cur_len;
            ctx->cmd_address += ctx->cmd_cur_len;
            ctx->cmd_left_len -= ctx->cmd_cur_len;
            ctx->cmd_cur_len = ctx->cmd_left_len > ctx->qspi_ctx->init.prog_page_size ? ctx->qspi_ctx->init.prog_page_size : ctx->cmd_left_len;
            if(ctx->cmd_cur_len > 0) {
              ctx->fsm_process = FLASH_FSM_WRITE_PAYLOAD;
            } else {
              ctx->fsm_process = FLASH_FSM_SECTION_ADDR;
            }
          } else {
            err = E_BADRESP;
            ctx->cmd_errcode_internal = err;
            ctx->fsm_process = FLASH_FSM_FLASH_UNLOCK;
          }
          continue;
        } else if(err != E_AGAIN) {
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
