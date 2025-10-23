/*
 * obd2.c
 *
 *  Created on: Oct 23, 2025
 *      Author: VHEMaster
 */

#include "obd2.h"

error_t obd2_init(obd2_ctx_t *ctx, const obd2_init_ctx_t *init)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(init == NULL, err = E_PARAM);

    memset(ctx, 0u, sizeof(obd2_ctx_t));
    memcpy(&ctx->init, init, sizeof(obd2_init_ctx_t));

  } while(0);

  return err;
}

error_t obd2_configure(obd2_ctx_t *ctx, const obd2_config_t *config)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(config == NULL, err = E_PARAM);

    memcpy(&ctx->config, config, sizeof(obd2_config_t));

  } while(0);

  return err;
}

error_t obd2_data_get_error(obd2_ctx_t *ctx, obd2_error_code_t *code)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(code == NULL, err = E_PARAM);

    *code = ctx->error_code;


  } while(0);

  return err;
}

error_t obd2_reset(obd2_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);

    ctx->reset_trigger = true;

  } while(0);

  return err;
}

void obd2_loop(obd2_ctx_t *ctx)
{
  do {
    BREAK_IF(ctx == NULL);

  } while(0);
}
