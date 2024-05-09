/*
 * qspi.c
 *
 *  Created on: Apr 25, 2024
 *      Author: VHEMaster
 */

#include "qspi.h"
#include "qspi_fsm.h"
#include "compiler.h"
#include <string.h>

void qspi_loop_main(qspi_ctx_t *ctx)
{

}

void qspi_loop_slow(qspi_ctx_t *ctx)
{

}

void qspi_loop_fast(qspi_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    err = qspi_fsm(ctx);
    if(err != E_OK && err != E_AGAIN) {
      //TODO: set DTC here?
    }

  } while(0);
}

void qspi_cplt_cb(qspi_ctx_t *ctx)
{
  if(ctx != NULL && ctx->ready == true) {
    ctx->cmd_async_errcode = E_OK;
  }
}

void qspi_err_cb(qspi_ctx_t *ctx)
{
  if(ctx != NULL && ctx->ready == true) {
    ctx->cmd_async_errcode = E_IO;
  }
}

void qspi_timeout_cb(qspi_ctx_t *ctx)
{
  if(ctx != NULL && ctx->ready == true) {
    ctx->cmd_async_errcode = E_TIMEOUT;
  }
}

error_t qspi_init(qspi_ctx_t *ctx, const qspi_init_t *init_ctx)
{
  error_t err = E_OK;

  do {
    memset(ctx, 0, sizeof(qspi_ctx_t));
    memcpy(&ctx->init, init_ctx, sizeof(qspi_init_t));

    ctx->cmd_poll.Match = 0;
    ctx->cmd_poll.Mask = QSPI_STATUS_REG_BUSY | QSPI_STATUS_REG_WEL;
    ctx->cmd_poll.Mask |= ctx->cmd_poll.Mask << 8;
    ctx->cmd_poll.Interval = QSPI_STATUS_POLL_INTERVAL;
    ctx->cmd_poll.StatusBytesSize = 2;
    ctx->cmd_poll.MatchMode = QSPI_MATCH_MODE_AND;
    ctx->cmd_poll.AutomaticStop = QSPI_AUTOMATIC_STOP_ENABLE;

    ctx->ready = true;

  } while(0);

  return err;
}

error_t qspi_reset(qspi_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    memset(&ctx->jedec_quad, 0, sizeof(ctx->jedec_quad));
    memset(&ctx->jedec, 0, sizeof(ctx->jedec));
    ctx->jedec_ready = false;
    ctx->initialized = false;
    ctx->init_errcode = E_AGAIN;

  } while(0);

  return err;
}

error_t qspi_lock(qspi_ctx_t *ctx)
{
  error_t err = E_OK;
  uint32_t prim;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false || ctx->initialized == false, err = E_NOTRDY);

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

error_t qspi_unlock(qspi_ctx_t *ctx)
{
  error_t err = E_OK;
  uint32_t prim;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->init_errcode != E_OK, err = ctx->init_errcode);

    prim = EnterCritical();
    if(ctx->locked == false || ctx->cmd_ready == true) {
      err = E_INVALACT;
    } else {
      ctx->locked = false;
      err = E_OK;
    }
    ExitCritical(prim);

  } while(0);

  return err;
}

error_t qspi_fast_read(qspi_ctx_t *ctx, uint32_t address, void *payload, uint32_t length)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || payload == NULL || length == 0, err = E_PARAM);
    BREAK_IF_ACTION((address + length > ctx->init.flash_size), err = E_PARAM);
    BREAK_IF_ACTION((length & (ctx->init.flash_dies_count - 1)), err = E_PARAM);
    BREAK_IF_ACTION(ctx->init_errcode != E_OK, err = ctx->init_errcode);
    BREAK_IF_ACTION(ctx->locked == false, err = E_INVALACT);
    BREAK_IF_ACTION(ctx->cmd_ready == true, err = E_INVALACT);

    ctx->cmd_ptr = &ctx->init.cmd_hsread;
    ctx->cmd_ptr->Address = address;
    ctx->cmd_ptr->NbData = length;
    ctx->cmd_payload_tx = NULL;
    ctx->cmd_payload_rx = payload;
    ctx->cmd_wren_needed = false;

    ctx->cmd_status_poll_needed = false;

    ctx->cmd_errcode = E_AGAIN;
    ctx->cmd_ready = true;

    err = E_OK;

  } while(0);

  return err;
}

