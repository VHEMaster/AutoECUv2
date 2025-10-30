/*
 * uds.c
 *
 *  Created on: Oct 23, 2025
 *      Author: VHEMaster
 */

#include "uds.h"

error_t uds_init(uds_ctx_t *ctx, const uds_init_ctx_t *init)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(init == NULL, err = E_PARAM);

    memset(ctx, 0u, sizeof(uds_ctx_t));
    memcpy(&ctx->init, init, sizeof(uds_init_ctx_t));

    ctx->initialized = true;

  } while(0);

  return err;
}

error_t uds_configure(uds_ctx_t *ctx, const uds_config_t *config)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->initialized == false, err = E_INVALACT);
    BREAK_IF_ACTION(config == NULL, err = E_PARAM);

    memcpy(&ctx->config, config, sizeof(uds_config_t));

    ctx->configured = true;

  } while(0);

  return err;
}

error_t uds_data_get_error(uds_ctx_t *ctx, uds_error_code_t *code)
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

error_t uds_reset(uds_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->configured == false, err = E_INVALACT);

    ctx->reset_trigger = true;

  } while(0);

  return err;
}

void uds_loop(uds_ctx_t *ctx)
{
  do {
    BREAK_IF(ctx == NULL);
    BREAK_IF(ctx->configured == false);

  } while(0);
}

error_t uds_message_write_upstream(uds_ctx_t *ctx, const uint8_t *payload, uint16_t length)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(payload == NULL || length == 0, err = E_PARAM);
    BREAK_IF_ACTION(ctx->configured == false, err = E_INVALACT);

    // TODO: IMPLEMENT

  } while(0);

  return err;
}

error_t uds_message_read_downstream(uds_ctx_t *ctx, uint8_t *payload, uint16_t *length)
{
  error_t err = E_AGAIN;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(payload == NULL || length == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->configured == false, err = E_INVALACT);

    // TODO: IMPLEMENT

  } while(0);

  return err;
}
