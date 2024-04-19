/*
 * cj125.c
 *
 *  Created on: Apr 6, 2024
 *      Author: VHEMaster
 */

#include <string.h>
#include "cj125.h"
#include "cj125_fsm.h"
#include "compiler.h"

static void cj125_cplt_cb(spi_slave_t *spi_slave, error_t errorcode)
{
  cj125_ctx_t *ctx = (cj125_ctx_t *)spi_slave->usrdata;

  //TODO: is it really needed?
  (void)ctx;
}

error_t cj125_init(cj125_ctx_t *ctx, const cj125_init_ctx_t *init_ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || init_ctx == NULL || init_ctx->spi_slave == NULL, err = E_PARAM);

    memset(ctx, 0u, sizeof(cj125_ctx_t));
    memcpy(&ctx->init, init_ctx, sizeof(cj125_init_ctx_t));
    ctx->init.spi_slave->usrdata = ctx;
    ctx->config.pid_cb_period = CJ125_PID_CB_DEFAULT_PERIOD_US;

    if(gpio_valid(&ctx->init.nrst_pin)) {
      gpio_reset(&ctx->init.nrst_pin);
    }

    err = spi_slave_configure_datasize(ctx->init.spi_slave, 16);
    BREAK_IF(err != E_OK);

    err = spi_slave_configure_mode(ctx->init.spi_slave, CJ125_SPI_MODE);
    BREAK_IF(err != E_OK);

    err = spi_slave_configure_callback(ctx->init.spi_slave, cj125_cplt_cb);
    BREAK_IF(err != E_OK);

    ctx->ready = true;

  } while(0);

  return err;
}

void cj125_loop_main(cj125_ctx_t *ctx)
{

}

void cj125_loop_slow(cj125_ctx_t *ctx)
{

}

void cj125_loop_fast(cj125_ctx_t *ctx)
{
  error_t err = E_OK;
  time_us_t now = time_get_current_us();

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);

    if(ctx->ready) {
      err = cj125_fsm(ctx);
      if(ctx->configured) {
        if(time_diff(now, ctx->pid_cb_timestamp) >= ctx->config.pid_cb_period) {
          if(ctx->config.pid_cb != NULL) {
            ctx->config.pid_cb(ctx, now, ctx->config.pid_cb_usrdata);
          }
        }
      }
    }


  } while(0);

  if(err != E_OK && err != E_AGAIN) {
    //TODO: set error in future
  }
}

error_t cj125_write_config(cj125_ctx_t *ctx, cj125_config_t *config)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || config == NULL, err = E_PARAM);
    BREAK_IF_ACTION(config->ampfactor >= CJ125_AF_MAX , err = E_PARAM);
    BREAK_IF_ACTION(config->curr_to_lambda_relation.items == 0 || config->curr_to_lambda_relation.items >= CJ125_RELATION_ITEMS_MAX, err = E_PARAM);
    BREAK_IF_ACTION(config->res_to_temp_relation.items == 0 || config->res_to_temp_relation.items >= CJ125_RELATION_ITEMS_MAX, err = E_PARAM);
    BREAK_IF_ACTION(config->pump_ref_current >= CJ125_CONFIG_PRC_MAX, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    if(ctx->config_request == false) {
      if(&ctx->config != config) {
        memcpy(&ctx->config, config, sizeof(cj125_config_t));
      }
      ctx->config_errcode = E_AGAIN;
      ctx->config_request = true;
    }
    if(ctx->config_errcode != E_AGAIN) {
      err = ctx->config_errcode;
      ctx->config_request = false;
    } else {
      err = E_AGAIN;
    }

  } while(0);

  return err;
}

error_t cj125_set_ampfactor(cj125_ctx_t *ctx, cj125_af_t ampfactor)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || ampfactor >= CJ125_AF_MAX, err = E_PARAM);

    if(ctx->ampfactor_request == false) {
      if(ctx->ampfactor != ampfactor) {
        ctx->ampfactor = ampfactor;
        ctx->ampfactor_errcode = E_AGAIN;
        ctx->ampfactor_request = true;
      } else {
        err = E_OK;
      }
    }
    if(ctx->ampfactor_errcode != E_AGAIN) {
      err = ctx->ampfactor_errcode;
      ctx->ampfactor_request = false;
    } else {
      err = E_AGAIN;
    }

  } while(0);

  return err;
}

error_t cj125_calib_mode(cj125_ctx_t *ctx, bool enabled)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    if(ctx->calib_accept != ctx->calib_request) {
      err = E_AGAIN;
    } else if(enabled != ctx->calib_request) {
      ctx->calib_request = enabled;
      err = E_AGAIN;
    }

  } while(0);

  return err;
}

error_t cj125_update_ref(cj125_ctx_t *ctx, float ref_voltage)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);

    ctx->data.ref_voltage = ref_voltage;

  } while(0);

  return err;
}

error_t cj125_update_ur(cj125_ctx_t *ctx, float ur_voltage)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);

    ctx->data.ur_voltage = ur_voltage;

  } while(0);

  return err;
}

error_t cj125_update_ua(cj125_ctx_t *ctx, float ua_voltage)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);

    ctx->data.ua_voltage = ua_voltage;

  } while(0);

  return err;
}

error_t cj125_get_data(cj125_ctx_t *ctx, const cj125_data_t *data)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);
    BREAK_IF_ACTION(ctx->configured == false, err = E_NOTRDY);

  } while(0);

  return err;
}
