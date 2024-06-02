/*
 * egt.c
 *
 *  Created on: May 14, 2024
 *      Author: VHEMaster
 */

#include "egt.h"
#include "compiler.h"
#include "interpolation.h"
#include "config_tcs.h"
#include <math.h>
#include <string.h>

error_t egt_init(egt_ctx_t *ctx, const egt_init_ctx_t *init_ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || init_ctx == NULL, err = E_PARAM);

    memset(ctx, 0u, sizeof(egt_ctx_t));
    memcpy(&ctx->init, init_ctx, sizeof(egt_init_ctx_t));

    ctx->ready = true;

  } while(0);

  return err;
}

error_t egt_configure(egt_ctx_t *ctx, const egt_config_t *config)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || config == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    if(ctx->configured == true) {
      err = egt_reset(ctx);
      BREAK_IF(err != E_OK);
    }

    ctx->configured = false;

    if(&ctx->config != config) {
      memcpy(&ctx->config, config, sizeof(egt_config_t));
    }

    if(ctx->config.enabled == true) {

      ctx->configured = true;
    }

  } while(0);

  if(err != E_OK) {
    (void)egt_reset(ctx);
  }

  return err;
}

error_t egt_reset(egt_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    ctx->configured = false;

  } while(0);

  return err;
}

void egt_loop_main(egt_ctx_t *ctx)
{

}

void egt_loop_slow(egt_ctx_t *ctx)
{
  error_t err = E_OK;
  time_us_t now = time_get_current_us();

  do {
    BREAK_IF(ctx == NULL);
    if(ctx->configured == true) {
      if(ctx->started == true) {
        err = ecu_devices_tcs_get_data(ctx->config.tcs_instance, &ctx->device_data);
        BREAK_IF(err != E_OK);
        err = ecu_devices_tcs_get_diag(ctx->config.tcs_instance, &ctx->device_diag);
        BREAK_IF(err != E_OK);

        ctx->data.sensor_temperature = ctx->device_data.temperature;
        ctx->data.reference_temperature = ctx->device_data.reference;

        if(ctx->device_diag.byte != 0) {
          ctx->diag.bits.device_error = true;
        }

        if(ctx->data.sensor_temperature > ctx->config.temperature_too_high) {
          ctx->diag.bits.temperature_too_high = true;
        }

        if(ctx->data.sensor_temperature < ctx->config.temperature_too_low) {
          ctx->diag.bits.temperature_too_low = true;
        }

      } else if(time_diff(now, ctx->startup_time) > ctx->config.boot_time) {
        ctx->started = true;
      }
    } else {
      ctx->startup_time = now;
      ctx->data.sensor_temperature = 0;
      ctx->data.reference_temperature = 0;
    }
  } while(0);
}

ITCM_FUNC void egt_loop_fast(egt_ctx_t *ctx)
{

}

error_t egt_get_value(egt_ctx_t *ctx, egt_data_t *data)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(data == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);
    BREAK_IF_ACTION(ctx->configured == false, err = E_NOTRDY);

    *data = ctx->data;

  } while(0);

  return err;
}

error_t egt_get_diag(egt_ctx_t *ctx, egt_diag_t *diag)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(diag == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);
    BREAK_IF_ACTION(ctx->configured == false, err = E_NOTRDY);

    *diag = ctx->diag;

  } while(0);

  return err;
}
