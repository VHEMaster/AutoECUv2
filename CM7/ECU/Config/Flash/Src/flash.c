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

error_t flash_init(void)
{
  flash_runtime_ctx_t *ctx = &flash_runtime_ctx;
  error_t err = E_OK;

  do {
    memset(&flash_runtime_ctx, 0u, sizeof(flash_runtime_ctx));;

    err = ecu_devices_get_flash_ctx(ECU_DEVICE_FLASH_1, &ctx->qspi_ctx);
    BREAK_IF(err != E_OK);

    err = flash_mem_layout_init();
    BREAK_IF(err != E_OK)

    ctx->ready = true;

  } while(0);

  return err;
}

void flash_loop_fast(void)
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

error_t flash_io_lock(void)
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

error_t flash_io_unlock(void)
{
  flash_runtime_ctx_t *ctx = &flash_runtime_ctx;
  error_t err = E_OK;
  uint32_t prim;

  do {
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

error_t flash_io_read(uint32_t address, void *payload, uint32_t length)
{
  flash_runtime_ctx_t *ctx = &flash_runtime_ctx;
  error_t err = E_OK;
  flash_cmd_type_t cmd = FLASH_CMD_READ;

  do {
    BREAK_IF_ACTION(ctx->locked == false, err = E_INVALACT);
    BREAK_IF_ACTION(ctx->cmd_request != FLASH_CMD_NONE && ctx->cmd_request != cmd, err = E_INVALACT);

    if(ctx->cmd_request == FLASH_CMD_NONE) {
      ctx->cmd_address = address;
      ctx->cmd_payload_rx = payload;
      ctx->cmd_length = length;

      ctx->cmd_request = cmd;
      ctx->cmd_errcode = E_AGAIN;
    }
    if(ctx->cmd_errcode != E_AGAIN) {
      err = ctx->cmd_errcode;
      ctx->cmd_request = FLASH_CMD_NONE;
    } else {
      err = E_AGAIN;
    }

  } while(0);

  return err;
}

error_t flash_io_write(flash_erase_type_t erase_type, uint32_t address, const void *payload, uint32_t length)
{
  flash_runtime_ctx_t *ctx = &flash_runtime_ctx;
  error_t err = E_OK;
  flash_cmd_type_t cmd = FLASH_CMD_WRITE;

  do {
    BREAK_IF_ACTION(ctx->locked == false, err = E_INVALACT);
    BREAK_IF_ACTION(ctx->cmd_request != FLASH_CMD_NONE && ctx->cmd_request != cmd, err = E_INVALACT);

    if(ctx->cmd_request == FLASH_CMD_NONE) {
      ctx->cmd_address = address;
      ctx->cmd_payload_tx = payload;
      ctx->cmd_length = length;
      ctx->cmd_erase_type = erase_type;

      ctx->cmd_request = cmd;
      ctx->cmd_errcode = E_AGAIN;
    }
    if(ctx->cmd_errcode != E_AGAIN) {
      err = ctx->cmd_errcode;
      ctx->cmd_request = FLASH_CMD_NONE;
    } else {
      err = E_AGAIN;
    }

  } while(0);

  return err;
}