error_t qspi_page_write(qspi_ctx_t *ctx, uint32_t address, const void *payload, uint32_t length)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || payload == NULL || length == 0, err = E_PARAM);
    BREAK_IF_ACTION(((address & (ctx->init.prog_page_size - 1)) + length > ctx->init.prog_page_size), err = E_PARAM);
    BREAK_IF_ACTION((address + length > ctx->init.flash_size), err = E_PARAM);
    BREAK_IF_ACTION(ctx->init_errcode != E_OK, err = ctx->init_errcode);
    BREAK_IF_ACTION(ctx->locked == false, err = E_INVALACT);
    BREAK_IF_ACTION(ctx->cmd_ready == true, err = E_INVALACT);

    ctx->cmd_ptr = &ctx->init.cmd_prog;
    ctx->cmd_ptr->Address = address;
    ctx->cmd_ptr->NbData = length;
    ctx->cmd_payload_tx = payload;
    ctx->cmd_payload_rx = NULL;
    ctx->cmd_wren_needed = true;

    ctx->cmd_status_poll_needed = true;
    ctx->cmd_poll_delay = ctx->init.delay_program_time;
    ctx->cmd_poll_delay *= length;
    ctx->cmd_poll_delay /= ctx->init.prog_page_size;
    ctx->cmd_poll_timeout = ctx->init.timeout_program_time;

    ctx->cmd_errcode = E_AGAIN;
    ctx->cmd_ready = true;

    err = E_OK;

  } while(0);

  return err;
}

error_t qspi_sector_erase(qspi_ctx_t *ctx, uint32_t address)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION((address > ctx->init.flash_size), err = E_PARAM);
    BREAK_IF_ACTION((address & (ctx->init.erase_sector_size - 1)), err = E_PARAM);
    BREAK_IF_ACTION(ctx->init_errcode != E_OK, err = ctx->init_errcode);
    BREAK_IF_ACTION(ctx->locked == false, err = E_INVALACT);
    BREAK_IF_ACTION(ctx->cmd_ready == true, err = E_INVALACT);

    ctx->cmd_ptr = &ctx->init.cmd_se;
    ctx->cmd_ptr->Address = address;
    ctx->cmd_wren_needed = true;

    ctx->cmd_status_poll_needed = true;
    ctx->cmd_poll_delay = ctx->init.delay_sector_erase;
    ctx->cmd_poll_timeout = ctx->init.timeout_sector_erase;

    ctx->cmd_errcode = E_AGAIN;
    ctx->cmd_ready = true;

    err = E_OK;

  } while(0);

  return err;
}

error_t qspi_block_erase(qspi_ctx_t *ctx, uint32_t address)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION((address > ctx->init.flash_size), err = E_PARAM);
    BREAK_IF_ACTION((address & (ctx->init.erase_sector_size - 1)), err = E_PARAM);
    BREAK_IF_ACTION(ctx->init_errcode != E_OK, err = ctx->init_errcode);
    BREAK_IF_ACTION(ctx->locked == false, err = E_INVALACT);
    BREAK_IF_ACTION(ctx->cmd_ready == true, err = E_INVALACT);

    ctx->cmd_ptr = &ctx->init.cmd_be;
    ctx->cmd_ptr->Address = address;
    ctx->cmd_wren_needed = true;

    ctx->cmd_status_poll_needed = true;
    ctx->cmd_poll_delay = ctx->init.delay_sector_erase;
    ctx->cmd_poll_timeout = ctx->init.timeout_sector_erase;

    ctx->cmd_errcode = E_AGAIN;
    ctx->cmd_ready = true;

    err = E_OK;

  } while(0);

  return err;
}

error_t qspi_chip_erase(qspi_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->init_errcode != E_OK, err = ctx->init_errcode);
    BREAK_IF_ACTION(ctx->locked == false, err = E_INVALACT);
    BREAK_IF_ACTION(ctx->cmd_ready == true, err = E_INVALACT);

    ctx->cmd_ptr = &ctx->init.cmd_ce;
    ctx->cmd_wren_needed = true;

    ctx->cmd_status_poll_needed = true;
    ctx->cmd_poll_delay = ctx->init.delay_chip_erase;
    ctx->cmd_poll_timeout = ctx->init.timeout_chip_erase;

    ctx->cmd_errcode = E_AGAIN;
    ctx->cmd_ready = true;

    err = E_OK;

  } while(0);

  return err;
}

