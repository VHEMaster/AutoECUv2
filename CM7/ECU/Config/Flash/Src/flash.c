/*
 * flash.c
 *
 *  Created on: May 4, 2024
 *      Author: VHEMaster
 */

#include "flash.h"
#include "flash_fsm.h"
#include "flash_memory_layout.h"
#include "compiler.h"
#include "bool.h"

#include <string.h>

static flash_runtime_ctx_t flash_runtime_ctx = {0};
static BUFFER_DMA ALIGNED_CACHE flash_runtime_dma_ctx_t flash_runtime_dma_ctx = {0};

error_t flash_init(void)
{
  flash_runtime_ctx_t *ctx = &flash_runtime_ctx;
  error_t err = E_OK;

  do {
    memset(ctx, 0u, sizeof(flash_runtime_ctx_t));
    ctx->dma_ctx = &flash_runtime_dma_ctx;

    err = ecu_devices_get_flash_ctx(ECU_DEVICE_FLASH_1, &ctx->qspi_ctx);
    BREAK_IF(err != E_OK);

    err = flash_mem_layout_init();
    BREAK_IF(err != E_OK);

    ctx->ready = true;

  } while(0);

  return err;
}

ITCM_FUNC void flash_loop_fast(void)
{
  flash_runtime_ctx_t *ctx = &flash_runtime_ctx;
  error_t err = E_OK;

  do {
    if(ctx->ready) {
      err = flash_fsm(ctx);
      if(err != E_OK && err != E_AGAIN) {
        //TODO: set error in future
      }
    }
  } while(0);
}

error_t flash_lock(void)
{
  flash_runtime_ctx_t *ctx = &flash_runtime_ctx;
  error_t err = E_OK;
  uint32_t prim;

  do {
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    prim = EnterCritical();
    if(ctx->locked == true) {
      err = E_AGAIN;
    } else {
      ctx->locked = true;
      err = E_OK;
    }
    ExitCritical(prim);

  } while(0);

  return err;
}

error_t flash_unlock(void)
{
  flash_runtime_ctx_t *ctx = &flash_runtime_ctx;
  error_t err = E_OK;
  uint32_t prim;

  do {
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);
    prim = EnterCritical();
    if(ctx->locked == false || ctx->cmd_request != FLASH_CMD_NONE) {
      err = E_INVALACT;
    } else {
      ctx->locked = false;
      err = E_OK;
    }
    ExitCritical(prim);

  } while(0);

  return err;
}

error_t flash_section_read(uint16_t section_type, uint16_t section_index, flash_payload_version_t *payload_version, void *payload, uint32_t length)
{
  flash_runtime_ctx_t *ctx = &flash_runtime_ctx;
  error_t err = E_AGAIN;
  flash_cmd_type_t cmd = FLASH_CMD_READ;

  do {
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);
    BREAK_IF_ACTION(ctx->locked == false, err = E_INVALACT);
    BREAK_IF_ACTION(ctx->cmd_request != FLASH_CMD_NONE && ctx->cmd_request != cmd, err = E_INVALACT);

    if(ctx->cmd_request == FLASH_CMD_NONE) {
      ctx->cmd_section_type = section_type;
      ctx->cmd_section_index = section_index;
      ctx->cmd_payload_version_p = payload_version;
      ctx->cmd_payload_rx = payload;
      ctx->cmd_length = length;

      ctx->cmd_request = cmd;
      ctx->cmd_errcode = E_AGAIN;
    }
    if(ctx->cmd_errcode != E_AGAIN) {
      err = ctx->cmd_errcode;
      ctx->cmd_request = FLASH_CMD_NONE;
    }

  } while(0);

  return err;
}

error_t flash_section_write(uint16_t section_type, uint16_t section_index, flash_payload_version_t payload_version, const void *payload, uint32_t length)
{
  flash_runtime_ctx_t *ctx = &flash_runtime_ctx;
  error_t err = E_AGAIN;
  flash_cmd_type_t cmd = FLASH_CMD_WRITE;

  do {
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);
    BREAK_IF_ACTION(ctx->locked == false, err = E_INVALACT);
    BREAK_IF_ACTION(ctx->cmd_request != FLASH_CMD_NONE && ctx->cmd_request != cmd, err = E_INVALACT);

    if(ctx->cmd_request == FLASH_CMD_NONE) {
      ctx->cmd_section_type = section_type;
      ctx->cmd_section_index = section_index;
      ctx->cmd_payload_version = payload_version;
      ctx->cmd_payload_tx = payload;
      ctx->cmd_length = length;

      ctx->cmd_request = cmd;
      ctx->cmd_errcode = E_AGAIN;
    }
    if(ctx->cmd_errcode != E_AGAIN) {
      err = ctx->cmd_errcode;
      ctx->cmd_request = FLASH_CMD_NONE;
    }

  } while(0);

  return err;
}

error_t flash_mem_base_address(uint32_t *base_address)
{
  flash_runtime_ctx_t *ctx = &flash_runtime_ctx;
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);
    BREAK_IF_ACTION(base_address == NULL, err = E_PARAM);

    *base_address = ctx->qspi_ctx->init.memory_base_address;

  } while(0);

  return err;
}

error_t flash_mem_section_address(uint32_t *section_address, uint16_t section_type, uint16_t section_index)
{
  flash_runtime_ctx_t *ctx = &flash_runtime_ctx;
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);
    BREAK_IF_ACTION(section_address == NULL, err = E_PARAM);

    err = flash_mem_layout_get_section_address(section_address, section_type, section_index);

  } while(0);

  return err;
}


error_t flash_mem_header_len(uint32_t *header_len)
{
  flash_runtime_ctx_t *ctx = &flash_runtime_ctx;
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);
    BREAK_IF_ACTION(header_len == NULL, err = E_PARAM);

    *header_len = ECU_FLASH_SECTION_HEADER_LENGTH;

  } while(0);

  return err;
}
