/*
 * l9960.c
 *
 *  Created on: Apr 6, 2024
 *      Author: VHEMaster
 */

#include <string.h>
#include "l9960.h"
#include "l9960_fsm.h"
#include "l9960_internal.h"
#include "compiler.h"

static void l9960_cplt_cb(spi_slave_t *spi_slave, error_t errorcode)
{
  l9960_ctx_t *ctx = (l9960_ctx_t *)spi_slave->usrdata;

  //TODO: is it really needed?
  (void)ctx;
}

error_t l9960_init(l9960_ctx_t *ctx, const l9960_init_ctx_t *init_ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || init_ctx == NULL || init_ctx->spi_slave == NULL, err = E_PARAM);
    BREAK_IF_ACTION(gpio_valid(&init_ctx->dis_pin) == false && gpio_valid(&init_ctx->ndis_pin) == false, err = E_PARAM);

    memset(ctx, 0u, sizeof(l9960_ctx_t));
    memcpy(&ctx->init, init_ctx, sizeof(l9960_init_ctx_t));
    ctx->init.spi_slave->usrdata = ctx;

    l9960_internal_set_enabled(ctx, false);

    err = spi_slave_configure_datasize(ctx->init.spi_slave, 16);
    BREAK_IF(err != E_OK);

    err = spi_slave_configure_mode(ctx->init.spi_slave, L9960_SPI_MODE);
    BREAK_IF(err != E_OK);

    err = spi_slave_configure_callback(ctx->init.spi_slave, l9960_cplt_cb);
    BREAK_IF(err != E_OK);

    ctx->ready = true;

  } while(0);

  return err;
}

void l9960_loop_main(l9960_ctx_t *ctx)
{

}

void l9960_loop_slow(l9960_ctx_t *ctx)
{

}

void l9960_loop_fast(l9960_ctx_t *ctx)
{
  error_t err = E_OK;

  if(ctx->ready) {
    err = l9960_fsm(ctx);
    if(err != E_OK && err != E_AGAIN) {
      //TODO: set error in future
    }
  }
}

error_t l9960_reset(l9960_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    if(ctx->reset_request == false) {
      ctx->reset_errcode = E_AGAIN;
      ctx->reset_request = true;
    } else if(ctx->reset_errcode != E_AGAIN) {
      err = ctx->reset_errcode;
      ctx->reset_request = false;
    } else {
      err = E_AGAIN;
    }

  } while(0);

  return err;
}

error_t l9960_write_config(l9960_ctx_t *ctx, const l9960_config_t *config)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || config == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->initialized == false, err = E_NOTRDY);

    if(ctx->configure_request == false) {
      if(&ctx->config != config) {
        memcpy(&ctx->config, config, sizeof(l9960_config_t));
      }
      ctx->configure_errcode = E_AGAIN;
      ctx->configure_request = true;
    } else if(ctx->configure_errcode != E_AGAIN) {
      err = ctx->configure_errcode;
      ctx->configure_request = false;
    } else {
      err = E_AGAIN;
    }

  } while(0);

  return err;
}

error_t l9960_hwsc(l9960_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->initialized == false, err = E_NOTRDY);

    if(ctx->hwsc_request == false) {
      ctx->hwsc_errcode = E_AGAIN;
      ctx->hwsc_request = true;
    } else if(ctx->hwsc_errcode != E_AGAIN) {
      err = ctx->hwsc_errcode;
      ctx->hwsc_request = false;
    } else {
      err = E_AGAIN;
    }

  } while(0);

  return err;
}

error_t l9960_diagoff(l9960_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->initialized == false, err = E_NOTRDY);

    if(ctx->diagoff_request == false) {
      ctx->diagoff_errcode = E_AGAIN;
      ctx->diagoff_request = true;
    } else if(ctx->diagoff_errcode != E_AGAIN) {
      err = ctx->diagoff_errcode;
      ctx->diagoff_request = false;
    } else {
      err = E_AGAIN;
    }

  } while(0);

  return err;
}

error_t l9960_set_enabled(l9960_ctx_t *ctx, bool enabled)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->initialized == false, err = E_NOTRDY);
    BREAK_IF_ACTION(ctx->configured == false, err = E_NOTRDY);

    l9960_set_enabled(ctx, enabled);

  } while(0);

  return err;
}

error_t l9960_get_version(l9960_ctx_t *ctx, l9960_version_t *ver)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || ver == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->initialized == false, err = E_NOTRDY);
    BREAK_IF_ACTION(ctx->version_valid != true, err = E_AGAIN);

    memcpy(ver, &ctx->version, sizeof(l9960_version_t));
  } while(0);

  return err;
}

error_t l9960_get_status(l9960_ctx_t *ctx, l9960_status_t *status)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || status == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->initialized == false, err = E_NOTRDY);
    BREAK_IF_ACTION(ctx->status_valid != true, err = E_AGAIN);

    memcpy(status, &ctx->status, sizeof(l9960_status_t));
  } while(0);

  return err;
}

error_t l9960_get_diagnostic(l9960_ctx_t *ctx, l9960_diag_t *diag)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || diag == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->initialized == false, err = E_NOTRDY);
    BREAK_IF_ACTION(ctx->diag_valid != true, err = E_AGAIN);

    memcpy(diag, &ctx->diag, sizeof(l9960_diag_t));
  } while(0);

  return err;
}
