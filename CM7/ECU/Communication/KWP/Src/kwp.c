/*
 * kwp.c
 *
 *  Created on: Oct 23, 2025
 *      Author: VHEMaster
 */

#include "kwp.h"

error_t kwp_init(kwp_ctx_t *ctx, const kwp_init_ctx_t *init)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(init == NULL, err = E_PARAM);

    memset(ctx, 0u, sizeof(kwp_ctx_t));
    memcpy(&ctx->init, init, sizeof(kwp_init_ctx_t));

    ctx->initialized = true;

  } while(0);

  return err;
}

error_t kwp_configure(kwp_ctx_t *ctx, const kwp_config_t *config)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->initialized == false, err = E_INVALACT);
    BREAK_IF_ACTION(config == NULL, err = E_PARAM);

    memcpy(&ctx->config, config, sizeof(kwp_config_t));

    ctx->configured = true;

  } while(0);

  return err;
}

error_t kwp_data_get_error(kwp_ctx_t *ctx, kwp_error_code_t *code)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->configured == false, err = E_INVALACT);
    BREAK_IF_ACTION(code == NULL, err = E_PARAM);

    *code = ctx->error_code;


  } while(0);

  return err;
}

error_t kwp_reset(kwp_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->configured == false, err = E_INVALACT);

    ctx->reset_trigger = true;

  } while(0);

  return err;
}

void kwp_loop_slow(kwp_ctx_t *ctx)
{
  do {
    BREAK_IF(ctx == NULL);
    BREAK_IF(ctx->configured == false);

  } while(0);
}

void kwp_loop_comm(kwp_ctx_t *ctx)
{
  do {
    BREAK_IF(ctx == NULL);
    BREAK_IF(ctx->configured == false);

  } while(0);
}

void kwp_loop_main(kwp_ctx_t *ctx)
{
  do {
    BREAK_IF(ctx == NULL);
    BREAK_IF(ctx->configured == false);

  } while(0);
}
