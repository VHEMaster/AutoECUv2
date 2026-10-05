/*
 * timing_knockwindow.c
 *
 *  Created on: 5 окт. 2026 г.
 *      Author: VHEMaster
 */



#include "config_global.h"
#include "timing_common.h"
#include "config_hw.h"
#include "common.h"
#include "interpolation.h"

error_t knockwindow_init(knockwindow_ctx_t *ctx, const knockwindow_init_ctx_t *init_ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || init_ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(init_ctx->calibration_config == NULL, err = E_PARAM);

    memset(ctx, 0u, sizeof(knockwindow_ctx_t));
    memcpy(&ctx->init, init_ctx, sizeof(knockwindow_init_ctx_t));


    ctx->ready = true;

  } while(0);

  return err;
}

error_t knockwindow_configure(knockwindow_ctx_t *ctx, const knockwindow_config_t *config)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || config == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    ctx->configured = false;

    if(&ctx->config != config) {
      memcpy(&ctx->config, config, sizeof(knockwindow_config_t));
    }

    if(ctx->config.enabled) {

      ctx->configured = true;
    }

  } while(0);

  return err;
}

error_t knockwindow_reset(knockwindow_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    ctx->configured = false;

  } while(0);

  return err;
}

error_t knockwindow_get_runtime_data_ptr(knockwindow_ctx_t *ctx, knockwindow_runtime_ctx_t **runtime_data)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(runtime_data == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    *runtime_data = &ctx->runtime;

  } while(0);

  return err;
}

OPTIMIZE_FAST
ITCM_FUNC void knockwindow_signal_update_callback(knockwindow_ctx_t *ctx)
{

}
