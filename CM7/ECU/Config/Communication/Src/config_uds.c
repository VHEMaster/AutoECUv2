/*
 * config_uds.c
 *
 *  Created on: Oct 23, 2025
 *      Author: vhemaster
 */

#include <string.h>
#include "config_uds.h"
#include "compiler.h"

typedef struct ecu_comm_uds_ctx_tag ecu_comm_uds_ctx_t;

static void ecu_comm_uds_error_callback(uds_ctx_t *ctx, uds_error_code_t code, void *userdata);

typedef struct ecu_comm_uds_ctx_tag {
    uds_config_t config_default;
    uds_init_ctx_t init;
    uds_ctx_t *ctx;
}ecu_comm_uds_ctx_t;

static const uds_config_t ecu_comm_uds_config_default = {

};

static ecu_comm_uds_ctx_t ecu_comm_uds_ctx[ECU_COMM_UDS_MAX] = {
    {
      .init = {
          .error_callback = ecu_comm_uds_error_callback,
          .callback_userdata = NULL,
      },
      .config_default = ecu_comm_uds_config_default,
    },
};

error_t ecu_comm_uds_init(ecu_comm_uds_t instance, uds_ctx_t *ctx)
{
  error_t err = E_OK;
  ecu_comm_uds_ctx_t *uds_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_COMM_UDS_MAX || ctx == NULL, err = E_PARAM);

    uds_ctx = &ecu_comm_uds_ctx[instance];
    uds_ctx->ctx = ctx;

    uds_ctx->init.callback_userdata = (void *)uds_ctx;

    err = uds_init(uds_ctx->ctx, &uds_ctx->init);
    BREAK_IF(err != E_OK);

  } while(0);

  return err;
}

error_t ecu_comm_uds_get_default_config(ecu_comm_uds_t instance, uds_config_t *config)
{
  error_t err = E_OK;
  ecu_comm_uds_ctx_t *uds_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_COMM_UDS_MAX || config == NULL, err = E_PARAM);

    uds_ctx = &ecu_comm_uds_ctx[instance];

    memcpy(config, &uds_ctx->config_default, sizeof(uds_config_t));

  } while(0);

  return err;
}

error_t ecu_comm_uds_configure(ecu_comm_uds_t instance, const uds_config_t *config)
{
  error_t err = E_OK;
  ecu_comm_uds_ctx_t *uds_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_COMM_UDS_MAX || config == NULL, err = E_PARAM);

    uds_ctx = &ecu_comm_uds_ctx[instance];

    err = uds_configure(uds_ctx->ctx, config);

  } while(0);

  return err;
}

error_t ecu_comm_uds_reset(ecu_comm_uds_t instance)
{
  error_t err = E_OK;
  ecu_comm_uds_ctx_t *uds_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_COMM_UDS_MAX, err = E_PARAM);

    uds_ctx = &ecu_comm_uds_ctx[instance];

    err = uds_reset(uds_ctx->ctx);

  } while(0);

  return err;
}

static void ecu_comm_uds_error_callback(uds_ctx_t *ctx, uds_error_code_t code, void *userdata)
{
  // TODO: IMPLEMENT
}
