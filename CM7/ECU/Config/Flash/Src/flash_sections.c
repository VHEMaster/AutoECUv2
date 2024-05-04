/*
 * flash_sections.c
 *
 *  Created on: May 3, 2024
 *      Author: VHEMaster
 */

#include "flash_sections.h"
#include "flash_memory_layout.h"
#include "config_devices.h"
#include "bool.h"
#include "compiler.h"

typedef enum {
  FLASH_SECTION_FSM_FIND_MEM_LAYOUT = 0,
  FLASH_SECTION_FSM_FIND_QSPI_ACQUIRE,
  FLASH_SECTION_FSM_FIND_QSPI_LOCK,
  FLASH_SECTION_FSM_FIND_QSPI_READ,
  FLASH_SECTION_FSM_FIND_READ_RESULT,
  FLASH_SECTION_FSM_FIND_QSPI_UNLOCK,
}flash_sections_fsm_find_t;

typedef enum {
  FLASH_SECTION_FSM_ALLOCATE_QSPI_ACQUIRE = 0,
}flash_sections_fsm_allocate_t;

typedef struct {
    flash_section_header_t section_header;
    const flash_mem_layout_t *mem_layout;
    qspi_ctx_t *qspi_ctx;
    flash_sections_fsm_find_t fsm_find;
    flash_sections_fsm_allocate_t fsm_allocate;

    uint32_t section_find_block_index;
    uint32_t section_find_sector_index;

    error_t errcode;
    bool locked;
}flash_sections_ctx_t;

static flash_sections_ctx_t flash_sections_ctx = {0};

error_t flash_section_lock()
{
  error_t err = E_OK;
  uint32_t prim;
  flash_sections_ctx_t *ctx = &flash_sections_ctx;

  do {
    prim = EnterCritical();
    if(ctx->locked == true) {
      err = E_AGAIN;
    } else {
      ctx->locked = true;
      ctx->fsm_find = 0;
      ctx->fsm_allocate = 0;
      err = E_OK;
    }
    ExitCritical(prim);

  } while(0);

  return err;
}

error_t flash_section_unlock()
{
  error_t err = E_OK;
  uint32_t prim;
  flash_sections_ctx_t *ctx = &flash_sections_ctx;

  do {
    prim = EnterCritical();
    if(ctx->locked == false) {
      err = E_INVALACT;
    } else {
      ctx->locked = false;
      err = E_OK;
    }
    ExitCritical(prim);

  } while(0);

  return err;
}

error_t flash_section_find(flash_section_info_t *info)
{
  error_t err = E_OK;
  flash_sections_ctx_t *ctx = &flash_sections_ctx;

  while(true) {
    BREAK_IF_ACTION(ctx->locked != true, err = E_INVALACT);
    err = E_AGAIN;

    switch(ctx->fsm_find) {
      case FLASH_SECTION_FSM_FIND_MEM_LAYOUT:
        ctx->errcode = E_AGAIN;
        err = flash_mem_layout_get(&ctx->mem_layout);
        if(err == E_OK) {
          ctx->fsm_find = FLASH_SECTION_FSM_FIND_QSPI_ACQUIRE;
          err = E_AGAIN;
          continue;
        } else if(err != E_AGAIN) {
          ctx->fsm_find = 0;
          break;
        }
        break;
      case FLASH_SECTION_FSM_FIND_QSPI_ACQUIRE:
        err = ecu_devices_get_flash_ctx(ECU_DEVICE_FLASH_1, &ctx->qspi_ctx);
        if(err == E_OK) {
          ctx->fsm_find = FLASH_SECTION_FSM_FIND_QSPI_LOCK;
          err = E_AGAIN;
          continue;
        } else if(err != E_AGAIN) {
          ctx->fsm_find = 0;
          break;
        }
        break;
      case FLASH_SECTION_FSM_FIND_QSPI_LOCK:
        err = qspi_lock(ctx->qspi_ctx);
        if(err == E_OK) {
          ctx->fsm_find = FLASH_SECTION_FSM_FIND_QSPI_READ;
          info->address;
          ctx->section_find_block_index = 0;
          ctx->section_find_sector_index = 0;
          err = E_AGAIN;
          continue;
        } else if(err != E_AGAIN) {
          ctx->fsm_find = 0;
          break;
        }
        break;
      case FLASH_SECTION_FSM_FIND_QSPI_READ:
        err = qspi_fast_read(ctx->qspi_ctx, info->address, &ctx->section_header, sizeof(ctx->section_header));
        if(err == E_OK) {
          ctx->fsm_find = FLASH_SECTION_FSM_FIND_READ_RESULT;
          err = E_AGAIN;
          continue;
        } else if(err != E_AGAIN) {
          ctx->errcode = err;
          ctx->fsm_find = FLASH_SECTION_FSM_FIND_QSPI_UNLOCK;
          break;
        }
        break;
      case FLASH_SECTION_FSM_FIND_READ_RESULT:

        break;
      case FLASH_SECTION_FSM_FIND_QSPI_UNLOCK:
        err = qspi_unlock(ctx->qspi_ctx);
        if(err != E_AGAIN) {
          if(err == E_OK) {
            err = ctx->errcode;
          }
          ctx->fsm_find = 0;
          break;
        }
        break;
      default:
        break;
    }

    break;
  }

  return err;
}

error_t flash_section_allocate(flash_section_info_t *info)
{
  error_t err = E_OK;

  return err;
}