error_t qspi_write_bpr(qspi_ctx_t *ctx, const uint8_t *bpr)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || bpr == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->init_errcode != E_OK, err = ctx->init_errcode);
    BREAK_IF_ACTION(ctx->locked == false, err = E_INVALACT);
    BREAK_IF_ACTION(ctx->cmd_ready == true, err = E_INVALACT);

    ctx->cmd_ptr = &ctx->init.cmd_wbpr;
    for(int i = 0; i < QSPI_BPR_SIZE; i++) {
      ctx->bpr[i] = bpr[i];
      ctx->payload_bpr[i * 2] = ctx->bpr[i];
      ctx->payload_bpr[i * 2 + 1] = ctx->bpr[i];
    }
    ctx->cmd_payload_tx = ctx->payload_bpr;

    ctx->cmd_wren_needed = true;

    ctx->cmd_status_poll_needed = true;
    ctx->cmd_poll_delay = 0;
    ctx->cmd_poll_timeout = QSPI_BPR_TIMEOUT_US;

    ctx->cmd_errcode = E_AGAIN;
    ctx->cmd_ready = true;

    err = E_OK;

  } while(0);

  return err;
}


error_t qspi_otp_read(qspi_ctx_t *ctx, uint32_t address, void *payload, uint32_t length)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || payload == NULL || length == 0, err = E_PARAM);
    BREAK_IF_ACTION((length & (ctx->init.flash_dies_count - 1)), err = E_PARAM);
    BREAK_IF_ACTION((address + length > ctx->init.otp_size), err = E_PARAM);
    BREAK_IF_ACTION(ctx->init_errcode != E_OK, err = ctx->init_errcode);
    BREAK_IF_ACTION(ctx->locked == false, err = E_INVALACT);
    BREAK_IF_ACTION(ctx->cmd_ready == true, err = E_INVALACT);

    ctx->cmd_ptr = &ctx->init.cmd_rsid;
    ctx->cmd_ptr->Address = address;
    ctx->cmd_ptr->NbData = length;
    ctx->cmd_payload_tx = NULL;
    ctx->cmd_payload_rx = payload;
    ctx->cmd_wren_needed = false;

    ctx->cmd_status_poll_needed = false;

    ctx->cmd_errcode = E_AGAIN;
    ctx->cmd_ready = true;

    err = E_OK;

  } while(0);

  return err;
}

error_t qspi_otp_write(qspi_ctx_t *ctx, uint32_t address, const void *payload, uint32_t length)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || payload == NULL || length == 0, err = E_PARAM);
    BREAK_IF_ACTION((length & (ctx->init.flash_dies_count - 1)), err = E_PARAM);
    BREAK_IF_ACTION((address + length > ctx->init.otp_size), err = E_PARAM);
    BREAK_IF_ACTION(ctx->init_errcode != E_OK, err = ctx->init_errcode);
    BREAK_IF_ACTION(ctx->locked == false, err = E_INVALACT);
    BREAK_IF_ACTION(ctx->cmd_ready == true, err = E_INVALACT);

    ctx->cmd_ptr = &ctx->init.cmd_psid;
    ctx->cmd_ptr->Address = address;
    ctx->cmd_ptr->NbData = length;
    ctx->cmd_payload_tx = payload;
    ctx->cmd_payload_rx = NULL;
    ctx->cmd_wren_needed = true;

    ctx->cmd_status_poll_needed = true;
    ctx->cmd_poll_timeout = ctx->init.timeout_program_time;

    ctx->cmd_errcode = E_AGAIN;
    ctx->cmd_ready = true;

    err = E_OK;

  } while(0);

  return err;
}

error_t qspi_otp_lock(qspi_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->init_errcode != E_OK, err = ctx->init_errcode);
    BREAK_IF_ACTION(ctx->locked == false, err = E_INVALACT);
    BREAK_IF_ACTION(ctx->cmd_ready == true, err = E_INVALACT);

    ctx->cmd_ptr = &ctx->init.cmd_lsid;
    ctx->cmd_wren_needed = true;

    ctx->cmd_status_poll_needed = true;
    ctx->cmd_poll_timeout = ctx->init.timeout_program_time;

    ctx->cmd_errcode = E_AGAIN;
    ctx->cmd_ready = true;

    err = E_OK;

  } while(0);

  return err;
}


error_t qspi_prot_read(qspi_ctx_t *ctx, void *payload, uint32_t length)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || payload == NULL || length == 0, err = E_PARAM);
    BREAK_IF_ACTION((length & (ctx->init.flash_dies_count - 1)), err = E_PARAM);
    BREAK_IF_ACTION((length < ctx->init.prot_size), err = E_PARAM);
    BREAK_IF_ACTION(ctx->init_errcode != E_OK, err = ctx->init_errcode);
    BREAK_IF_ACTION(ctx->locked == false, err = E_INVALACT);
    BREAK_IF_ACTION(ctx->cmd_ready == true, err = E_INVALACT);

    ctx->cmd_ptr = &ctx->init.cmd_rbpr;
    ctx->cmd_ptr->Address = 0;
    ctx->cmd_ptr->NbData = length;
    ctx->cmd_payload_tx = NULL;
    ctx->cmd_payload_rx = payload;
    ctx->cmd_wren_needed = true;

    ctx->cmd_status_poll_needed = false;

    ctx->cmd_errcode = E_AGAIN;
    ctx->cmd_ready = true;

    err = E_OK;

  } while(0);

  return err;
}

error_t qspi_prot_write(qspi_ctx_t *ctx, const void *payload, uint32_t length)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || payload == NULL || length == 0, err = E_PARAM);
    BREAK_IF_ACTION((length & (ctx->init.flash_dies_count - 1)), err = E_PARAM);
    BREAK_IF_ACTION((length < ctx->init.prot_size), err = E_PARAM);
    BREAK_IF_ACTION(ctx->init_errcode != E_OK, err = ctx->init_errcode);
    BREAK_IF_ACTION(ctx->locked == false, err = E_INVALACT);
    BREAK_IF_ACTION(ctx->cmd_ready == true, err = E_INVALACT);

    ctx->cmd_ptr = &ctx->init.cmd_wbpr;
    ctx->cmd_ptr->Address = 0;
    ctx->cmd_ptr->NbData = length;
    ctx->cmd_payload_tx = payload;
    ctx->cmd_payload_rx = NULL;
    ctx->cmd_wren_needed = true;

    ctx->cmd_status_poll_needed = true;
    ctx->cmd_poll_timeout = ctx->init.timeout_program_time;

    ctx->cmd_errcode = E_AGAIN;
    ctx->cmd_ready = true;

    err = E_OK;

  } while(0);

  return err;
}

error_t qspi_prot_lock(qspi_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->init_errcode != E_OK, err = ctx->init_errcode);
    BREAK_IF_ACTION(ctx->locked == false, err = E_INVALACT);
    BREAK_IF_ACTION(ctx->cmd_ready == true, err = E_INVALACT);

    ctx->cmd_ptr = &ctx->init.cmd_lbpr;
    ctx->cmd_wren_needed = true;

    ctx->cmd_status_poll_needed = true;
    ctx->cmd_poll_timeout = ctx->init.timeout_program_time;

    ctx->cmd_errcode = E_AGAIN;
    ctx->cmd_ready = true;

    err = E_OK;

  } while(0);

  return err;
}

error_t qspi_sync(qspi_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->init_errcode != E_OK, err = ctx->init_errcode);
    BREAK_IF_ACTION(ctx->locked == false, err = E_INVALACT);
    BREAK_IF_ACTION(ctx->cmd_ready == false, err = E_INVALACT);

    err = ctx->cmd_errcode;
    if(err != E_AGAIN) {
      ctx->cmd_ready = false;
    }

  } while(0);

  return err;
}

error_t qspi_get_jedec(qspi_ctx_t *ctx, qspi_jedec_t *jedec)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || jedec == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->init_errcode != E_OK, err = ctx->init_errcode);
    BREAK_IF_ACTION(ctx->jedec_ready == false, err = E_AGAIN);

    memcpy(jedec, &ctx->jedec, sizeof(qspi_jedec_t));

  } while(0);

  return err;
}

